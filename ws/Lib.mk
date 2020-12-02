#!/usr/bin/env -S make -f

#
# WS
# Lib.mk
#
# Copyright © 2020 Gustavo C. Viegas.
#

SHELL := /bin/sh
.SUFFIXES: .cxx .o .d

BASE_DIR := ../inc/
INCLUDE_DIR := include/
SRC_DIR := src/
ETC_DIR := etc/
SUB_DIR := sub/
BUILD_DIR := build/
INSTALL_DIR := /usr/local/
INCLIB_DIR := $(INSTALL_DIR)include/yf/ws/

SO_LINK := libyf-ws.so
SO_NAME := $(SO_LINK).0
SO_FILE := $(SO_NAME).1.0

LIB_FILE := $(INSTALL_DIR)lib/$(SO_FILE)
LIB_NAME := $(INSTALL_DIR)lib/$(SO_NAME)
LIB_LINK := $(INSTALL_DIR)lib/$(SO_LINK)

SRC := \
	$(wildcard $(SRC_DIR)*.cxx) \
	$(wildcard $(ETC_DIR)*.cxx) \
	$(wildcard $(SUB_DIR)*.cxx)

OBJ := $(subst $(SRC_DIR),$(BUILD_DIR),$(SRC:.cxx=.o))
OBJ := $(subst $(ETC_DIR),$(BUILD_DIR),$(OBJ))
OBJ := $(subst $(SUB_DIR),$(BUILD_DIR),$(OBJ))

DEP := $(OBJ:.o=.d)

CXX := /usr/bin/c++
CXX_FLAGS := -std=gnu++17 -Wpedantic -Wall -Wextra -g #-O3

LD_LIBS := -ldl
LD_FLAGS := -iquote $(BASE_DIR) -iquote $(INCLUDE_DIR) -iquote $(SRC_DIR)

PP := $(CXX) -E
PP_FLAGS := -D YF_WS

all:

install: $(LIB_FILE)
	mkdir -p $(INCLIB_DIR)
	cp $(INCLUDE_DIR)*.h $(INCLIB_DIR)
	ln -sf $(LIB_FILE) $(LIB_LINK)
	ldconfig -n $(INSTALL_DIR)lib/

$(LIB_FILE): $(OBJ)
	$(CXX) -shared -Wl,-soname,$(SO_NAME) \
	$(CXX_FLAGS) $(LD_FLAGS) $^ $(LD_LIBS) -o $@

compile: $(OBJ)
	@echo Done.

-include $(DEP)

.PHONY: uninstall
uninstall:
	rm -f $(LIB_LINK) $(LIB_NAME) $(LIB_FILE) $(INCLIB_DIR)*.h
	rmdir --ignore-fail-on-non-empty $(INCLIB_DIR)

.PHONY: clean-obj
clean-obj:
	rm -f $(OBJ)

.PHONY: clean-dep
clean-dep:
	rm -f $(DEP)

.PHONY: clean
clean: clean-obj clean-dep

$(BUILD_DIR)%.o: $(SRC_DIR)%.cxx
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -fPIC -c $< -o $@

$(BUILD_DIR)%.o: $(ETC_DIR)%.cxx
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -fPIC -c $< -o $@

$(BUILD_DIR)%.o: $(SUB_DIR)%.cxx
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -fPIC -c $< -o $@

$(BUILD_DIR)%.d: $(SRC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(BUILD_DIR)%.d: $(ETC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(BUILD_DIR)%.d: $(SUB_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@
