# Helper Scripts

## distribute-python

This script must be run whenever the protobuf files are changed. To
prevent problems when protoc is not installed on a system it makes
sense to dristribute the generated Python files.

Run this script from the root directory.

```bash
./script/distribute-python
```
## Buildsystem DOCKER container images

Folders `scripts/buildsystem` and `scripts/containers/buildsystem` contain
the tools to create Docker images for building, testing and Debian package
creation of Machinekit-HAL

Heed README.md in folder `scripts/buildsystem`

Build images by invoking script `scripts/buildcontainerimage.py ${DISTRIBUTION} ${VERSION} ${ARCHITECTURE}`