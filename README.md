__machinekit-hal__ is a split out repo which just contains the HAL elements of machinekit

It can be built as a RIP and used in the same way as a machinekit RIP, but without the CNC elements.

Install _machinekit-hal-{flavour}_  if using packages.

PR's should not be made directly to this repo without prior notice.

The machinekit repo is periodically cherry-picked for relevant new commits by the developers
and machinekit-hal updated from these.

NB. There is a related repo __machinekit-cnc__, which contains all the CNC elements missing from this repo.

It is possible to install _machinekit-hal-{flavour}_ and then install _machinekit-cnc-{flavour}_ and have
a fully functioning system, albeit there is no advantage to doing so, rather than use the _machinekit_ packages.