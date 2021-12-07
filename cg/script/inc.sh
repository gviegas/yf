#!/usr/bin/env sh

#
# Updates CG local include directory.
#

VAR_DIR=~/var
INCLUDE_DIR=$VAR_DIR/include

cp -uv include/* $INCLUDE_DIR/yf/cg/
