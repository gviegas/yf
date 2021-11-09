#!/usr/bin/env sh

#
# Makes required sym links.
#

CG_DIR=cg
WS_DIR=ws
SG_DIR=sg

# cg
ln -srv test/Test.cxx $CG_DIR/test/Test.cxx
ln -srv test/Test.h $CG_DIR/test/Test.h

# ws
ln -srv test/Test.cxx $WS_DIR/test/Test.cxx
ln -srv test/Test.h $WS_DIR/test/Test.h

# sg
ln -srv test/Test.cxx $SG_DIR/test/Test.cxx
ln -srv test/Test.h $SG_DIR/test/Test.h
