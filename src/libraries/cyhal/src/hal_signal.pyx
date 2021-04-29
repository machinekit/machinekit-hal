from hal_priv cimport (
    hal_data_u, hal_valid_dir, hal_valid_type, sig_value,
    halg_foreach_pin_by_signal,
    )
from hal_util cimport hal2py, py2hal, shmptr
from hal_objectops cimport hal_pin_t, hal_sig_t, hal_comp_t
from hal_const cimport hal_type_t

cdef int _pin_by_signal_cb(hal_pin_t *pin,
                           hal_sig_t *sig,
                           void *user):
    arg =  <object>user
    arg.append(bytes(hh_get_name(&pin.hdr)).decode())
    return 0


cdef class Signal(HALObject):
    cdef int _handle
    cdef hal_data_u *_storage

    def _alive_check(self):
        if self._handle != hh_get_id(&self._o.sig.hdr):
            # the underlying HAL signal was deleted.
            raise RuntimeError("link: underlying HAL signal already deleted")

    def __init__(self, name, type=HAL_TYPE_UNSPECIFIED,
                 init=None, wrap=True, lock=True):
        hal_required()
        # if no type given, wrap existing signal
        if type == HAL_TYPE_UNSPECIFIED:
            self._o.sig = halg_find_object_by_name(lock,
                                                   hal_const.HAL_SIGNAL,
                                                   name.encode()).sig
            if self._o.sig == NULL:
                raise RuntimeError(f"signal '{name}' does not exist")

        else:
            if name in signals:
                if not wrap:
                    raise RuntimeError("Failed to create signal: "
                                       f"a signal with the name '{name}' already exists")
                signal = signals[name]
                if signal.type is not type:
                    raise RuntimeError(
                        f"Failed to create signal: "
                        f"type of existing signal '{describe_hal_type(signal.type)}'"
                        f" does not match type '{describe_hal_type(type)}'"
                    )
            else:
                r = halg_signal_new(lock, name.encode(), type)
                if r:
                    raise RuntimeError(f"Failed to create signal {name}: {hal_lasterror()}")

            self._o.sig = halg_find_object_by_name(lock,
                                                   hal_const.HAL_SIGNAL,
                                                   name.encode()).sig
            if self._o.sig == NULL:
                raise RuntimeError(f"BUG: couldnt lookup signal {name}")

        self._storage = sig_value(self._o.sig)
        self._handle = self.id  # memoize for liveness check
        if init:
            self.set(init)

    def link(self, *pins):
        self._alive_check()
        for p in pins:
            net(self.name, p)

    def __iadd__(self, pins):
        self._alive_check()
        if not hasattr(pins, '__iter__') or isinstance(pins, str):
            pins = [pins]
        for pin in pins:
            if isinstance(pin, str):
                pin = Pin(pin)
            elif not isinstance(pin, Pin):
                raise TypeError(f'linking of {str(pin)} to signal {self.name} not possible')

            net(self, pin)  # net is more verbose than link
        return self

    def __isub__(self, pins):
        if not hasattr(pins, '__iter__') or isinstance(pins, str):
            pins = [pins]
        for pin in pins:
            if isinstance(pin, str):
                pin = Pin(pin)
            if not isinstance(pin, Pin):
                raise TypeError(f'unlinking of {str(pin)} from signal {self.name} not possible')
            if pin.signame != self.name:
                raise RuntimeError(f'cannot unlink: pin {pin.name} is not linked to signal {self.name}')
            pin.unlink()
        return self

    def delete(self):
        # this will cause a handle mismatch if later operating on a deleted signal wrapper
        r = hal_signal_delete(self.name.encode())
        if (r < 0):
            raise RuntimeError(f"Fail to delete signal {self.name}: {hal_lasterror()}")

    def set(self, v):
        self._alive_check()
        if self._o.sig.writers > 0:
            raise RuntimeError(f"Signal {hh_get_name(&self._o.sig.hdr)} already as {self._o.sig.writers} writer(s)")
        return py2hal(self._o.sig.type, self._storage, v)

    def get(self):
        self._alive_check()
        return hal2py(self._o.sig.type, self._storage)

    def pins(self):
        """ return a list of Pin objects linked to this signal """
        self._alive_check()
        pinnames = []
        with HALMutex():
            # collect pin names
            halg_foreach_pin_by_signal(0, self._o.sig, _pin_by_signal_cb,
                                       <void *>pinnames)
            # now the wrapped objects, all under the HAL mutex held:
            pinlist = []
            for n in pinnames:
                pinlist.append(pins.__getitem_unlocked__(n))
            return pinlist

    property writername:
        def __get__(self):
            cdef char *name
            self._alive_check()
            # [] or ['writer']
            s = modifier_name(self._o.sig, HAL_OUT)
            if s:
                return s[0]
            return None

    property bidirname:
        def __get__(self):
            self._alive_check()
            s =  modifier_name(self._o.sig, HAL_IO)
            if s:
                return s
            return None

    property readers:
        def __get__(self):
             self._alive_check()
             return self._o.sig.readers

    property writers:
        def __get__(self):
             self._alive_check()
             return self._o.sig.writers

    property type:
        def __get__(self):
             self._alive_check()
             return self._o.sig.type

    property bidirs:
        def __get__(self):
            self._alive_check()
            return self._o.sig.bidirs


    def __repr__(self):
        return f"<hal.Signal {self.name} {describe_hal_type(self.type)} {self.get()}>"

cdef int _find_writer(hal_object_ptr o,  foreach_args_t *args):
    cdef hal_pin_t *pin
    pin = o.pin
    if signal_of(pin) == args.user_ptr1 and pin.dir == args.user_arg1:
        result =  <object>args.user_ptr2
        result.append(bytes(hh_get_name(o.hdr)).decode())
        if int(pin.dir) == int(HAL_OUT):
            return 1  # stop iteration, there can only be one writer
    return 0 # continue

# find the names of pins which modify a given signal
# can be a name on dir == HAL_OUT or a list of names on HAL_IO
cdef modifier_name(hal_sig_t *sig, int dir):
    names = []
    cdef foreach_args_t args = nullargs
    args.type = hal_const.HAL_PIN
    args.user_arg1 = dir
    args.user_ptr1 = <void *>sig
    args.user_ptr2 = <void *>names

    rc = halg_foreach(1, &args, _find_writer)
    return names



cdef _newsig(str name, hal_type_t hal_type, init=None):
    if not hal_valid_type(hal_type):
        raise TypeError(f"newsig: {name} - invalid type {hal_type}")
    return Signal(name, type=hal_type, init=init, wrap=False)

def newsig(name, hal_type, init=None):
    if not isinstance(name, str):
        raise TypeError(f"newsig: name must be str, not {type(name)}")
    _newsig(name, hal_type, init)
    return signals[name] # add to sigdict

_wrapdict[hal_const.HAL_SIGNAL] = Signal
signals = HALObjectDict(hal_const.HAL_SIGNAL)
