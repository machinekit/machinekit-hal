# generic dictionary of HAL objects.
# instantiated with object type.

cdef class HALObjectDict:

    cdef int  _type
    cdef dict _objects

    def __cinit__(self, int type_):
        #hal_required()
        if not type_ in _wrapdict:
            raise RuntimeError(f"unsupported type {type_}")
        self._type = type_
        self._objects = dict()

    # supposed to be 'private' - must be called
    # with HAL mutex held (!)

    def __getitem_unlocked__(self, name):
        hal_required()

        # this one is silly - and slow:
        # array-type indexing with ints
        # leave out for now, no good use case.
        # if isinstance(name, int):
        #     return object_names(0, self._type)[name]

        if name in self._objects:
            # already wrapped
            return self._objects[name]

        cdef hal_object_ptr ptr
        ptr = halg_find_object_by_name(0, self._type, name.encode())
        if ptr.any == NULL:
            raise NameError(f"no such {hal_object_typestr(self._type)} '{name}'")
        method = _wrapdict[self._type]
        w = method(name, lock=False, wrap=True)
        # add new wrapper
        self._objects[name] = w
        return w

    def __getitem__(self, name):
        hal_required()
        with HALMutex():
            return self.__getitem_unlocked__(name)

    def __contains__(self, arg):
        hal_required()
        if isinstance(arg, HALObject):
            arg = arg.name
        try:
            self.__getitem__(arg)
            return True
        except NameError:
            return False

    def __len__(self):
        hal_required()
        return object_count(1, self._type)

    def __delitem__(self, name):
        hal_required()
        # this calls the wrapper dtor and deletes the underlying HAL
        # object
        self._objects[name].delete()
        del self._objects[name]

    def __call__(self):
        hal_required()
        return object_names(1, self._type)

    def __repr__(self):
        hal_required()
        d = {name : self[name] for name in object_names(1, self._type)}
        return str(d)

    def __iter__(self):
        for name in object_names(1, self._type):
            yield name

# example instantiation:
# _wrapdict[hal_const.HAL_INST] = Instance
# instances = HALObjectDict(hal_const.HAL_INST)
