#! /bin/bash

set -euo pipefail

IMAGE=gs1encoder-c-doc-build

cd $(dirname $0)
docker build . -t "$IMAGE"

cd ../../..
exec docker run --rm -u `id -u`:`id -g` -v `pwd`:/srv -w /srv/maintenance/docker/c-docs "$IMAGE"

