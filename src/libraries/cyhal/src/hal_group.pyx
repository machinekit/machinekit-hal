from hal_util cimport shmptr #hal2py, py2hal, shmptr, valid_dir, valid_type
from hal_priv cimport MAX_EPSILON, hal_data
from hal_objectops cimport halg_find_object_by_id
from hal_group cimport (
    hal_compiled_group_t, hal_group_t, halg_group_new, hal_cgroup_match,
    hal_cgroup_free, halpr_group_compile, halg_member_new, halg_member_delete,
    hal_unref_group,
    )
from rtapi cimport  RTAPI_BIT_TEST


cdef class Group(HALObject):
    cdef hal_compiled_group_t *_cg

    def __cinit__(self, str name, int arg1=0, int arg2=0, wrap=False, bool lock=True):
        hal_required()
        self._cg = NULL
        self._o.group = halg_find_object_by_name(lock,
                                                 hal_const.HAL_GROUP,
                                                 name.encode()).group
        if wrap:
            self._o.group = halg_find_object_by_name(
                0, hal_const.HAL_GROUP, name.encode()).group
            if self._o.group == NULL:
                raise RuntimeError(f"halpr_find_group_by_name({name}) failed")
        elif self._o.group == NULL:
            # not found, create a new group
            r = halg_group_new(lock, name.encode(), arg1, arg2)
            if r:
                raise RuntimeError(f"Failed to create group {name}: {hal_lasterror()}")
            self._o.group = halg_find_object_by_name(lock,
                                                     hal_const.HAL_GROUP,
                                                     name.encode()).group
            if self._o.group == NULL:
                raise InternalError(f"BUG: cannot find group '{name}'")

        else:
            # if wrapping existing group and args are nonzero, they better match up
            if arg1:
                if self.userarg1 != arg1:
                    raise RuntimeError(f"userarg1 does not match for existing group {name}: {arg1}, was {self.userarg1}")
            if arg2:
                if self.userarg2 != arg2:
                    raise RuntimeError(f"userarg2 does not match for existing group {name}: {arg2}, was {self.userarg2}")

    def members(self):  # member wrappers
        return [members[n] for n in owned_names(1, hal_const.HAL_MEMBER, self.id)]

    def signals(self):  # members resolved into signal names
        return [signals[n] for n in owned_names(1, hal_const.HAL_MEMBER, self.id)]

    def changed(self):
        cdef hal_sig_t *s
        if self._cg == NULL:
            self.compile()
        if self._cg.n_monitored == 0:
            return []
        if hal_cgroup_match(self._cg) == 0:
            return []
        result = []
        for i in range(self._cg.n_members):
            if RTAPI_BIT_TEST(self._cg.changed, i):
                s = <hal_sig_t *>shmptr(self._cg.member[i].sig_ptr)
                result.append(signals[bytes(hh_get_name(&s.hdr)).decode()])
        return result

    def compile(self):
        with HALMutex():
            hal_cgroup_free(self._cg)
            rc =  halpr_group_compile(self.name.encode(), &self._cg)
            if rc < 0:
                raise RuntimeError(f"hal_group_compile({self.name}) failed: {hal_lasterror()}")
        hal_unref_group(self.name.encode())

    def member_add(self, member, int arg1=0, int eps_index=0): 
        if isinstance(member, Signal):
            member = member.name
        rc = halg_member_new(1, self.name.encode(), member.encode(), arg1, eps_index)
        if rc:
            raise RuntimeError(f"Failed to add member '{member}' to  group '{self.name}': {hal_lasterror()}")

    def member_delete(self, member):
        if isinstance(member, Signal):
            member = member.name
        rc = halg_member_delete(1, self.name.encode(), member)
        if rc:
            raise RuntimeError(f"Failed to delete member '{member}' from  group '{self.name}': {hal_lasterror()}")
    property userarg1:
        def __get__(self): return self._o.group.userarg1
        def __set__(self, int r): self._o.group.userarg1 = r

    property userarg2:
        def __get__(self): return self._o.group.userarg2
        def __set__(self, int r): self._o.group.userarg2 = r


cdef class Member(HALObject):
    cdef hal_sig_t *s
    cdef hal_group_t *g

    def __cinit__(self, *args,  init=None, int userarg1=0, eps=0,
                  wrap=True, lock=True):
        hal_required()
        if not wrap:
            # create a new member and wrap it
            group = args[0]
            member = args[1]
            r = halg_member_new(lock, group.encode(), member.encode(),
                                userarg1, eps)
            if r:
                raise RuntimeError(f"Failed to create group {group} member {member}: {r} {hal_lasterror()}")
        else:
            member = args[0]
        #  member now must exist, look it up
        self._o.member = halg_find_object_by_name(lock,
                                                  hal_const.HAL_MEMBER,
                                                  member.encode()).member
        if self._o.member == NULL:
            raise RuntimeError(f"no such member {member}")

    property group:
        def __get__(self):
            cdef hal_object_ptr owner_grp = halg_find_object_by_id(
                False, hal_const.HAL_GROUP, self.owner_id)
            name_b = bytes(hh_get_name(owner_grp.hdr))
            return groups[name_b.decode()]

    property sig:
        def __get__(self):
            if self._o.member.sig_ptr == 0:
                raise InternalError("BUG: __call__: sig_ptr zero")
            # signal has same name as member
            return signals[self.name]

    property epsilon:
        def __get__(self):
            return hal_data.epsilon[self._o.member.eps_index]

    property eps:
        def __get__(self): return self._o.member.eps_index
        def __set__(self, int eps):
            if (eps < 0) or (eps > MAX_EPSILON-1):
                raise InternalError(f"member {self.name} of {self.group.name}: epsilon index {eps} out of range")
            self._o.member.eps_index = eps

    property userarg1:
        def __get__(self): return self._o.member.userarg1


    def __repr__(self):
        return f"<hal.Member {self.name} of {self.group.name}>"

_wrapdict[hal_const.HAL_GROUP] = Group
groups = HALObjectDict(hal_const.HAL_GROUP)

_wrapdict[hal_const.HAL_MEMBER] = Member
members = HALObjectDict(hal_const.HAL_MEMBER)
