#!/usr/bin/env bash

DIR=".wasm"

if [ ! -d $DIR ]; then
  mkdir $DIR
fi

# NOTE `-std`: To use modern c++11 features like std::tuple and std::vector,
# we need to enable C++ 11 by passing the parameter to gcc through emcc.
emcc src/*.cc \
  -std=c++11 \
  -O1 \
  -I src/eigen3 \
  -s WASM=1 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s WASM_ASYNC_COMPILATION=0 \
  -s SINGLE_FILE=1 \
  -s MODULARIZE=1 \
  -s ASSERTIONS=1 \
  --bind \
  -o $DIR/main.js
cp $DIR/main.js ./dist/lib.js
