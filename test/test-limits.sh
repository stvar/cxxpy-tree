#!/bin/bash

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

#
# File generated by a command like:
# $ gen-test -T limits
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L limits.old <(echo \
'$ cd ../src/tree
$ export LD_LIBRARY_PATH=.:abc:cxxpy
$ tree() { ./tree "$@"; }
$ tree -t
tree: tree-printer: error: no input given
command failed: tree -t
$ tree -t '\'''\''
tree: parse error: <text>:1:1: expected token IDENT but got EOS
command failed: tree -t '\'''\''
$ tree -f
tree: tree-printer: error: no input given
command failed: tree -f
$ tree -f foo
tree: error: foo: cannot open file: No such file or directory
command failed: tree -f foo
$ tree -f /dev/zero
tree: error: /dev/zero: file is not regular
command failed: tree -f /dev/zero
$ echo|tree -f -
tree: parse error: <stdin>:1:1: expected token IDENT but got EOS
command failed: echo|tree -f -
$ echo -n|tree -f -
tree: parse error: <stdin>:1:1: expected token IDENT but got EOS
command failed: echo -n|tree -f -'
) -L limits.new <(
echo '$ cd ../src/tree'
cd ../src/tree 2>&1 ||
echo 'command failed: cd ../src/tree'

echo '$ export LD_LIBRARY_PATH=.:abc:cxxpy'
export LD_LIBRARY_PATH=.:abc:cxxpy 2>&1 ||
echo 'command failed: export LD_LIBRARY_PATH=.:abc:cxxpy'

echo '$ tree() { ./tree "$@"; }'
tree() { ./tree "$@"; } 2>&1 ||
echo 'command failed: tree() { ./tree "$@"; }'

echo '$ tree -t'
tree -t 2>&1 ||
echo 'command failed: tree -t'

echo '$ tree -t '\'''\'''
tree -t '' 2>&1 ||
echo 'command failed: tree -t '\'''\'''

echo '$ tree -f'
tree -f 2>&1 ||
echo 'command failed: tree -f'

echo '$ tree -f foo'
tree -f foo 2>&1 ||
echo 'command failed: tree -f foo'

echo '$ tree -f /dev/zero'
tree -f /dev/zero 2>&1 ||
echo 'command failed: tree -f /dev/zero'

echo '$ echo|tree -f -'
echo|tree -f - 2>&1 ||
echo 'command failed: echo|tree -f -'

echo '$ echo -n|tree -f -'
echo -n|tree -f - 2>&1 ||
echo 'command failed: echo -n|tree -f -'
)
