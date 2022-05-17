#!/bin/sh

# path to emsdk_env.sh
# https://emscripten.org/docs/getting_started/downloads.html
EMSDK_ENV_PATH="$HOME/emsdk/emsdk_env.sh"

(
    source $EMSDK_ENV_PATH && cd gui && . ./update-parser.sh
)

cd gui
yarn compile 
