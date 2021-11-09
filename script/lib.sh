#!/usr/bin/env sh

#
# Builds YF libs.
#

(script/var.sh)

CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# ws
make -C $WS_DIR -f Lib.mk -j 4

# cg
make -C $CG_DIR -f Lib.mk -j 4

# sg
make -C $SG_DIR -f Lib.mk -j 4
