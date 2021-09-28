#!/usr/bin/env -S make -f

#
# WS
# Devel.mk
#
# Copyright © 2020-2021 Gustavo C. Viegas.
#

SHELL := /bin/sh
.SUFFIXES: .cxx .o .d

BASE_DIR := ../inc/
INCLUDE_DIR := include/
SRC_DIR := src/
TEST_DIR := test/
ETC_DIR := etc/
SUB_DIR := sub/
BIN_DIR := bin/
BUILD_DIR := build/

SRC := \
	$(wildcard $(SRC_DIR)*.cxx) \
	$(wildcard $(TEST_DIR)*.cxx) \
	$(wildcard $(ETC_DIR)*.cxx) \
	$(wildcard $(SUB_DIR)*.cxx)

OBJ := $(subst $(SRC_DIR),$(BUILD_DIR),$(SRC:.cxx=.o))
OBJ := $(subst $(TEST_DIR),$(BUILD_DIR),$(OBJ))
OBJ := $(subst $(ETC_DIR),$(BUILD_DIR),$(OBJ))
OBJ := $(subst $(SUB_DIR),$(BUILD_DIR),$(OBJ))

DEP := $(OBJ:.o=.d)

CXX := /usr/bin/c++
CXX_FLAGS := -std=gnu++17 -Wpedantic -Wall -Wextra -g

LD_LIBS := -ldl
LD_FLAGS := -iquote $(BASE_DIR) -iquote $(INCLUDE_DIR) -iquote $(SRC_DIR)

PP := $(CXX) -E
PP_FLAGS := -D YF_WS -D YF_DEVEL

OUT := $(BIN_DIR)devel

devel: $(OBJ)
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $^ $(LD_LIBS) -o $(OUT)

compile: $(OBJ)
	@echo Done.

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

$(BUILD_DIR)%.o: $(SUB_DIR)%.cxx
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(BUILD_DIR)%.d: $(SRC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(BUILD_DIR)%.d: $(TEST_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(BUILD_DIR)%.d: $(ETC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(BUILD_DIR)%.d: $(SUB_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@
