#!/usr/bin/env bash
set -euxo pipefail
docker pull skylyrac/blocksds:slim-latest
docker run --rm -v "$GITHUB_WORKSPACE:/work" -w /work skylyrac/blocksds:slim-latest make
test -s AetherOS5.nds
