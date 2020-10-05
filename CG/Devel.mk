#!/usr/bin/env -S make -f

#
# yf
# Devel.mk - Development makefile (CG).
#
# Copyright (C) 2020 Gustavo C. Viegas.
#

SHELL := /bin/sh
.SUFFIXES: .cxx .o .d

INCBASE_DIR := ../include/
INCLUDE_DIR := include/
SRC_DIR := src/
TEST_DIR := test/
ETC_DIR := etc/
BIN_DIR := ../bin/
BUILD_DIR := ../build/

SRC := \
  $(wildcard $(SRC_DIR)*.cxx) \
  $(wildcard $(TEST_DIR)*.cxx) \
  $(wildcard $(ETC_DIR)*.cxx)

OBJ := $(subst $(SRC_DIR),$(BUILD_DIR),$(SRC:.cxx=.o))
OBJ := $(subst $(TEST_DIR),$(BUILD_DIR),$(OBJ))
OBJ := $(subst $(ETC_DIR),$(BUILD_DIR),$(OBJ))

DEP := $(OBJ:.o=.d)

CXX := /usr/bin/clang++
CXX_FLAGS := -std=gnu++17 -Wpedantic -Wall -Wextra -Og

LD_LIBS := # -ldl
LD_FLAGS := \
  -iquote $(INCBASE_DIR) \
  -iquote $(INCLUDE_DIR) \
  -iquote $(SRC_DIR)

PP := $(CXX) -E
PP_FLAGS := -D YF_CG -D YF_DEVEL

OUT := $(BIN_DIR)CG-devel

devel: $(OBJ)
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $^ $(LD_LIBS) -o $(OUT)

-include $(DEP)

.PHONY: clean-out
clean-out:
	rm -f $(OUT)

.PHONY: clean-obj
clean-obj:
	rm -f $(OBJ)

.PHONY: clean-dep
clean-dep:
	rm -f $(DEP)

.PHONY: clean
clean: clean-out clean-obj clean-dep

$(BUILD_DIR)%.o: $(SRC_DIR)%.cxx
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(BUILD_DIR)%.o: $(TEST_DIR)%.cxx
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(BUILD_DIR)%.o: $(ETC_DIR)%.cxx
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(BUILD_DIR)%.d: $(SRC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(BUILD_DIR)%.d: $(TEST_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(BUILD_DIR)%.d: $(ETC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@
