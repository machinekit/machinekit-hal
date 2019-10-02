#!/bin/bash -e
#
# Build Docker container images

USAGE="Usage:  $0 <docker_id>/<docker_repo>:<tag>"

# Get image name from cmdline
IMAGE_NAME=${1:-${IMAGE_NAME:?$USAGE}}
# Extract image tag from image name
CACHE_TAG=${IMAGE_NAME##*:}
# Set Dockerfile path
DOCKERFILE_PATH=Dockerfile

# Build configuration variables for each tag
# - Architecture settings
case "${CACHE_TAG}" in
    amd64_*)
        DEBIAN_ARCH="amd64"
        SYS_ROOT=
        HOST_MULTIARCH="x86_64-linux-gnu"
        EXTRA_FLAGS=
        LDEMULATION=elf_x86_64
        ;;
    armhf_*)
        DEBIAN_ARCH="armhf"
        SYS_ROOT="/sysroot"
        HOST_MULTIARCH="arm-linux-gnueabihf"
        EXTRA_FLAGS=
        LDEMULATION=armelf_linux_eabi
        ;;
    arm64_*)
        DEBIAN_ARCH="arm64"
        SYS_ROOT="/sysroot"
        HOST_MULTIARCH="aarch64-linux-gnu"
        EXTRA_FLAGS=
        LDEMULATION=aarch64linux
        ;;
    i386_*)
        DEBIAN_ARCH="i386"
        SYS_ROOT="/sysroot"
        HOST_MULTIARCH="i386-linux-gnu"
        EXTRA_FLAGS="-m32"
        LDEMULATION="elf_i386"
        ;;
    *)
        echo "Unknown tag '${CACHE_TAG}'" >&2
        exit 1
        ;;
esac
# - Distro settings
case "${CACHE_TAG}" in
    *_8)
        DISTRO_CODENAME="jessie"
        BASE_IMAGE="debian:jessie"
        DISTRO_VER="8"
        ;;
    *_9)
        DISTRO_CODENAME="stretch"
        BASE_IMAGE="debian:stretch"
        DISTRO_VER="9"
        ;;
    *_10)
        DISTRO_CODENAME="buster"
        BASE_IMAGE="debian:buster"
        DISTRO_VER="10"
        ;;
    *)
        echo "Unknown tag '${CACHE_TAG}'" >&2
        exit 1
        ;;
esac

# Debug info
(
    echo "Build settings:"
    echo "    IMAGE_NAME=${IMAGE_NAME}"
    echo "    CACHE_TAG=${CACHE_TAG}"
    echo "    DEBIAN_ARCH=${DEBIAN_ARCH}"
    echo "    SYS_ROOT=${SYS_ROOT}"
    echo "    HOST_MULTIARCH=${HOST_MULTIARCH}"
    echo "    DISTRO_CODENAME=${DISTRO_CODENAME}"
    echo "    BASE_IMAGE=${BASE_IMAGE}"
    echo "    DISTRO_VER=${DISTRO_VER}"
    echo "    EXTRA_FLAGS=${EXTRA_FLAGS}"
    echo "    LDEMULATION=${LDEMULATION}"
) >&2

# Be sure we're in the right directory
cd "$(dirname $0)"

# Build the image
set -x
exec docker build \
       --build-arg DEBIAN_ARCH=${DEBIAN_ARCH} \
       --build-arg SYS_ROOT=${SYS_ROOT} \
       --build-arg HOST_MULTIARCH=${HOST_MULTIARCH} \
       --build-arg DISTRO_CODENAME=${DISTRO_CODENAME} \
       --build-arg BASE_IMAGE=${BASE_IMAGE} \
       --build-arg DISTRO_VER=${DISTRO_VER} \
       --build-arg EXTRA_FLAGS=${EXTRA_FLAGS} \
       --build-arg LDEMULATION=${LDEMULATION} \
       -f $DOCKERFILE_PATH -t $IMAGE_NAME .
