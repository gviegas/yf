#!/usr/bin/env sh

#
# Builds YF for development.
#

CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# ws
make -C $WS_DIR -f Devel.mk

# cg
make -C $CG_DIR -f Devel.mk

# sg
make -C $SG_DIR -f Devel.mk
