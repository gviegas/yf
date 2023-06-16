#!/usr/bin/env -S make -f

SHELL := /bin/sh

.SUFFIXES: .cxx .o .d

VAR_DIR := ~/var/
INCLUDE_DIR := include/
SRC_DIR := src/
SUB_DIR := sub/
CACHE_DIR := $(VAR_DIR)cache/yf/ws/
LIB_DIR := $(VAR_DIR)
LIB_BIN_DIR := $(LIB_DIR)lib/
LIB_INC_DIR := $(LIB_DIR)include/yf/ws/

LIB_A := $(LIB_BIN_DIR)libyf-ws.a

SRC := $(wildcard $(SRC_DIR)*.cxx) $(wildcard $(SUB_DIR)*.cxx)

OBJ := $(subst $(SRC_DIR),$(CACHE_DIR),$(SRC:.cxx=.o))
OBJ := $(subst $(SUB_DIR),$(CACHE_DIR),$(OBJ))

DEP := $(OBJ:.o=.d)

CC := /usr/bin/c++
CC_FLAGS := -std=gnu++17 -Wpedantic -Wall -Wextra -O3

LD_LIBS := -ldl
LD_FLAGS := -I $(VAR_DIR)include/ \
	    -iquote $(INCLUDE_DIR) \
	    -iquote $(SRC_DIR) \
	    -iquote $(SUB_DIR)

PP := $(CC) -E
PP_FLAGS := -D YF -D YF_WS

lib: $(LIB_A)
	mkdir -pv $(LIB_INC_DIR)
	cp -v $(INCLUDE_DIR)*.h $(LIB_INC_DIR)

$(LIB_A): $(OBJ)
	ar rcs $@ $^

compile: $(OBJ)
	@echo Done.

.PHONY: clean-obj
clean-obj:
	rm -fv $(OBJ)

.PHONY: clean-dep
clean-dep:
	rm -fv $(DEP)

.PHONY: clean-lib
clean-lib:
	rm -fv $(LIB_A) $(LIB_INC_DIR)*.h
	rmdir -v --ignore-fail-on-non-empty $(LIB_INC_DIR)

.PHONY: clean
clean: clean-obj clean-dep clean-lib

$(CACHE_DIR)%.o: $(SRC_DIR)%.cxx
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(CACHE_DIR)%.o: $(SUB_DIR)%.cxx
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(CACHE_DIR)%.d: $(SRC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(CACHE_DIR)%.d: $(SUB_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

-include $(DEP)
