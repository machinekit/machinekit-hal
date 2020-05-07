ARG BASE_IMAGE
ARG VERSION_SHA

FROM ${BASE_IMAGE}

ARG NAME
ARG MAINTAINER
ARG DESCRIPTION
ARG BUILD_DATE
ARG VCS_REF
ARG VCS_URL
LABEL io.machinekit.machinekit-hal.name=${NAME}
LABEL io.machinekit.machinekit-hal.maintainer=${MAINTAINER}
LABEL io.machinekit.machinekit-hal.description=${DESCRIPTION}
LABEL io.machinekit.machinekit-hal.build-date=${BUILD_DATE}
LABEL io.machinekit.machinekit-hal.vcs-ref=${VCS_REF}
LABEL io.machinekit.machinekit-hal.vcs-url=${VCS_URL}

###################################################################
# Build configuration settings

# - Passed in from hooks/build script based on Docker tag
ARG DEBIAN_ARCH
ARG HOST_MULTIARCH
ARG DISTRO_CODENAME
ARG DISTRO_VER
ARG SYS_ROOT
ARG EXTRA_FLAGS
ARG LDEMULATION

# - Set in container environment
ENV DEBIAN_ARCH=${DEBIAN_ARCH}
ENV HOST_MULTIARCH=${HOST_MULTIARCH}
ENV DISTRO_CODENAME=${DISTRO_CODENAME}
ENV DISTRO_VER=${DISTRO_VER}
ENV SYS_ROOT=${SYS_ROOT}
ENV EXTRA_FLAGS=${EXTRA_FLAGS}
ENV LDEMULATION=${LDEMULATION}

###################################################################
# Generic apt configuration

ENV TERM=dumb

# apt config:  silence warnings and set defaults
ENV DEBIAN_FRONTEND=noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN=true
ENV LC_ALL=C.UTF-8
ENV LANGUAGE=C.UTF-8
ENV LANG=C.UTF-8

# turn off recommends on container OS and proot OS
RUN echo 'APT::Install-Recommends "0";\nAPT::Install-Suggests "0";' > \
	    /etc/apt/apt.conf.d/01norecommend

# use stable Debian mirror
ARG DEBIAN_MIRROR=ftp.debian.org
ARG DEBIAN_SECURITY_MIRROR=security.debian.org
RUN sed -i /etc/apt/sources.list \
        -e "s/deb.debian.org/${DEBIAN_MIRROR}/" \
        -e "s/security.debian.org/${DEBIAN_SECURITY_MIRROR}/"

###################################################################
# Add foreign arches and update OS

# add foreign architectures
RUN if test -n "${SYS_ROOT}"; then \
        dpkg --add-architecture $DEBIAN_ARCH; \
    fi

# update Debian OS
RUN apt-get update \
    && apt-get -y upgrade \
    && apt-get clean

###################################################################
# Install utility and dev packages

# Utilities
RUN apt-get -y install \
	libfile-fcntllock-perl \
	locales \
	git \
	bzip2 \
	sharutils \
	net-tools \
	time \
	procps \
	help2man \
	xvfb \
	xauth \
	python-sphinx \
	wget \
	sudo \
	lftp \
	apt-transport-https \
	ca-certificates \
	multistrap \
	debian-keyring \
	debian-archive-keyring \
	python-apt \
	rubygems \
    curl \
    dpkg-sig \
    && { \
	test $DISTRO_VER -ge 10 \
	    || apt-get install -y \
		python-restkit; \
        } \
    && apt-get clean

# Dev tools
RUN apt-get install -y \
	build-essential \
	devscripts \
	fakeroot \
	equivs \
	lsb-release \
	less \
	python-debian \
	libtool \
	ccache \
	autoconf \
	automake \
	quilt \
	psmisc \
	pkg-config \
	qemu-user-static \
    libcmocka-dev \
    python-nose \
	linux-libc-dev:${DEBIAN_ARCH} \
        dh-python \
	libreadline-dev \
    && { \
        if test $DISTRO_VER -ge 9 -a \
	        \( $DEBIAN_ARCH = armhf -o $DEBIAN_ARCH = arm64 \); then \
            apt-get install -y \
	        crossbuild-essential-${DEBIAN_ARCH}; \
        fi; \
    } \
    && apt-get clean

# - amd64 -> i386 multiarch/multilib compiles
RUN if test $DISTRO_VER -eq 8; then \
        apt-get install -y \
	    binutils-i586-linux-gnu \
	    gcc-4.9-multilib \
	    g++-4.9-multilib; \
    elif test $DISTRO_VER -eq 9; then \
        apt-get install -y \
	    binutils-multiarch \
	    gcc-6-multilib \
	    g++-6-multilib \
	    libgcc-6-dev:$DEBIAN_ARCH; \
    else \
        apt-get install -y \
	    binutils-multiarch \
	    gcc-8-multilib \
	    g++-8-multilib \
	    libgcc-8-dev:$DEBIAN_ARCH; \
    fi

# - Linaro gcc cross compiler for armhf, Jessie only
RUN if test $DISTRO_VER -eq 8; then \
        VER=4.9.4-2017.01 && \
        ARCH=arm-linux-gnueabihf && \
        DIR=gcc-linaro-${VER}-x86_64_${ARCH} && \
        TXZ=${DIR}.tar.xz && \
        URI=http://releases.linaro.org/components/toolchain/binaries/latest-4/${ARCH} && \
        WORKD=/tmp/linaro && \
        mkdir -p ${WORKD} && cd ${WORKD} && \
        echo "Downloading gcc-linaro-${VER}" && \
        wget --progress=dot:mega ${URI}/${TXZ} && \
        wget -q ${URI}/${TXZ}.asc && \
        echo "Validating checksum of compiler download" && \
        md5sum -c ${TXZ}.asc && \
        echo "Extracting compiler to /opt/" && \
        tar xCf /opt ${WORKD}/${TXZ} && \
        ln -snf ${DIR} /opt/gcc-linaro-hf && \
        rm -rf ${WORKD} && \
        ln -s ../../bin/ccache /usr/lib/ccache/arm-linux-gnueabihf-gcc && \
        ln -s ../../bin/ccache /usr/lib/ccache/arm-linux-gnueabihf-g++ ; \
    fi

# - Qemu for emulating ARM on amd64
RUN if test $DEBIAN_ARCH = armhf -o $DEBIAN_ARCH = arm64; then \
        apt-get install -y qemu binfmt-support qemu-user-static \
        && apt-get clean; \
    fi

###################################################################
# Build environment

ENV CPPFLAGS="${SYS_ROOT:+--sysroot=$SYS_ROOT ${EXTRA_FLAGS}}"
ENV LDFLAGS="${SYS_ROOT:+--sysroot=$SYS_ROOT ${EXTRA_FLAGS}}"
ENV PKG_CONFIG_PATH="${SYS_ROOT:+$SYS_ROOT/usr/lib/$HOST_MULTIARCH/pkgconfig:$SYS_ROOT/usr/lib/pkgconfig:$SYS_ROOT/usr/share/pkgconfig}"
ENV DPKG_ROOT=$SYS_ROOT

# armhf build root environment
ENV ARM_HOST_MULTIARCH=arm-linux-gnueabihf

# arm64
ENV ARM64_HOST_MULTIARCH=aarch64-linux-gnu

# i386 build root environment
ENV I386_HOST_MULTIARCH=i386-linux-gnu

###########################################
# Monkey-patches and multistrap setup

# Create sysroot directory
RUN test -z "$SYS_ROOT" || mkdir $SYS_ROOT

# Add multistrap configurations
ADD scripts/buildsystem/debian/multistrap-configs /tmp/multistrap-configs/

# Add `{dh_shlibdeps,dpkg-shlibdeps} --sysroot` argument
ADD scripts/buildsystem/debian/dpkg-shlibdeps/dpkg-shlibdeps-*.patch /tmp/
RUN cd / && \
    patch -p0 -F 0 -N < /tmp/dpkg-shlibdeps-$DISTRO_VER.patch && \
    rm /tmp/dpkg-shlibdeps-*.patch
RUN test -z "$SYS_ROOT" \
    || { \
        mkdir -p ${SYS_ROOT}/etc \
        && cp /etc/ld.so.conf ${SYS_ROOT}/etc/ld.so.conf \
        && echo '/lib/arm-linux-gnueabihf\n/usr/lib/arm-linux-gnueabihf' > \
	    /etc/ld.so.conf.d/arm-linux-gnueabihf.conf \
        && echo '/lib/aarch64-linux-gnu\n/usr/lib/aarch64-linux-gnu' > \
	    /etc/ld.so.conf.d/aarch64-linux-gnu.conf; \
    }
# Symlink i586 binutils to i386 so ./configure can find them
RUN test $DISTRO_VER -gt 8 || \
    for i in /usr/bin/i586-linux-gnu-*; do \
	ln -s $(basename $i) $(echo $i | sed 's/i586/i386/'); \
    done
# Give dh_strip the multiarch utility names it needs
RUN test $DISTRO_VER -eq 8 || { \
        ln -s x86_64-linux-gnu-objcopy /usr/bin/i686-linux-gnu-objcopy && \
        ln -s x86_64-linux-gnu-objdump /usr/bin/i686-linux-gnu-objdump && \
        ln -s x86_64-linux-gnu-strip /usr/bin/i686-linux-gnu-strip; \
    }

# Symlink armhf-arch pkg-config, Jessie only
RUN if test $DISTRO_VER -eq 8; then \
        ln -s pkg-config /usr/bin/${ARM_HOST_MULTIARCH}-pkg-config; \
    fi

###################################################################
# Machinekit:  Configure apt

# add Machinekit dependencies package archive
RUN curl -1sLf \
  'https://dl.cloudsmith.io/public/machinekit/machinekit/cfg/setup/bash.deb.sh' \
  | sudo bash \
  && apt-get update

###################################################################
# Machinekit:  Install dependency packages

# Xenomai threads still supported on Jessie
RUN if test $DISTRO_CODENAME = jessie -a -z "$SYS_ROOT"; then \
        apt-get install -y libxenomai-dev \
        && apt-get clean; \
    fi

##############################
# Machinekit:  Configure multistrap

# Set up debian/control for `mk-build-deps`
ADD debian/ /tmp/debian/
RUN MK_CROSS_BUILDER=1 /tmp/debian/configure

# Directory for `mk-build-deps` apt repository
RUN mkdir /tmp/debs && \
    touch /tmp/debs/Sources

# Create deps package and local package repo
RUN if test $DISTRO_VER -eq 8; then \
        mk-build-deps --arch $DEBIAN_ARCH /tmp/debian/control; \
    else \
        mk-build-deps --build-arch $DEBIAN_ARCH --host-arch $DEBIAN_ARCH \
	    /tmp/debian/control; \
    fi \
    && mv *.deb /tmp/debs \
    && ( cd /tmp/debs && dpkg-scanpackages -m . > /tmp/debs/Packages )

# Add deps repo to apt sources for native builds
RUN if test $DISTRO_VER -le 9 -a $DEBIAN_ARCH = amd64; then \
	echo "deb file:///tmp/debs ./" > /etc/apt/sources.list.d/local.list \
	&& apt-get update; \
    fi


##############################
# Machinekit:  Host arch build environment

# Native arch:  Install build dependencies
RUN if test -z "$SYS_ROOT"; then \
        apt-get update \
        && if test $DISTRO_VER -le 9; then \
	    apt-get install -y  -o Apt::Get::AllowUnauthenticated=true \
		machinekit-hal-build-deps \
	    && sed -i /etc/apt/sources.list.d/local.list -e '/^deb/ s/^/#/' \
	    && apt-get update ; \
        else \
	    apt-get install -y /tmp/debs/machinekit-hal-build-deps_*.deb; \
	fi \
	&& apt-get clean; \
    fi

# Patch multistrap on Buster
# https://github.com/volumio/Build/issues/348#issuecomment-462271607
RUN if test $DISTRO_VER -ge 9; then \
        sed -i /usr/sbin/multistrap \
	    -e '/AllowUnauthenticated/ s/"$/ -o Acquire::AllowInsecureRepositories=true"/'; \
    fi

# Foreign arch:  Build "sysroot"
# - Select and unpack build dependency packages
RUN test -z "$SYS_ROOT" \
    || multistrap -f /tmp/multistrap-configs/$DISTRO_CODENAME.conf \
	-a $DEBIAN_ARCH -d $SYS_ROOT
# - Fix symlinks in "sysroot" libdir pointing to `/lib/$MULTIARCH`
RUN test -z "$SYS_ROOT" \
    || { \
        for link in $(find $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/ -type l); do \
            if test $(dirname $(readlink $link)) != .; then \
                ln -sf ../../../lib/${HOST_MULTIARCH}/$(basename \
                    $(readlink $link)) $link; \
            fi; \
        done; \
    }
# - Link tcl/tk setup scripts
RUN test -z "$SYS_ROOT" \
    || { \
        mkdir -p /usr/lib/${HOST_MULTIARCH} && \
        ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/tcl8.6 \
            /usr/lib/${HOST_MULTIARCH} && \
        ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/tk8.6 \
            /usr/lib/${HOST_MULTIARCH}; \
    }

# - Link directories with glib/gtk includes in the wrong place
RUN test -z "$SYS_ROOT" \
    || { \
        ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/glib-2.0 \
	    /usr/lib/${HOST_MULTIARCH} && \
	ln -s $SYS_ROOT/usr/lib/${HOST_MULTIARCH}/gtk-2.0 \
	    /usr/lib/${HOST_MULTIARCH}; \
    }

# - Link `xeno-config` to where autoconf can find it
RUN test -z "$SYS_ROOT" -o $DISTRO_VER -gt 8 || \
    ln -s /sysroot/usr/bin/xeno-config /usr/bin/

##############################
# Machinekit:  Build arch build environment

# Install Multi-Arch: foreign dependencies
RUN apt-get update \
    && apt-get install -y \
        cython \
        uuid-runtime \
        protobuf-compiler \
        python-protobuf \
        python-pyftpdlib \
        python-tk \
        netcat-openbsd \
        tcl8.6 tk8.6 \
        cgroup-tools \
    && if test $DISTRO_VER -le 9; then \
	    apt-get install -y \
		yapps2-runtime yapps2; \
       else \
	    apt-get install -y \
		python-yapps yapps2; \
       fi \
    && apt-get clean

# Monkey-patch entire /usr/include, and re-add build-arch headers
RUN test -z "$SYS_ROOT" \
    || { \
        mv /usr/include /usr/include.build && \
        ln -s $SYS_ROOT/usr/include /usr/include && \
	ln -sf /usr/include.build/x86_64-linux-gnu $SYS_ROOT/usr/include; \
    }

# On Buster, fix krb5-dev .pc files in sysroot
RUN if test $DISTRO_VER -eq 10 -a -n "$SYS_ROOT"; then \
        cd /sysroot/usr/lib/${HOST_MULTIARCH}/pkgconfig/mit-krb5 && \
        for i in *; do \
	    rm ../$i \
	    && ln -s mit-krb5/$i ../$i; \
	done; \
    fi
# On Buster, the libczmq.pc file requires libsystemd but the
# libczmq-dev package doesn't require libsystemd-dev; take care of
# native case (sysroot case taken care of in multistrap config)
RUN if test $DISTRO_VER -eq 10 -a -z "$SYS_ROOT"; then \
        apt-get install -y libsystemd-dev \
        && apt-get clean; \
    fi

###########################################
# Set up environment

RUN echo "ALL ALL=(ALL:ALL) NOPASSWD: ALL" >> /etc/sudoers

RUN addgroup --gid 1000 machinekit && \
    adduser --uid 1000 --ingroup machinekit \
        --home /home/machinekit --shell /bin/bash \
        --disabled-password --gecos "" machinekit

RUN USER=machinekit && \ 
    GROUP=machinekit && \
    curl -SsL https://github.com/boxboat/fixuid/releases/download/v0.4.1/fixuid-0.4.1-linux-amd64.tar.gz | \
        tar -C /usr/local/bin -xzf - && \
    chown root:root /usr/local/bin/fixuid && \
    chmod 4755 /usr/local/bin/fixuid && \
    mkdir -p /etc/fixuid && \
    printf "user: $USER\ngroup: $GROUP\n" > /etc/fixuid/config.yml

USER machinekit

# Customize the following to match the user's environment

ENV PATH=/usr/lib/ccache:/opt/gcc-linaro-hf/bin:/usr/sbin:/usr/bin:/sbin:/bin:${PATH}

# Customize the run environment to your taste
# - bash prompt
# - 'ls' alias
RUN sed -i ~/.bashrc \
    -e 's/^PS1=.*/PS1="\\h:\\W\\$ "/' \
    -e '$a alias ls="ls -aFs"'

ENTRYPOINT ["fixuid"]
