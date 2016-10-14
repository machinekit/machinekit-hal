FROM zultron/docker-cross-builder
MAINTAINER John Morris <john@zultron.com>

env DEBIAN_ARCH=armhf
env SYS_ROOT=$ARM_ROOT
env HOST_MULTIARCH=$ARM_HOST_MULTIARCH

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
# Arch build environment

# Create deps package
RUN mk-build-deps -a $DEBIAN_ARCH /tmp/debian/control && \
    mv *.deb /tmp/debs && \
    ( cd /tmp/debs && dpkg-scanpackages -m . > /tmp/debs/Packages )

# Build "sysroot"
# - Select and unpack build dependency packages
RUN multistrap -f /tmp/jessie.conf -a $DEBIAN_ARCH -d $SYS_ROOT
# - Fix symlinks in "sysroot" libdir pointing to `/lib/$MULTIARCH`
RUN for link in $(find $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/ -type l); do \
        if test $(dirname $(readlink $link)) != .; then \
	    ln -sf ../../../lib/${HOST_MULTIARCH}/$(basename \
	        $(readlink $link)) $link; \
	fi; \
    done
# - Link tcl/tk setup scripts
RUN mkdir -p /usr/lib/${HOST_MULTIARCH} && \
    ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/tcl8.6 \
        /usr/lib/${HOST_MULTIARCH} && \
    ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/tk8.6 \
        /usr/lib/${HOST_MULTIARCH}

