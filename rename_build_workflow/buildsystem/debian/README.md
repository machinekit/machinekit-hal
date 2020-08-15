# Machinekit Native and Cross-Building inside container

This builds Debian flavoured Docker images with tools and dependencies for
building/cross-building Machinekit-HAL packages.  The system relies on Debian
Multi-Arch to provide dependency resolution when cross-compiling, and as such
the ability to cross-compile from one platform to another is dependent
on availability of specific packages from upstream distribution repositories.

Special care was taken to allow cross-compiling from `amd64` architecture
to `armhf`, `arm64` and `i386`, respective `i686` architectures. Anything else
is on best effort basis.

## Using the images

- To build the image for specific configuration, you will need system with
  Docker daemon and CLI installed, then Python 3 environment with Python SH
  package (on Debian like systems installed by `sudo apt install python3-sh`)

  Invoke:

  - `cd` into root of Machinekit-HAL repository locally cloned on your computer

  - Run `scripts/buildcontainerimage.py DISTRIBUTION VERSION ARCHITECTURE

    Where:
      - ARCHITECTURE is the HOST architecture for which you want to build the Machinekit-HAL packages
      - VERSION is the version of the supported distribution (for example Buster)
      - DISTRIBUTION is the distro name (for example Debian)

      Currently supported possibilities are listed in `scripts/debian-distro-settings.json`

- To build Machinekit in a Docker container with cross-build tools,
  `cd` into parent directory of a Machinekit-HAL source tree:

        # Bootstrap Machinekit-HAL repository for $TAG and $DISTRIBUTION
		docker run -it --rm -u "$(id -u):$(id -g)" -v "$(pwd):/home/machinekit/build" -w "/home/machinekit/build/machinekit-hal" machinekit-hal-$DISTRIBUTION-builder-v.$TAG scripts/bootstrap

		    # Prepare the changelog for package build
    docker run -it --rm -u "$(id -u):$(id -g)" -v "$(pwd):/home/machinekit/build" -w "/home/machinekit/build/machinekit-hal" machinekit-hal-$DISTRIBUTION-builder-v.$TAG scripts/configure.py -c

		# Build the packagesdocker run -it --rm -u "$(id -u):$(id -g)" -v "$(pwd):/home/machinekit/build" -w "/home/machinekit/build/machinekit-hal" machinekit-hal-$DISTRIBUTION-builder-v.$TAG scripts/buildpackages.py

- To test the Machinekit-HAL repository with `runtests` tool, `cd` into parent directory of a Machinekit-HAL source tree:

        # Run runtests Machinekit-HAL repository for $TAG and $DISTRIBUTION
		docker run -it --rm -u "$(id -u):$(id -g)" -v "$(pwd):/home/machinekit/build" -w "/home/machinekit/build/machinekit-hal" machinekit-hal-$DISTRIBUTION-builder-v.$TAG debian/ripruntests.py