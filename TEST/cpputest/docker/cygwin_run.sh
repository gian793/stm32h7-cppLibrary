#!/bin/bash

TAG=jwgrenning/cpputest-runner

HOST_WORKDIR=$(cygpath -w "${PWD}")
WORKDIR=/home
COMMAND=${1:-/bin/bash}

docker run \
  --rm \
  --name cpputest-runner \
  --volume "${HOST_WORKDIR}":"${WORKDIR}" \
  --workdir "${WORKDIR}" \
  -it $TAG \
  $COMMAND
