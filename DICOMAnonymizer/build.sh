#!/bin/bash
BUILD_DIR=./build
TOOL_CHAIN_PATH=/home/stas/projects/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
if [ ! -d "$BUILD_DIR" ]
then
    mkdir "$BUILD_DIR"
fi
cd build \
    && cmake .. -DCMAKE_TOOLCHAIN_FILE="$TOOL_CHAIN_PATH" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=. \
    && cmake --build . \
    && cmake --install .