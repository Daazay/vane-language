#!/bin/bash

pushd "$(dirname "$0")/.." > /dev/null

COMPILER=$1
ACTION="gmake"
PREMAKE="./third-party/premake/premake5"
PREMAKE_SCRIPT="./scripts/premake/premake5.lua"
CC_OPTION=""

VALID_COMPILERS=("gcc" "clang")
if [ -n "$COMPILER" ]; then
    valid=false
    for c in "${VALID_COMPILERS[@]}"; do
        if [ "$c" = "$COMPILER" ]; then
            valid=true
            break
        fi
    done

    if ! $valid; then
        echo "Invalid compiler: $COMPILER"
        echo "Available compilers: ${VALID_COMPILERS[*]}"
        exit 1
    fi
    CC_OPTION="--cc=$COMPILER"
else
    CC_OPTION="--cc=gcc"
fi

$PREMAKE --file=$PREMAKE_SCRIPT $ACTION $CC_OPTION