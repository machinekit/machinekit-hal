FROM zultron/docker-cross-builder:@DISTRO@
MAINTAINER John Morris <john@zultron.com>

###################################################################
# Build configuration settings

env DEBIAN_ARCH=@DEBIAN_ARCH@
env SYS_ROOT=@SYS_ROOT@
env HOST_MULTIARCH=@HOST_MULTIARCH@
env DISTRO=@DISTRO@
env EXTRA_FLAGS=@EXTRA_FLAGS@

###################################################################
# Environment (computed)

ENV CPPFLAGS="--sysroot=$SYS_ROOT ${EXTRA_FLAGS}"
ENV LDFLAGS="--sysroot=$SYS_ROOT ${EXTRA_FLAGS}"
ENV PKG_CONFIG_PATH="$SYS_ROOT/usr/lib/$HOST_MULTIARCH/pkgconfig:$SYS_ROOT/usr/lib/pkgconfig:$SYS_ROOT/usr/share/pkgconfig"
ENV DPKG_ROOT=$SYS_ROOT
ENV PATH=/usr/lib/ccache:/usr/sbin:/usr/bin:/sbin:/bin:/usr/local/bin:$SYS_ROOT/usr/bin

###################################################################
# Configure apt for Machinekit

# add Machinekit package archive
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 43DDF224
RUN echo "deb http://deb.machinekit.io/debian jessie main" > \
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
# Host arch build environment

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

# - Link directories with glib/gtk includes in the wrong place
RUN test $DEBIAN_ARCH = amd64 || { \
	ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/glib-2.0 \
	    /usr/lib/${HOST_MULTIARCH}; \
	ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/gtk-2.0 \
	    /usr/lib/${HOST_MULTIARCH}; \
	}

##############################
# Build arch build environment

# Install Multi-Arch: foreign dependencies
RUN apt-get install -y \
        cython \
        uuid-runtime \
        protobuf-compiler \
        python-protobuf \
        python-pyftpdlib \
        python-tk \
        netcat-openbsd \
        tcl8.6 tk8.6 \
        libxenomai-dev

# Monkey-patch entire /usr/include, and re-add build-arch headers
RUN test $DEBIAN_ARCH = amd64 || { \
        mv /usr/include /usr/include.build && \
        ln -s $SYS_ROOT/usr/include /usr/include; \
	ln -sf /usr/include.build/x86_64-linux-gnu $SYS_ROOT/usr/include; \
    }
