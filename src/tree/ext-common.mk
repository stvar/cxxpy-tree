# Copyright (C) 2016  Stefan Vargyas
# 
# This file is part of C++Py-Tree.
# 
# C++Py-Tree is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# C++Py-Tree is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with C++Py-Tree.  If not, see <http://www.gnu.org/licenses/>.

CFG       := ../../..
CXXPY_INC := ../../../include
GRAM_DIR  := ../../grammar
INC       := . ../.. ${CXXPY_INC} ${GRAM_DIR}

GRAM_SRCS := ast-base.cpp
GRAM_OBJS := $(patsubst %.cpp,%.o, ${GRAM_SRCS})
AST_SRCS  := ast.cpp
AST_OBJS  := $(patsubst %.cpp,%.o, ${AST_SRCS})
EXT_OBJS  := $(patsubst %.cpp,%.o, ${EXT_SRCS})
MOD_SRCS  := printer.cpp
MOD_OBJS  := $(patsubst %.cpp,%.o, ${MOD_SRCS})

SRCS := $(addprefix ${GRAM_DIR}/,${GRAM_SRCS}) ${AST_SRCS} ${EXT_SRCS} ${MOD_SRCS}
OBJS := ${GRAM_OBJS} ${AST_OBJS} ${EXT_OBJS} ${MOD_OBJS}

TREE := tree.so eval.so
MODS := $(patsubst %.o,%.so, ${MOD_OBJS})
BIN  := $(patsubst %.cpp,%.so, ${EXT_SRCS}) 
BINS := $(TREE) $(BIN) $(MODS)

LD_LIBRARY_PATH := ..
LDFLAGS := -Wl,-L$(LD_LIBRARY_PATH) -Wl,--rpath-link=$(LD_LIBRARY_PATH)
LIBS := $(TREE) -lstdc++ -ldl

include ${CFG}/common.mk

# dependency rules

$(GRAM_OBJS) $(AST_OBJS): ${GRAM_DIR}/ast-base.hpp ${GRAM_DIR}/ast-base-impl.hpp ast.hpp

$(AST_OBJS): %.o: %.hpp

$(BIN): ast-base.o ${AST_OBJS} $(TREE)

$(MODS): $(BIN)

# building rules

$(GRAM_OBJS): %.o: ${GRAM_DIR}/%.cpp
	${GCC} ${CFLAGS} -c $< -o $@

$(TREE): %.so:
	test -L $@ || ln -s ../$@ $@

$(BIN): %.so: %.o
	${GCC} ${SOFLAGS} ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}

$(MODS): %.so: %.o
	${GCC} ${SOFLAGS} $^ -o $@

# main targets

modules: $(MODS)

