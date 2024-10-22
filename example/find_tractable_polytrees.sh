#!/bin/sh
CURRENT_DIR=$(pwd)
PROJECT_DIR=$(cd $(dirname $0); cd ..; pwd)

if [ ! -d "${PROJECT_DIR}/build" ]; then
  cmake -S $PROJECT_DIR -B "${PROJECT_DIR}/build"
  cmake --build "${PROJECT_DIR}/build"
fi

SEARCH_COMMAND="${PROJECT_DIR}/build/example/search"
eval $SEARCH_COMMAND $PROJECT_DIR "$@"
