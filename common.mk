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

.PHONY: default all clean allclean

default: all

ifndef CFG
CFG := ..
endif
ifndef INC
INC := ../include
endif

GCC := gcc
GCC_VERSION := 40800
GCC_STD := gnu++11

CFLAGS := -std=${GCC_STD} \
          -Wall -Wextra \
          -Wno-ignored-qualifiers \
          -I${CFG} $(addprefix -I,${INC}) \
          -fPIC -g

SOFLAGS := -shared

ifndef HDRS
HDRS := $(wildcard *.hpp)
endif
ifndef SRCS
SRCS := $(wildcard *.cpp)
endif
ifndef OBJS
OBJS := $(patsubst %.cpp, %.o, ${SRCS})
endif

#
# GCC version checking
#

GCC_VERSION_CHECK = $(shell \
    bash -c '. ${CFG}/gcc-version.sh && gcc-version-check ${GCC_VERSION} ${GCC}' 2>&1)

ifneq (${GCC_VERSION_CHECK},)
$(error ${GCC_VERSION_CHECK})
endif

#
# dependency rules
#

ifeq (.depend, $(wildcard .depend))
include .depend
endif

$(OBJS): ${CFG}/config.h

#
# building rules
#

depend::
	${GCC} ${CFLAGS} -c ${SRCS} -MM > .depend

%.o: %.cpp
	${GCC} ${CFLAGS} -c $< -o $@

#
# main targets
#

all:: $(BINS)

clean::
	rm -f *.o

allclean:: clean
	rm -f *~ .depend $(BINS)
