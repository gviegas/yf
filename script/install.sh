#!/usr/bin/env sh

#
# Installs YF.
#

INSTALL_DIR=/usr/local
YF_DIR=$INSTALL_DIR/include/yf
CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# yf
mkdir -p $YF_DIR
cp priv/* pub/* $YF_DIR

# ws
make -C $WS_DIR -f Lib.mk install

# cg
make -C $CG_DIR -f Lib.mk install

# sg
#TODO
