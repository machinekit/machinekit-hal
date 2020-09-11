import os
import contextlib
import pytest
import subprocess

allowed_float_error = 1.0e-6
DEBUG=os.environ.get("DEBUG", "5")

def setup():
    e=os.environ.copy()
    e["DEBUG"] = DEBUG
    subprocess.call("realtime restart", shell=True,stderr=subprocess.STDOUT,env=e)

def teardown():
    e=os.environ.copy()
    e["DEBUG"] = DEBUG

    # When `rtapi_app` exits, it may try to kill the python process;
    # ignore it
    (lambda s=__import__('signal'):
         s.signal(s.SIGTERM, s.SIG_IGN))()

    subprocess.call("realtime stop", shell=True,stderr=subprocess.STDOUT,env=e)

@contextlib.contextmanager
def fixture(name):
    setup()
    yield()
    teardown()

@pytest.fixture(scope="module")
def realtime(request):
    with contextlib.ExitStack() as stack:
        yield [stack.enter_context(fixture("CLASS"))]


@pytest.fixture
def fnear():
    def fnear_func(f1,f2):
        global allowed_float_error
        return abs(f1 - f2) <= allowed_float_error

    return fnear_func
