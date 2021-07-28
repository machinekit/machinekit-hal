from hal_objectops cimport (
    hal_object_ptr, halg_foreach, foreach_args_t, halg_find_object_by_name,
    hh_get_name, hh_get_id, hh_get_owner_id, hh_get_object_type, hh_get_refcnt,
    hh_incr_refcnt, hh_decr_refcnt,
    hh_valid, hal_object_typestr, hh_snprintf,
    )

# generic finders: find names, count of a given type of object
cdef int _append_name_cb(hal_object_ptr o,  foreach_args_t *args):
    arg =  <object>args.user_ptr1
    arg.append(bytes(hh_get_name(o.hdr)).decode())
    return 0

cdef list object_names(int lock, int type):
    names = []
    cdef foreach_args_t args = nullargs
    args.type = type
    args.user_ptr1 = <void *>names
    halg_foreach(lock, &args, _append_name_cb)
    return names

cdef int object_count(int lock,int type):
    cdef foreach_args_t args = nullargs
    args.type = type
    return halg_foreach(lock, &args, NULL)

# returns the names of directly owned objects of a given type
# owner_id may be id of a comp or an inst
cdef list owned_names(int lock, int type, int owner_id):
    names = []
    cdef foreach_args_t args = nullargs
    args.type = type
    args.owner_id = owner_id
    args.user_ptr1 = <void *>names
    halg_foreach(lock, &args, _append_name_cb)
    return names

# returns the names of all directly or indirectly owned objects
# of a given type
# comp_id MUST be id of a comp
cdef list comp_owned_names(int lock, int type, int comp_id):
    names = []
    cdef foreach_args_t args = nullargs
    args.type = type
    args.owning_comp = comp_id
    args.user_ptr1 = <void *>names
    halg_foreach(lock, &args, _append_name_cb)
    return names


# expose a memory range as a python Buffer
cdef class mview:
    cdef void *base
    cdef int size

    def __cinit__(self, long base, size):
        self.base = <void *>base
        self.size = size

    def __getbuffer__(self, Py_buffer *view, int flags):
        r = PyBuffer_FillInfo(view, self, self.base, self.size, 0, flags)
        view.obj = self

# methods and properties to exposing the common HAL object header.

cdef class HALObject:
    cdef hal_object_ptr _o

    cdef _object_check(self):
        if self._o.any == NULL:
            raise RuntimeError("NULL object header")
        if hh_valid(self._o.hdr) == 0:
            raise RuntimeError("invalid object detected")

    def delete(self):
        # Subclasses may define this
        pass

    property name:
        def __get__(self):
            self._object_check()
            return bytes(hh_get_name(self._o.hdr)).decode()

    property id:
        def __get__(self):
            self._object_check()
            return hh_get_id(self._o.hdr)

    property handle:  # deprecated name
        def __get__(self):
            return self.id

    property owner_id:
        def __get__(self):
            self._object_check()
            return hh_get_owner_id(self._o.hdr)

    property object_type:
        def __get__(self):
            self._object_check()
            return hh_get_object_type(self._o.hdr)

    property strtype:
        def __get__(self):
            self._object_check()
            return hal_object_typestr(hh_get_object_type(self._o.hdr))

    property strhdr:
        def __get__(self):
            self._object_check()
            cdef char buf[100]
            hh_snprintf(buf, sizeof(buf), self._o.hdr)
            return buf

    property refcnt:
        def __get__(self):
            self._object_check()
            return hh_get_refcnt(self._o.hdr)

    property valid:
        def __get__(self):
            return hh_valid(self._o.hdr) == 1

    cdef incref(self):
        self._object_check()
        return hh_incr_refcnt(self._o.hdr)

    cdef decref(self):
        self._object_check()
        return hh_decr_refcnt(self._o.hdr)
