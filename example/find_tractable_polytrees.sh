#!/bin/sh
CURRENT_DIR=$(pwd)
PROJECT_DIR=$(cd $(dirname $0); cd ..; pwd)
cd $PROJECT_DIR

if [ ! -d "${PROJECT_DIR}/build" ]; then
  cmake -S $PROJECT_DIR -B "${PROJECT_DIR}/build"
  cmake --build "${PROJECT_DIR}/build"
fi

./build/example/search $1

cd $CURRENT_DIR
