__machinekit-hal__ is a split out repo which just contains the HAL elements of machinekit

It can be built as a RIP and used in the same way as a machinekit RIP, but without the CNC elements.

Install _machinekit-hal-{flavour}_  if using packages.

NB. There is a related repo __machinekit-cnc__, which contains all the CNC elements missing from this repo.
    This can be built as a combined RIP build with machinekit-hal, by invoking scripts/build_with_cnc
    from the root of the machinekit-hal clone.

