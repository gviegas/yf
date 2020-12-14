#!/usr/bin/env sh

#
# Makes required sym links.
#

CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# cg
ln -sr test/Test.cxx $CG_DIR/test/Test.cxx
ln -sr test/Test.h $CG_DIR/test/Test.h

# ws
ln -sr test/Test.cxx $WS_DIR/test/Test.cxx
ln -sr test/Test.h $WS_DIR/test/Test.h

# sg
ln -sr test/Test.cxx $SG_DIR/test/Test.cxx
ln -sr test/Test.h $SG_DIR/test/Test.h
