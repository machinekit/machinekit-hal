#####################################################################
# Description:  Dockerfile
#
#               This file, 'Dockerfile', implements Debian styled Docker images
#               used for building, testing and running Machinekit-HAL in CI/CD
#               workflows.
#
# Copyright (C) 2020 -          Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
#   based on original Docker mk-cross-builder images by:
#
# Copyright (C) 2016 - 2019     John Morris  <john AT zultron DOT com>
# Copyright (C) 2016 - 2019     Mick Grant   <arceye AT mgware DOT co DOT uk>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
######################################################################

ARG DEBIAN_DISTRO_BASE

FROM ${DEBIAN_DISTRO_BASE} AS machinekit-hal_base

SHELL [ "bash", "-c" ]

###########################
# Generic apt configuration

ENV TERM=dumb

# Apt config:  silence warnings and set defaults
ENV DEBIAN_FRONTEND=noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN=true
ENV LC_ALL=C.UTF-8
ENV LANGUAGE=C.UTF-8
ENV LANG=C.UTF-8

# Turn off recommends on container OS
RUN printf "%s;\n%s;\n"                 \
    'APT::Install-Recommends "0"'       \
    'APT::Install-Suggests "0"'         \
    > /etc/apt/apt.conf.d/01norecommend

# Add Machinekit Dependencies repository
RUN apt-get update &&                                                             \
    apt-get install -y                                                            \
        curl                                                                      \
        apt-transport-https                                                       \
        lsb-release                                                               \ 
        ca-certificates &&                                                        \
    curl -1sLf                                                                    \
    'https://dl.cloudsmith.io/public/machinekit/machinekit/cfg/setup/bash.deb.sh' \
        | distro="$(lsb_release -is)" codename="$(lsb_release -cs)" bash &&                                                                 \
    apt-get clean

# Update system OS
RUN apt-get update &&     \
    apt-get -y upgrade && \
    apt-get clean

####################################
# Set up Machinekit user environment

ENV USER=machinekit

RUN addgroup --gid 1000 ${USER} &&                            \
    adduser --uid 1000 --ingroup ${USER} --home /home/${USER} \
    --shell /bin/bash --disabled-password --gecos "" ${USER}

RUN apt-get update &&        \
    apt-get install -y       \
        sudo                 \
        machinekit-fixuid && \
    apt-get clean

COPY debian/buildsystem/base-entrypoint.sh /opt/bin/base-entrypoint.sh

RUN chmod +x /opt/bin/base-entrypoint.sh &&                       \
    mkdir /opt/environment &&                                     \
    echo "${USER} ALL=(ALL:ALL) NOPASSWD: ALL" >> /etc/sudoers && \
    mkdir -p /etc/fixuid &&                                       \
    printf "user: ${USER}\ngroup: ${USER}\n" > /etc/fixuid/config.yml

ENTRYPOINT [ "/opt/bin/base-entrypoint.sh" ]

######################################################################

FROM machinekit-hal_base AS machinekit-hal_builder_base

RUN apt-get update &&       \
    apt-get install -y      \
        build-essential     \
        fakeroot            \
        devscripts          \
        equivs              \
        ninja-build         \
        python3-venv        \
        jq &&               \
    apt-get clean;

# Kitware is publishing pre-built binaries only for amd64 and arm64 architectures!
RUN curl -1vLf                                                                                                      \
    $(curl -s https://api.github.com/repos/kitware/cmake/releases/latest |                                          \
        jq -r --arg FILE "cmake-\d{1,}\.\d{1,}\.\d{1,}(-.{1,})?-linux-$(dpkg-architecture -qDEB_BUILD_GNU_CPU)\.sh" \
        '.assets | .[] | select(.name? | match($FILE)) | .browser_download_url')                                    \
        --output /tmp/cmake.sh &&                                                                                   \
    bash /tmp/cmake.sh --skip-license --prefix=/usr/local

# Python 3.9 has --upgrade-deps, but lower versions needs manual action
RUN python3 -m venv /home/${USER}/pybuild        \
            --system-site-packages            && \
        /home/${USER}/pybuild/bin/pip install    \
            --upgrade pip build setuptools

RUN ln -s /home/${USER}/pybuild/bin/activate \
        /opt/environment/pyactivate

######################################################################

FROM machinekit-hal_builder_base AS machinekit-hal_builder

ARG HOST_ARCHITECTURE

# This could be squashed into one layer, but it would require using BuildKit
COPY debian/buildsystem/doctor-multiarch-apt-repositories.sh \
     /doctor-multiarch-apt-repositories.sh
RUN bash /doctor-multiarch-apt-repositories.sh -a ${HOST_ARCHITECTURE} && \
    apt-get update &&                                                     \
    rm -f /doctor-multiarch-apt-repositories.sh

RUN if ! dpkg-architecture -e${HOST_ARCHITECTURE};      \
    then                                                \
        dpkg --add-architecture ${HOST_ARCHITECTURE} && \
        apt-get update;                                 \
    fi

RUN printf "%b"                                               \
           "#!/bin/bash\n"                                    \
           "set -a\n"                                         \
           "$(dpkg-architecture -a${HOST_ARCHITECTURE} -s)\n" \
           "set +a\n"                                         \
           > /opt/environment/dpkg-architecture.sh

COPY debian/ /tmp/debian/

RUN cd /tmp &&                                                                \
    ./debian/bootstrap -s -p /tmp &&                                          \
    mk-build-deps --build-arch=$(dpkg-architecture -qDEB_BUILD_ARCH)          \
                  --host-arch=${HOST_ARCHITECTURE}                            \
                  -ir -t                                                      \
        "apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends -y" \
                  ./debian/control
