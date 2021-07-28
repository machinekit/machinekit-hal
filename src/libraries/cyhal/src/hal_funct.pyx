

def addf(func, thread, int position=-1, rmb=False, wmb=False):
    c_func = func.encode() if isinstance(func, str) else func
    c_thread = thread.encode() if isinstance(thread, str) else thread
    hal_required()
    r = hal_add_funct_to_thread(c_func, c_thread, position, rmb, wmb)
    if r:
        raise RuntimeError(f"hal_add_funct_to_thread({func}, {thread}) failed: {r} {hal_lasterror()}")


def delf(func, thread):
    c_func = func.encode() if isinstance(func, str) else func
    c_thread = thread.encode() if isinstance(thread, str) else thread
    hal_required()
    r = hal_del_funct_from_thread(c_func, c_thread)
    if r:
        raise RuntimeError(f"hal_del_funct_from_thread({func}, {thread}) failed: {r} {hal_lasterror()}")
