from .ringread import main as ringread_main
from .ringwrite import main as ringwrite_main
from .zmqringpub import main as zmqringpub_main
from .zmqringsub import main as zmqringsub_main


def ringread():
    ringread_main()


def ringwrite():
    ringwrite_main()


def zmqringpub():
    zmqringpub_main()


def zmqringsub():
    zmqringsub_main()
