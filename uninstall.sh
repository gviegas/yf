#!/usr/bin/env sh

#
# Uninstalls YF.
#

INSTALL_DIR=/usr/local
YF_DIR=$INSTALL_DIR/include/yf

# sg
#TODO

# cg
make -C cg/ -f Lib.mk uninstall

# ws
make -C ws/ -f Lib.mk uninstall

# yf
rm $YF_DIR/*.h
rmdir --ignore-fail-on-non-empty $YF_DIR
