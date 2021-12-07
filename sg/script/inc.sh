#!/usr/bin/env sh

#
# Updates SG local include directory.
#

VAR_DIR=~/var
INCLUDE_DIR=$VAR_DIR/include

cp -uv include/* $INCLUDE_DIR/yf/sg/
