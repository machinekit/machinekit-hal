# net - link signal and pins by name
import sys

from hal_const cimport (
    HAL_BIT, HAL_FLOAT,HAL_S32,HAL_U32,HAL_S64,HAL_U64,
    HAL_TYPE_UNSPECIFIED, HAL_IN, HAL_OUT, HAL_IO,
    )

cdef _dirdict =  { HAL_IN : "HAL_IN",
                   HAL_OUT : "HAL_OUT",
                   HAL_IO:"HAL_IO" }

cdef _typedict = {  HAL_BIT : "HAL_BIT",
                    HAL_FLOAT : "HAL_FLOAT",
                    HAL_S32 : "HAL_S32",
                    HAL_U32 : "HAL_U32",
                    HAL_S64 : "HAL_S64",
                    HAL_U64 : "HAL_U64"   }

cdef _pindir(int dir):
    return _dirdict.get(dir, "HAL_DIR_UNSPECIFIED")

cdef _haltype(int type):
    return _typedict.get(type, "HAL_TYPE_UNSPECIFIED")


def net(sig,*pinnames):
    cdef int writers = 0, bidirs = 0, t = HAL_TYPE_UNSPECIFIED
    writer_name = None
    bidir_name = None

    if len(pinnames) == 0:
        raise RuntimeError("net: at least one pin name expected")

    signame = None
    if isinstance(sig, Pin) \
       or (isinstance(sig, str) and (sig in pins)):
        pin = sig
        if isinstance(sig, str):
            pin = pins[sig]

        if not (pin.dir == HAL_OUT or pin.dir == HAL_IO):
            raise RuntimeError('net: pin must have dir HAL_OUT or HAL_IO to create a signal')

        if not pin.signal:
            signame = pin.name.replace('.', '-')
            net(signame, pin)
        signame = pin.signal.name

    elif isinstance(sig, Signal):
        signame = sig.name
    elif isinstance(sig, str):
        signame = sig
    else:
        raise TypeError("net: first argument must be a signal or pin")

    s = None
    writer = None
    if signame in signals: #  pre-existing net type
        s = signals[signame]
        writers = s.writers
        bidirs = s.bidirs
        t = s.type

    if signame in pins:
        raise TypeError(f"net: '{signame}' is a pin - first argument must be a signal name")

    pinlist = []
    for names in pinnames:
        if not hasattr(names, '__iter__') or isinstance(names, str):
            names = [names]
        for pin in names:
            if isinstance(pin, str):
                pin = pins[pin]  # get wrappers - will raise KeyError if pin dont exist
            elif not isinstance(pin, Pin):
                RuntimeError('net: Can only link pins to signals')
            pinlist.append(pin)

    for w in pinlist:

        if w.linked:
            if w.signame == signame:  # already on same signal
                continue
            # linked already - but to another signal
            raise RuntimeError(f"net: pin '{w.name}'  was already linked to signal '{w.signame}'")

        if t == HAL_TYPE_UNSPECIFIED: # no pre-existing type, use this pin's type
            t = w.type

        if t != w.type: # pin type doesnt match net type
            raise TypeError(
                f"net: signal '{signame}' of type '{_haltype(t)}' cannot add pin"
                f" '{w.name}' of type '{_haltype(w.type)}'\n"
            )

        if w.dir == HAL_OUT:
            if writers:
                if not writer:
                    writer = pins[s.writername]
                raise TypeError(
                    f"net: signal '{signame}' can not add writer pin '{w.name}', "
                    f"it already has {_pindir(writer.dir)} pin '{writer.name}'"
                )
            writer = w
            writers += 1

        if w.dir == HAL_IO:
            if writers:
                if not writer:
                    writer = pins[s.writername]
                raise TypeError(
                    f"net: signal '{signame}' can not add writer pin '{w.name}', "
                    f"it already has {_pindir(writer.dir)} pin '{writer.name}'"
                )

            bidir = w
            bidirs += 1

    if not s:
        s = Signal(signame, type=t, wrap=False)

    if not pinlist:
        raise RuntimeError("'net' requires at least one pin, none given")

    for p in pinlist:
        r = hal_link(p.name.encode(), signame.encode())
        if r:
            raise RuntimeError(f"Failed to link pin {p.name} to {signame}: {r} - {hal_lasterror()}")

    return s
