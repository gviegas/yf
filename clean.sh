#!/usr/bin/env sh

#
# Removes bin/build files.
#

# sg
make -C sg/ -f Devel.mk clean

# cg
make -C cg/ -f Devel.mk clean

# ws
make -C ws/ -f Devel.mk clean
