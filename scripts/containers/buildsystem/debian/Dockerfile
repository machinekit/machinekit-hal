FROM zultron/docker-cross-builder
MAINTAINER John Morris <john@zultron.com>

###################################################################
# Configure apt for Machinekit

# add Machinekit package archive
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 43DDF224
RUN echo 'deb http://deb.machinekit.io/debian jessie main' > \
        /etc/apt/sources.list.d/machinekit.list


###################################################################
# Install Machinekit dependency packages

##############################
# Configure multistrap

# Set up debian/control for `mk-build-deps`
#     FIXME download parts from upstream
ADD debian/ /tmp/debian/
RUN /tmp/debian/configure -prxt8.6

# Add multistrap configurations
ADD jessie.conf /tmp/
ADD rpi.conf /tmp/

# Directory for `mk-build-deps` apt repository
RUN mkdir /tmp/debs && \
    touch /tmp/debs/Sources


##############################
# Native arch build environment
RUN yes y | mk-build-deps -ir /tmp/debian/control
# Regression test deps
RUN apt-get install -y \
	netcat-openbsd


##############################
# armhf arch build environment

# Create armhf deps package
RUN mk-build-deps -a armhf /tmp/debian/control && \
    mv *.deb /tmp/debs && \
    ( cd /tmp/debs && dpkg-scanpackages -m . > /tmp/debs/Packages )

# Build armhf "sysroot"
# - Select and unpack build dependency packages
RUN multistrap -f /tmp/jessie.conf -a armhf -d $ARM_ROOT
# - Fix symlinks in "sysroot" libdir pointing to `/lib/$MULTIARCH`
RUN for link in $(find $ARM_ROOT/usr/lib/${ARM_HOST_MULTIARCH}/ -type l); do \
        if test $(dirname $(readlink $link)) != .; then \
	    ln -sf ../../../lib/${ARM_HOST_MULTIARCH}/$(basename \
	        $(readlink $link)) $link; \
	fi; \
    done
# - Link tcl/tk setup scripts
RUN mkdir -p /usr/lib/${ARM_HOST_MULTIARCH} && \
    ln -s $ARM_ROOT/usr/lib/${ARM_HOST_MULTIARCH}/tcl8.6 \
        /usr/lib/${ARM_HOST_MULTIARCH} && \
    ln -s $ARM_ROOT/usr/lib/${ARM_HOST_MULTIARCH}/tk8.6 \
        /usr/lib/${ARM_HOST_MULTIARCH}


##############################
# i386 arch build environment

# Create i386 deps package
RUN mk-build-deps -a i386 /tmp/debian/control && \
    mv *.deb /tmp/debs && \
    ( cd /tmp/debs && dpkg-scanpackages -m . > /tmp/debs/Packages )

# Build i386 "sysroot"
# - Select and unpack build dependency packages
RUN multistrap -f /tmp/jessie.conf -a i386 -d $I386_ROOT
# - Fix symlinks in "sysroot" libdir pointing to `/lib/$MULTIARCH`
RUN for link in $(find $I386_ROOT/usr/lib/${I386_HOST_MULTIARCH}/ -type l); do \
        if test $(dirname $(readlink $link)) != .; then \
	    ln -sf ../../../lib/${I386_HOST_MULTIARCH}/$(basename \
	        $(readlink $link)) $link; \
	fi; \
    done
# - Link tcl/tk setup scripts
RUN mkdir -p /usr/lib/${I386_HOST_MULTIARCH} && \
    ln -s $I386_ROOT/usr/lib/${I386_HOST_MULTIARCH}/tcl8.6 \
        /usr/lib/${I386_HOST_MULTIARCH} && \
    ln -s $I386_ROOT/usr/lib/${I386_HOST_MULTIARCH}/tk8.6 \
        /usr/lib/${I386_HOST_MULTIARCH}


##############################
# Raspbian build environment

# (Using armhf deps package)

# Build rpi "sysroot"
# - Select and unpack build dependency packages
RUN multistrap -f /tmp/rpi.conf -a armhf -d $RPI_ROOT
# - Fix symlinks in "sysroot" libdir pointing to `/lib/$MULTIARCH`
RUN for link in $(find $RPI_ROOT/usr/lib/${ARM_HOST_MULTIARCH}/ -type l); do \
        if test $(dirname $(readlink $link)) != .; then \
            ln -sf ../../../lib/${ARM_HOST_MULTIARCH}/$(basename \
                $(readlink $link)) $link; \
        fi; \
    done
# # - Link tcl/tk setup scripts
# RUN mkdir -p /usr/lib/${ARM_HOST_MULTIARCH} && \
#     ln -s $RPI_ROOT/usr/lib/${ARM_HOST_MULTIARCH}/tcl8.6 \
#         /usr/lib/${ARM_HOST_MULTIARCH} && \
#     ln -s $RPI_ROOT/usr/lib/${ARM_HOST_MULTIARCH}/tk8.6 \
#         /usr/lib/${ARM_HOST_MULTIARCH}


