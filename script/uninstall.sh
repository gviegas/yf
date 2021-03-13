#!/usr/bin/env sh

#
# Uninstalls YF.
#

INSTALL_DIR=/usr/local
YF_DIR=$INSTALL_DIR/include/yf
CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# sg
#TODO

# cg
make -C $CG_DIR -f Lib.mk uninstall

# ws
make -C $WS_DIR -f Lib.mk uninstall

# yf
rm $YF_DIR/*.h
rmdir --ignore-fail-on-non-empty $YF_DIR
