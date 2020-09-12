
# RTAPI python bindings
# shared memory
# RT logger
# rtapi_app command interface

from os import strerror, getpid
from libc.stdlib cimport malloc, free
from cpython.bytes cimport PyBytes_FromString
from cpython.buffer cimport PyBuffer_FillInfo
from rtapi_app cimport (
    rtapi_connect, rtapi_newthread, rtapi_delthread,
    rtapi_loadrt, rtapi_unloadrt, rtapi_newinst, rtapi_delinst, rtapi_cleanup
    )

_HAL_KEY                   = HAL_KEY
_RTAPI_KEY                 = RTAPI_KEY
_RTAPI_RING_SHM_KEY        = RTAPI_RING_SHM_KEY
_GLOBAL_KEY                = GLOBAL_KEY
_DEFAULT_MOTION_SHMEM_KEY  = DEFAULT_MOTION_SHMEM_KEY
_SCOPE_SHM_KEY             = SCOPE_SHM_KEY

_KERNEL = MSG_KERNEL
_RTUSER = MSG_RTUSER
_ULAPI = MSG_ULAPI

MSG_NONE = RTAPI_MSG_NONE
MSG_ERR =  RTAPI_MSG_ERR
MSG_WARN = RTAPI_MSG_WARN
MSG_INFO = RTAPI_MSG_INFO
MSG_DBG = RTAPI_MSG_DBG
MSG_ALL = RTAPI_MSG_ALL

cdef class mview:
    cdef void *base
    cdef int size

    def __cinit__(self, long base, size):
        self.base = <void *>base
        self.size = size

    def __getbuffer__(self, Py_buffer *view, int flags):
        r = PyBuffer_FillInfo(view, self, self.base, self.size, 0, flags)
        view.obj = self

cdef class RtapiModule:
    cdef unicode _name
    cdef int _id

    def __cinit__(self, unicode name):
        self._id = -1
        if len(name) == 0:
            pyname = f"cyrtapimodule{getpid()}"
            self._name = pyname
        else:
           self._name = name
        self._id = rtapi_init(self._name.encode())
        if self._id < 0:
            raise RuntimeError(f"Fail to create RTAPI - realtime not running?  {strerror(-self._id)}")

    def __dealloc__(self):
        if self._id > 0:
            #print "---__dealloc__ rtapi_exit", self._name
            rtapi_exit(self._id)

    def shmem(self, int key, unsigned long size = 0):
        cdef void *ptr
        cdef int shmid

        shmid = rtapi_shmem_new(key, self._id, size)
        if shmid < 0:
            raise RuntimeError("shm segment 0x%x/%d does not exist" % (key,key))
        retval = rtapi_shmem_getptr(shmid, &ptr);
        if retval < 0:
            raise RuntimeError("getptr shm 0x%x/%d failed %d" % (key,key,retval))
        retval = rtapi_shmem_getsize(shmid, &size);
        if retval < 0:
            raise RuntimeError("getsize shm 0x%x/%d failed %d" % (key,key,retval))
        return memoryview(mview(<long>ptr, size))

    property name:
        def __get__(self): return self._name

    property id:
        def __get__(self): return self._id


# duck-type a File object so one can do this:
# log = RTAPILogger(level=rtapi.MSG_ERR,tag="marker")
# print >> log, "some message",
# and it will got to the shared RT log handled by msgd

cdef class RTAPILogger:
    cdef int _level
    cdef char *_tag

    def __init__(self, tag="cython", int level=RTAPI_MSG_ALL):
        tag_b = tag.encode()
        rtapi_required()
        rtapi_set_logtag(tag_b)
        self._level = level
        self._tag = tag_b

    def write(self,line):
        cdef bytes py_bytes = line.rstrip(" \t\f\v\n\r").encode()
        cdef char *c_string = py_bytes
        rtapi_print_msg(self._level, "%s", c_string)

    def flush(self):
        pass

    property tag:
        def __get__(self): return self._tag
        def __set__(self, char *tag):
            self._tag = tag
            rtapi_set_logtag(tag)

    property level:
        def __get__(self): return self._level
        def __set__(self, int level): self._level = level


# helper - mutate a string list into a char**argv type array, zero-terminated
# with a NULL pointer
cdef char ** _to_argv(args):
    l = len(args)
    cdef char **ret = <char **>malloc((l+1) * sizeof(char *))
    if not ret:
        raise MemoryError()
    for i in range(l):
        py_bytes = args[i].encode()
        ret[i] = py_bytes
    ret[l] =  NULL # zero-terminate
    return ret

import sys
import os
from machinekit import hal, config
if sys.version_info >= (3, 0):
    import configparser
else:
    import ConfigParser as configparser

# enums for classify_comp
CS_NOT_LOADED = 0
CS_NOT_RT = 1
CS_RTLOADED_NOT_INSTANTIABLE = 2
CS_RTLOADED_AND_INSTANTIABLE = 3

autoload = True  #  autoload components on newinst

# classifies a component for newinst
def classify_comp(comp):
    if comp not in hal.components:
        return CS_NOT_LOADED
    c = hal.components[comp]
    if c.type != hal.TYPE_RT:
        return CS_NOT_RT
    if not c.has_ctor:
        return CS_RTLOADED_NOT_INSTANTIABLE
    return CS_RTLOADED_AND_INSTANTIABLE


class RTAPIcommand:
    """ connect to the rtapi_app RT demon to pass commands """

    def __init__(self, unicode uuid="", int instance=0, unicode uri=""):
        rtapi_required()
        if uuid == "" and uri == "":  # try to get the uuid from the ini
            mkconfig = config.Config()
            mkini = os.getenv("MACHINEKIT_INI")
            if mkini is None:
                mkini = mkconfig.MACHINEKIT_INI
            if not os.path.isfile(mkini):
                raise RuntimeError("MACHINEKIT_INI " + mkini + " does not exist")

            cfg = configparser.ConfigParser()
            cfg.read(mkini)
            try:
                uuid = cfg.get("MACHINEKIT", "MKUUID")
            except configparser.NoSectionError or configparser.NoOptionError:
                raise RuntimeError("need either a uuid=<uuid> or uri=<uri> parameter")
        c_uri = uri.encode()
        c_uuid = uuid.encode()
        r = rtapi_connect(instance, c_uri, c_uuid)
        if r:
            raise RuntimeError("cant connect to rtapi: %s" % strerror(-r))

    def newthread(self, str name, int period, instance=0, fp=0, cpu=-1,
                  cgname="", flags=0):
        c_cgname = cgname.encode()
        r = rtapi_newthread(instance, name.encode(), period, cpu, c_cgname, fp, flags)
        if r:
            raise RuntimeError(f"rtapi_newthread failed:  {strerror(-r)}")

    def delthread(self, name, instance=0):
        r = rtapi_delthread(instance, name.encode())
        if r:
            raise RuntimeError(f"rtapi_delthread failed:  {strerror(-r)}")

    def loadrt(self, *args, instance=0, **kwargs):
        cdef char** argv

        if len(args) < 1:
            raise RuntimeError("loadrt needs at least the module name as argument")
        name = args[0]
        for key in kwargs.keys():
            args +=(f'{key}={str(kwargs[key])}', )
        c_name = bytes(name.encode())
        argv = _to_argv(args[1:])
        r = rtapi_loadrt( instance, c_name, <const char **>argv)
        free(argv)
        if r:
            raise RuntimeError(f"rtapi_loadrt '{args}' failed: {strerror(-r)}")

        return hal.components[name.split('/')[-1]]

    def unloadrt(self, str name, int instance=0):
        if len(name) == 0:
            raise RuntimeError("unloadrt needs at least the module name as argument")
        r = rtapi_unloadrt( instance, name.encode())
        if r:
            raise RuntimeError(f"rtapi_unloadrt '{name}' failed: {strerror(-r)}")

    def newinst(self, *args, instance=0, **kwargs):
        cdef char** argv

        if len(args) < 2:
            raise RuntimeError("newinst needs at least module and instance name as argument")
        comp = args[0]
        instname = args[1]
        for key in kwargs.keys():
            args +=(f'{key}={str(kwargs[key])}', )

        status = classify_comp(comp)
        if status is CS_NOT_LOADED:
            if autoload:  # flag to prevent creating a new instance
                c_comp = bytes(comp.encode())
                argv = _to_argv([])
                rtapi_loadrt(instance, c_comp, <const char **>argv)
                free(argv)
            else:
                raise RuntimeError(f"component '{comp}' not loaded\n")
        elif status is CS_NOT_RT:
            raise RuntimeError(f"'{comp}' not an RT component\n")
        elif status is CS_RTLOADED_NOT_INSTANTIABLE:
            raise RuntimeError(f"legacy component '{comp}' loaded, but not instantiable\n")
        elif status is CS_RTLOADED_AND_INSTANTIABLE:
            pass  # good

        #TODO check singleton

        if instname in hal.instances:
            raise RuntimeError(f'instance with name {instname} already exists')

        c_comp = bytes(comp.encode())
        c_instname = bytes(instname.encode())
        argv = _to_argv(args[2:])
        r = rtapi_newinst(instance, c_comp, c_instname, <const char **>argv)
        free(argv)
        if r:
            raise RuntimeError(f"rtapi_newinst '{args}' failed: {strerror(-r)}")

        return hal.instances[instname]

    def delinst(self, instname, instance=0):
        r = rtapi_delinst(instance, instname.encode())
        if r:
            raise RuntimeError(f"rtapi_delinst '{instname}' failed: {strerror(-r)}")


# default module to connect to RTAPI:
__rtapimodule = None
cdef rtapi_required():
    global __rtapimodule
    if not __rtapimodule:
        __rtapimodule = RtapiModule("")
    if not __rtapimodule:
        raise RuntimeError("cant connect to RTAPI - realtime not running?")

def _atexit():
    if __rtapimodule:
        rtapi_exit(__rtapimodule.id)

import atexit
atexit.register(_atexit)

(lambda s=__import__('signal'):
     s.signal(s.SIGTERM, s.default_int_handler))()

# global RTAPIcommand to use in HAL config files
__rtapicmd = None
def init_RTAPI(**kwargs):
    global __rtapicmd
    if not __rtapicmd:
        __rtapicmd = RTAPIcommand(**kwargs)
        for method in dir(__rtapicmd):
            if callable(getattr(__rtapicmd, method)) and method not in ('__init__', '__class__'):
                setattr(sys.modules[__name__], method, getattr(__rtapicmd, method))
    else:
        raise RuntimeError('RTAPIcommand already initialized')
    if not __rtapicmd:
        raise RuntimeError('unable to initialize RTAPIcommand - realtime not running?')


# make sure to close the zmq socket when done
def _cleanup_rtapi():
    rtapi_cleanup()

import atexit
atexit.register(_cleanup_rtapi)
