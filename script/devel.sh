#!/usr/bin/env sh

#
# Builds YF tests.
#

(script/var.sh; script/link.sh)

CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# ws
make -C $WS_DIR -f Devel.mk -j 4

# cg
make -C $CG_DIR -f Devel.mk -j 4

# sg
make -C $SG_DIR -f Devel.mk -j 4
