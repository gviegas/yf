#!/usr/bin/env sh

#
# Creates local work tree.
#

VAR_DIR=~/var
INCLUDE_DIR=$VAR_DIR/include
LIB_DIR=$VAR_DIR/lib
BIN=$VAR_DIR/bin
CACHE_DIR=$VAR_DIR/cache
SHARE_DIR=$VAR_DIR/share

mkdir -pv $VAR_DIR $INCLUDE_DIR $LIB_DIR $BIN $CACHE_DIR $SHARE_DIR

# yf
mkdir -pv $INCLUDE_DIR/yf $CACHE_DIR/yf $SHARE_DIR/yf
cp -uv pub/* priv/* $INCLUDE_DIR/yf/

# ws
mkdir -pv $INCLUDE_DIR/yf/ws $CACHE_DIR/yf/ws $SHARE_DIR/yf/ws
cp -uv ws/include/* $INCLUDE_DIR/yf/ws/

# cg
mkdir -pv $INCLUDE_DIR/yf/cg $CACHE_DIR/yf/cg $SHARE_DIR/yf/cg
cp -uv cg/include/* $INCLUDE_DIR/yf/cg/

# sg
mkdir -pv $INCLUDE_DIR/yf/sg $CACHE_DIR/yf/sg $SHARE_DIR/yf/sg
cp -uv sg/include/* $INCLUDE_DIR/yf/sg/
