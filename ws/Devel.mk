#!/usr/bin/env -S make -f

SHELL := /bin/sh

.SUFFIXES: .cxx .o .d

VAR_DIR := ~/var/
INCLUDE_DIR := include/
SRC_DIR := src/
SUB_DIR := sub/
TEST_DIR := test/
BIN_DIR := $(VAR_DIR)bin/
CACHE_DIR := $(VAR_DIR)cache/yf/ws/

SRC := $(wildcard $(SRC_DIR)*.cxx) \
       $(wildcard $(SUB_DIR)*.cxx) \
       $(wildcard $(TEST_DIR)*.cxx)

OBJ := $(subst $(SRC_DIR),$(CACHE_DIR),$(SRC:.cxx=.o))
OBJ := $(subst $(SUB_DIR),$(CACHE_DIR),$(OBJ))
OBJ := $(subst $(TEST_DIR),$(CACHE_DIR),$(OBJ))

DEP := $(OBJ:.o=.d)

CC := /usr/bin/c++
CC_FLAGS := -std=gnu++17 -Wpedantic -Wall -Wextra -g

LD_LIBS :=
LD_FLAGS := -I $(VAR_DIR)include/ \
	    -iquote $(INCLUDE_DIR) \
	    -iquote $(SRC_DIR) \
	    -iquote $(SUB_DIR)

PP := $(CC) -E
PP_FLAGS := -D YF -D YF_WS -D YF_DEVEL

OUT := $(BIN_DIR)ws-devel

devel: $(OBJ)
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $^ $(LD_LIBS) -o $(OUT)

compile: $(OBJ)
	@echo Done.

.PHONY: clean-out
clean-out:
	rm -fv $(OUT)

.PHONY: clean-obj
clean-obj:
	rm -fv $(OBJ)

.PHONY: clean-dep
clean-dep:
	rm -fv $(DEP)

.PHONY: clean
clean: clean-out clean-obj clean-dep

$(CACHE_DIR)%.o: $(SRC_DIR)%.cxx
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(CACHE_DIR)%.o: $(SUB_DIR)%.cxx
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(CACHE_DIR)%.o: $(TEST_DIR)%.cxx
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $(PP_FLAGS) -c $< -o $@

$(CACHE_DIR)%.d: $(SRC_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(CACHE_DIR)%.d: $(SUB_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

$(CACHE_DIR)%.d: $(TEST_DIR)%.cxx
	@$(PP) $(LD_FLAGS) $(PP_FLAGS) $< -MM -MT $(@:.d=.o) > $@

-include $(DEP)
