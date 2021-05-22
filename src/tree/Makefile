# Copyright (C) 2016, 2017, 2021  Stefan Vargyas
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

CFG       := ../..
CXXPY_INC := ../../include
GRAM_DIR  := ../grammar
INC       := . ${CXXPY_INC} ${GRAM_DIR}

SYS_SRCS  := sys.cpp
SYS_OBJS  := $(patsubst %.cpp,%.o, ${SYS_SRCS})
EXT_SRCS  := std-ext.cpp
EXT_OBJS  := $(patsubst %.cpp,%.o, ${EXT_SRCS})
OPTS_SRCS := options.cpp
OPTS_OBJS := $(patsubst %.cpp,%.o, ${OPTS_SRCS})
GRAM_SRCS := ast-base.cpp parser-base.cpp
GRAM_OBJS := $(patsubst %.cpp,%.o, ${GRAM_SRCS})
AST_SRCS  := ast.cpp
AST_OBJS  := $(patsubst %.cpp,%.o, ${AST_SRCS})
EVAL_SRCS := eval.cpp opts.cpp
EVAL_OBJS := $(patsubst %.cpp,%.o, ${EVAL_SRCS})
MOD_SRCS  := printer.cpp
MOD_OBJS  := $(patsubst %.cpp,%.o, ${MOD_SRCS})
BIN_SRCS  := parser.cpp tree.cpp
BIN_OBJS  := $(patsubst %.cpp,%.o, ${BIN_SRCS})

SRCS := $(addprefix ../,${SYS_SRCS}) $(addprefix ../,${EXT_SRCS}) $(addprefix ../,${OPTS_SRCS}) $(addprefix ${GRAM_DIR}/,${GRAM_SRCS}) ${AST_SRCS} ${EVAL_SRCS} ${MOD_SRCS} ${BIN_SRCS}
OBJS := ${SYS_OBJS} ${SYS_OBJS} ${OPTS_OBJS} ${GRAM_OBJS} ${AST_OBJS} ${EVAL_OBJS} ${MOD_OBJS}

TREE := tree.so
EVAL := eval.so
BIN  := tree
MODS := $(patsubst %.o,%.so, ${MOD_OBJS})
BINS := $(TREE) $(EVAL) $(BIN) $(MODS)
LIBS := -lstdc++ -ldl

include ${CFG}/common.mk

# dependency rules

$(GRAM_OBJS) $(AST_OBJS): ${GRAM_DIR}/ast-base.hpp ${GRAM_DIR}/ast-base-impl.hpp ast.hpp

$(AST_OBJS): %.o: %.hpp

$(EVAL_OBJS): %.o: %.hpp

$(SYS_OBJS): %.o: $(CXXPY_INC)/%.hpp

$(EXT_OBJS): %.o: $(CXXPY_INC)/%.hpp

$(OPTS_OBJS): %.o: $(CXXPY_INC)/%.hpp

$(MOD_OBJS): ${CXXPY_INC}/options.hpp ${GRAM_DIR}/ast-base.hpp ast.hpp tree.hpp

printer.o: ${GRAM_DIR}/ast-base-printer.hpp

$(TREE): $(OPTS_OBJS) ast-base.o

$(EVAL): $(TREE)

$(BIN): $(TREE) parser-base.o parser.o

$(MODS): $(TREE)

# building rules

$(SYS_OBJS): %.o: ../%.cpp
	${GCC} ${CFLAGS} -c $< -o $@

$(EXT_OBJS): %.o: ../%.cpp
	${GCC} ${CFLAGS} -c $< -o $@

$(OPTS_OBJS): %.o: ../%.cpp
	${GCC} ${CFLAGS} -c $< -o $@

$(GRAM_OBJS): %.o: ${GRAM_DIR}/%.cpp
	${GCC} ${CFLAGS} -c $< -o $@

$(TREE): ${SYS_OBJS} ${EXT_OBJS} ${AST_OBJS}
	${GCC} ${SOFLAGS} $^ -o $@

$(EVAL): ${EVAL_OBJS}
	${GCC} ${SOFLAGS} $^ -o $@

$(MODS): %.so: %.o
	${GCC} ${SOFLAGS} $^ -o $@

$(BIN): %: %.o
	${GCC} ${CFLAGS} $^ -o $@ ${LIBS}

# main targets

modules: $(EVAL) $(MODS)

