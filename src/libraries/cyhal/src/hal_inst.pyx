from hal_util cimport shmptr
from hal_objectops cimport hal_comp_t
from hal_priv cimport halg_inst_delete, halpr_find_owning_comp

cdef class Instance(HALObject):
    cdef hal_comp_t *_comp # owning comp

    def __cinit__(self, name, wrap=True, lock=True):
        hal_required()
        # no support for direct instance creation, or should we
        # go through rtapi_command?

        # NB: self._o lives in the HALObject
        self._o.inst = halg_find_object_by_name(lock,
                                                hal_const.HAL_INST,
                                                name.encode()).inst
        if self._o.inst == NULL:
            raise RuntimeError(f"instance {name} does not exist")

    def __del__(self):
        # delete wrapper
        instances.pop(self.name, None)
        # and the HAL object
        r = halg_inst_delete(1, self.name.encode())
        if (r < 0):
            raise RuntimeError(f"Fail to delete instance {self._name}: {hal_lasterror()}")

    property owner:
        def __get__(self):
            if self._comp == NULL:
                with HALMutex():
                    self._comp = halpr_find_owning_comp(self.id)
            if self._comp == NULL:
                raise RuntimeError(
                    f"BUG: Failed to find  owning comp {self.id} of instance {self.name}: {hal_lasterror()}"
                )
            # XXX use get_unlocked here!
            return Component(bytes(hh_get_name(&self._comp.hdr)).decode(), wrap=True)


    property pins:
        def __get__(self):
            """ return a list of Pin objects owned by this instance"""
            pinnames = []
            with HALMutex():
                # collect pin names
                pinnames = owned_names(0, hal_const.HAL_PIN,
                                       hh_get_id(&self._o.inst.hdr))
                pinlist = []
                for n in pinnames:
                    pinlist.append(pins.__getitem_unlocked__(n))
                return pinlist

    def pin(self, name, base=None):
        """ return component Pin object, base does not need to be supplied if pin name matches component name """
        if base is None:
            base = self.name
        return Pin(f'{base}.{name}')

    property size:
        def __get__(self): return self._o.inst.inst_size

    property blob:
        """ return a MemoryView of this instances HAL"""
        def __get__(self):
            cdef void *ptr
            ptr = shmptr(self._o.inst.inst_data_ptr)
            return memoryview(mview(<long>ptr, self._o.inst.inst_size))

_wrapdict[hal_const.HAL_INST] = Instance
instances = HALObjectDict(hal_const.HAL_INST)
