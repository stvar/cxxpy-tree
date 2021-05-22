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

TARGETS = default test depend all clean allclean

.PHONY: $(TARGETS)

SRC := src/tree
ABC := src/tree/abc
CPY := src/tree/cxxpy

default: all

$(filter-out default test, $(TARGETS))::
	cd ${SRC} && $(MAKE) $@
	cd ${ABC} && $(MAKE) $@
	cd ${CPY} && $(MAKE) $@

test:
	cd test && ./test.sh

allclean::
	rm -f *~

