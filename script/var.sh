#!/usr/bin/env sh

#
# Creates local work tree.
#

VAR=~/var
INCLUDE=$VAR/include
BIN=$VAR/bin
CACHE=$VAR/cache
SHARE=$VAR/share

mkdir -pv $VAR $INCLUDE $BIN $CACHE $SHARE

# yf
mkdir -pv $INCLUDE/yf $CACHE/yf $SHARE/yf
cp -v pub/* priv/* $INCLUDE/yf/

# ws
mkdir -pv $INCLUDE/yf/ws $CACHE/yf/ws $SHARE/yf/ws
cp -v ws/include/* $INCLUDE/yf/ws/

# cg
mkdir -pv $INCLUDE/yf/cg $CACHE/yf/cg $SHARE/yf/cg
cp -v cg/include/* $INCLUDE/yf/cg/

# sg
mkdir -pv $INCLUDE/yf/sg $CACHE/yf/sg $SHARE/yf/sg
cp -v sg/include/* $INCLUDE/yf/sg/
