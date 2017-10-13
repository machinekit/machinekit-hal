# Machinekit Cross-Builder

This builds Debian Docker images with tools and dependencies for
building/cross-building Machinekit packages.  For cross-building
`armhf` and `i386` architectures, the image contains a `multistrap`
system root tree, and for native `amd64` builds the image contains
needed dependencies and tools in the root filesystem.

## Using the builder

- Determine the 'tag' for the desired architecture and distro
  combination.  The format is `$ARCH_$DISTRO`, where `$ARCH` may be
  one of `amd64`, `i386`, `armhf`; `$DISTRO` may be one of
  `8` for Jessie, `9` for Stretch; e.g. `armhf_9`.

- Pull or build desired Docker image `$TAG`:

  - Pull a pre-built image:

        # Pull image, `armhf_9` tag, from Docker Hub
        docker pull zultron/mk-cross-builder:$TAG

  - Or, build from `Dockerfile`:

        # Clone this repository; `cd` into the directory
        git clone https://github.com/zultron/mk-cross-builder.git
        cd mk-cross-builder

        # Build Docker image
        ./mk-cross-builder.sh build $TAG

- To build Machinekit in a Docker container with cross-build tools,
  `cd` into a Machinekit source tree:

        # Build source packages and amd64 binary packages for Stretch
		scripts/build_docker -t amd64_9 -c deb

		# Build amd64 binary-only packages (no source) for Jessie
		scripts/build_docker -t amd64_8 -c deb -n

		# Build amd64 (default) RIP build with regression tests
		scripts/build_docker -c test

		# Build armhf binary-only packages for Stretch
		scripts/build_docker -t armhf_9 -c deb

		# Run command in `armhf_9` container
		scripts/build_docker -t armhf_9 bash -c 'echo $HOST_MULTIARCH'

	- Note that source packages are only built by default on
      `amd64_*`, the native architecture.

- Querying packages in the sysroot:

Knowing what's in the multistrap filesystem can be difficult.  These
commands can help.  Run them from inside a container (see above).

        # Installed packages
        dpkg-query --admindir=$DPKG_ROOT/var/lib/dpkg -p libczmq-dev

        # Apt cache
        apt-cache -o Dir::State=$DPKG_ROOT/var/lib/apt/ show libczmq-dev


# Setting up automated `mk-cross-builder` image builds

- Fork this repo into a GitHub account
- In the GitHub repo "Settings", "Integrations & Services" tab, from
  the "Add Service" drop-down, select "Docker".
- If you haven't already, create a hub.docker.com account and link it
  with GitHub.
- From the "Create" drop-down menu (upper right), select "Create
  Automated Build".
- Select the GitHub "mk-cross-builder" repo and "Create" it.
- For each tag to be auto-built, enter the following:
  - Name: `master`
  - Dockerfile Location:  `/Dockerfile.$TAG`
  - Docker Tag Name: `$TAG`
- "Save Changes"

Now, either trigger builds with the "Build Settings" tab "Trigger"
button on hub.docker.com, or else push new commits to the master
branch on GitHub.

# Setting up automated Machinekit builds

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
