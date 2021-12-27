# the loadusr function
import subprocess
import shlex
import time


def loadusr(command, wait=False, wait_name=None, wait_timeout=5.0, shell=False, **kwargs):
    cmd = command

    for key in kwargs.keys():  # pass arguments using kwargs
        arg = kwargs[key]
        prefix = '--'
        if len(key) == 1:  # single length arguments have single prefix
            prefix = '-'
        if isinstance(arg, bool):  # boolean is store_true type argument
            if arg is True:
                cmd += f' {prefix}{key}'
        else:
            cmd += f' {prefix}{key} {str(arg)}'

    if not shell:
        cmd = shlex.split(cmd)  # correctly split the command
    p = subprocess.Popen(cmd, shell=shell)

    wait = wait or (wait_name is not None)

    if not wait:
        return

    if wait_name is None:
        wait_name = command.split(' ')[0]  # guess the name

    timeout = 0.0
    while True:
        p.poll()
        # check if process is alive
        ret = p.returncode
        if ret is not None:
            raise RuntimeError(f"{' '.join(cmd)} exited with return code {ret}")
        # check if component exists
        if (wait_name is not None) and (wait_name in components) and (components[wait_name].state == COMP_READY):
            return components[wait_name]
        # check for timeout
        if timeout >= wait_timeout:
            p.kill()
            raise RuntimeError(f'{command} did not start')

        timeout += 0.1
        time.sleep(0.1)
