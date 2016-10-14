# Machinekit Cross-Builder

This builds a Docker image containing `multistrap` system root trees
for Debian Jessie `armhf` and `i386` architectures, with tools in the
native system to cross-build Machinekit packages.

The image is based on the [`docker-cross-builder` image][1].  For more
information, see that repo.

[1]: https://github.com/zultron/docker-cross-builder

## Using the builder

- Pull or build one of the Docker images, `amd64`, `i386`, `armhf` or
  `raspbian`.
  - Pull image, `armhf` tag, from Docker Hub

            docker pull zultron/mk-cross-builder:armhf

  - Or, build image from `Dockerfile`
	- Clone this repository and `cd` into the directory

            git clone https://github.com/zultron/mk-builder-3.git
            cd mk-builder-3

	- Build Docker image for `armhf`

	        ./mk-cross-builder.sh build armhf

- Start the Docker image
  - If `$MK_CROSS_BUILDER` is the path to this clone (and `$MK` is the
    path to the Machinekit clone)

            cd $MK
            $MK_CROSS_BUILDER/mk-cross-builder.sh

- To build software, see the [`docker-cross-builder` repo][1]
  instructions.
