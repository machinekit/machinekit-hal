# Machinekit Cross-Builder

This builds a Docker image containing `multistrap` system root trees
for Debian Jessie `armhf` and `i386` architectures, with tools in the
native system to cross-build Machinekit packages.

The image is based on the [`docker-cross-builder` image][1].  For more
information, see that repo.

[1]: https://github.com/zultron/docker-cross-builder

## Using the builder

- Pull/build desired Docker image: `amd64`, `i386`, `armhf`,
  `raspbian`

  - Pull:

        # Pull image, `armhf` tag, from Docker Hub
        docker pull dovetailautomata/mk-cross-builder:armhf

  - Build from `Dockerfile`:

        # Clone this repository; `cd` into the directory
        git clone https://github.com/dovetailautomata/mk-cross-builder.git
        cd mk-cross-builder

        # Build Docker image for `armhf`
        ./mk-cross-builder.sh build armhf

- To build Machinekit in a Docker container with cross-build tools:
  see Machinekit repo `scripts/build_docker`; from the Machinekit base
  directory:

        # Build source packages and amd64 binary packages
		scripts/build_docker -t amd64 -c deb

		# Build amd64 binary-only packages (no source)
		scripts/build_docker -t amd64 -c deb -n

		# Build amd64 (default) RIP build with regression tests
		scripts/build_docker -c test

		# Build armhf binary-only packages
		scripts/build_docker -t armhf -c deb

	    # Interactive shell in raspbian cross-builder
        scripts/build_docker -t raspbian

		# Run command in `armhf`
		scripts/build_docker bash -c 'echo $HOST_MULTIARCH'

	- Note that `-t amd64` is the default, and source package build is
      default only on `amd64`.  With no `-c`, 
