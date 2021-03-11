#!/usr/bin/env sh

#
# Installs YF.
#

INSTALL_DIR=/usr/local
YF_DIR=$INSTALL_DIR/include/yf

# yf
mkdir -p $YF_DIR
cp priv/* pub/* $YF_DIR

sh inc.sh

# ws
make -C ws/ -f Lib.mk install

# cg
make -C cg/ -f Lib.mk install

# sg
#TODO
