gcc -c -O2 -g -Wall -Wa,-a,-ad -funwind-tables -fno-omit-frame-pointer -I. -pthread -DTHREAD_FLAVOR_ID=0 -DRTAPI -D_GNU_SOURCE -D_FORTIFY_SOURCE=0 -DPB_FIELD_32BIT '-DPB_SYSTEM_HEADER=<'machinetalk'/include/pb-hal.h>' -D__MODULE__ -I. -I./inifile -I./protobuf/nanopb -I./rtapi -I./hal/lib -I./machinetalk/nanopb -DSEQUENTIAL_SUPPORT -DHAL_SUPPORT -DDYNAMIC_PLCSIZE -DRT_SUPPORT -DOLD_TIMERS_MONOS_SUPPORT -DMODBUS_IO_MASTER -mieee-fp  -fPIC hal/icomp-example/lutnv2.c



#-o objects/posix/hal/icomp-example/lutnv2.o
