# Machinekit Cross-Builder

This builds a Docker image containing `multistrap` system root trees
for Debian Jessie `armhf` and `i386` architectures, with tools in the
native system to cross-build Machinekit packages.

The image is based on the [`docker-cross-builder` image][1].  For more
information, see that repo.

[1]: https://github.com/Dovetail-Automata/docker-cross-builder

## Using the builder

- Pull/build desired Docker image: `amd64`, `i386`, `armhf`,
  `raspbian`

  - Pull:

        # Pull image, `armhf` tag, from Docker Hub
        docker pull dovetailautomata/mk-cross-builder:armhf

  - Build from `Dockerfile`:

        # Clone this repository; `cd` into the directory
        git clone https://github.com/Dovetail-Automata/mk-cross-builder.git
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

- Querying packages in the sysroot:

        # Installed packages
        dpkg-query --admindir=$DPKG_ROOT/var/lib/dpkg -p libczmq-dev

        # Apt cache
        apt-cache -o Dir::State=$DPKG_ROOT/var/lib/apt/ show libczmq-dev


# Setting up automated builds

- Create Github organization
  - In GitHub account:  Create organization, e.g. `my-mk-pkgs`
  - Fork Machinekit repo into organization
  - Enable Docker builds on repo
	- Click the "Settings" tab
	- Click "Integrations & Services" on the left
	- Click "Add Service", and find and click "Docker"
- Set up Travis CI
  - Create account
  - Link to GitHub
  - On upper right, drop-down menu, select 'accounts'
  - Click "Review and add your authorized organizations"; goes back to GH
    - `my-mk-pkgs` org should be checked
  - If org not on left side, click "Sync account" button
  - Click org on left side
	- Flip switch for repo on
  - Go to repo; drop-down "More options" menu choose "Settings"
  - Turn these "General settings" on:
	- Build only if `.travis.yml` is present
	- Build pushes
	- Build pull requests
- Set up Packagecloud.io
  - Create account with free plan
  - Upper right drop-down menu, pick "API Token"; copy
  - On Travis CI repo settings, add these variables:
	- `PACKAGECLOUD_USER`:  enter your user name  (Display value: on)
	- `PACKAGECLOUD_REPO`:  `machinekit` (Display value: on)
	- `PACKAGECLOUD_TOKEN`:  paste your GH API token  (Display value: **OFF**)
