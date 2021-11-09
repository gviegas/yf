#!/usr/bin/env sh

#
# Removes YF lib/header/build files and directories.
#

CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

VAR_DIR=~/var
INCLUDE_DIR=$VAR_DIR/include
CACHE_DIR=$VAR_DIR/cache
SHARE_DIR=$VAR_DIR/share

# sg
make -C $SG_DIR -f Lib.mk clean
rmdir -v --ignore-fail-on-non-empty \
			$INCLUDE_DIR/yf/sg $CACHE_DIR/yf/sg $SHARE_DIR/yf/sg

# cg
make -C $CG_DIR -f Lib.mk clean
rmdir -v --ignore-fail-on-non-empty \
			$INCLUDE_DIR/yf/cg $CACHE_DIR/yf/cg $SHARE_DIR/yf/cg

# ws
make -C $WS_DIR -f Lib.mk clean
rmdir -v --ignore-fail-on-non-empty \
			$INCLUDE_DIR/yf/ws $CACHE_DIR/yf/ws $SHARE_DIR/yf/ws

# yf
rm -fv $INCLUDE_DIR/yf/*.h
rmdir -v --ignore-fail-on-non-empty $INCLUDE_DIR/yf $CACHE_DIR/yf $SHARE_DIR/yf
