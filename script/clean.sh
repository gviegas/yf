#!/usr/bin/env sh

#
# Removes bin/build files.
#

CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# sg
make -C $SG_DIR -f Devel.mk clean

# cg
make -C $CG_DIR -f Devel.mk clean

# ws
make -C $WS_DIR -f Devel.mk clean
