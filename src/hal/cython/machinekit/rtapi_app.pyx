
def connect(char *uuid, instance=0, uri=None):
    r = rtapi_connect(instance, uri, uuid)
    if r:
        raise RuntimeError(f"cant connect to rtapi: {rpc_error()}")
