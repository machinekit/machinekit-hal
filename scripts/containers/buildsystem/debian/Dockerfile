FROM zultron/docker-cross-builder
MAINTAINER John Morris <john@zultron.com>

env DEBIAN_ARCH=
env SYS_ROOT=
env HOST_MULTIARCH=
env DISTRO=

###################################################################
# Configure apt for Machinekit

# add Machinekit package archive
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 43DDF224
RUN echo "deb http://deb.machinekit.io/debian $DISTRO main" > \
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
ADD $DISTRO.conf /tmp/

# Directory for `mk-build-deps` apt repository
RUN mkdir /tmp/debs && \
    touch /tmp/debs/Sources

# Create deps package
RUN mk-build-deps -a $DEBIAN_ARCH /tmp/debian/control && \
    mv *.deb /tmp/debs && \
    ( cd /tmp/debs && dpkg-scanpackages -m . > /tmp/debs/Packages )

# Add deps repo to apt sources
RUN echo "deb file:///tmp/debs ./" > /etc/apt/sources.list.d/local.list

# Update apt cache
RUN apt-get update


##############################
# Arch build environment

# Build "sysroot"
# - Select and unpack build dependency packages
RUN if test $DEBIAN_ARCH = amd64; then \
        apt-get install -y  -o Apt::Get::AllowUnauthenticated=true \
            machinekit-build-deps; \
    else \
        multistrap -f /tmp/$DISTRO.conf -a $DEBIAN_ARCH -d $SYS_ROOT; \
    fi
# - Fix symlinks in "sysroot" libdir pointing to `/lib/$MULTIARCH`
RUN if ! test $DEBIAN_ARCH = amd64; then \
        for link in $(find $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/ -type l); do \
            if test $(dirname $(readlink $link)) != .; then \
                ln -sf ../../../lib/${HOST_MULTIARCH}/$(basename \
                    $(readlink $link)) $link; \
            fi; \
        done; \
    fi
# - Link tcl/tk setup scripts
RUN test $DEBIAN_ARCH = amd64 || { \
        mkdir -p /usr/lib/${HOST_MULTIARCH} && \
        ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/tcl8.6 \
            /usr/lib/${HOST_MULTIARCH} && \
        ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/tk8.6 \
            /usr/lib/${HOST_MULTIARCH}; \
        }

