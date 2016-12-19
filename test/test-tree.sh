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
# $ gen-test -T tree
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L tree.old <(echo \
'$ cd ../src/tree
$ export LD_LIBRARY_PATH=.:abc:cxxpy
$ tree() { ./tree "$@" -- -i; }
$ tree -p "A('\''x'\'')"
Tree(
.   .root = TreeNode(
.   .   .name = NodeName('\''A'\'')
.   .   .args = LiteralArgs(
.   .   .   .literal = Literal('\''x'\'')
.   .   )
.   )
)
$ tree -u "A('\''x'\'')"
Tree(
.   TreeNode(
.   .   NodeName('\''A'\'')
.   .   LiteralArgs(
.   .   .   Literal('\''x'\'')
.   .   )
.   )
)
$ tree -P "A('\''x'\'')"
A('\''x'\'')
$ tree -p '\''D(.l=list<B>[[0]=B()])'\''
Tree(
.   .root = TreeNode(
.   .   .name = NodeName('\''D'\'')
.   .   .args = ListArgs(
.   .   .   .list = list<NodeArg> [
.   .   .   .   [0] = NodeArg(
.   .   .   .   .   .name = ArgName('\''l'\'')
.   .   .   .   .   .value = ListValue(
.   .   .   .   .   .   .list = NodeList(
.   .   .   .   .   .   .   .base = NodeName('\''B'\'')
.   .   .   .   .   .   .   .elems = list<ListElem> [
.   .   .   .   .   .   .   .   [0] = ListElem(
.   .   .   .   .   .   .   .   .   .index = '\''0'\''
.   .   .   .   .   .   .   .   .   .node = TreeNode(
.   .   .   .   .   .   .   .   .   .   .name = NodeName('\''B'\'')
.   .   .   .   .   .   .   .   .   .   .args = ListArgs(
.   .   .   .   .   .   .   .   .   .   .   .list = list<NodeArg> []
.   .   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   ]
.   .   .   .   .   .   )
.   .   .   .   .   )
.   .   .   .   )
.   .   .   ]
.   .   )
.   )
)
$ tree -u '\''D(.l=list<B>[[0]=B()])'\''
Tree(
.   TreeNode(
.   .   NodeName('\''D'\'')
.   .   ListArgs(
.   .   .   list<NodeArg> [
.   .   .   .   NodeArg(
.   .   .   .   .   ArgName('\''l'\'')
.   .   .   .   .   ListValue(
.   .   .   .   .   .   NodeList(
.   .   .   .   .   .   .   NodeName('\''B'\'')
.   .   .   .   .   .   .   list<ListElem> [
.   .   .   .   .   .   .   .   ListElem(
.   .   .   .   .   .   .   .   .   '\''0'\''
.   .   .   .   .   .   .   .   .   TreeNode(
.   .   .   .   .   .   .   .   .   .   NodeName('\''B'\'')
.   .   .   .   .   .   .   .   .   .   ListArgs(
.   .   .   .   .   .   .   .   .   .   .   list<NodeArg> []
.   .   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   ]
.   .   .   .   .   .   )
.   .   .   .   .   )
.   .   .   .   )
.   .   .   ]
.   .   )
.   )
)
$ tree -P '\''D(.l=list<B>[[0]=B()])'\''
D(
.   .l = list<B> [
.   .   [0] = B()
.   ]
)
$ tree -T '\''D(.l=list<B>[[0]=B()])'\''
D(.l=list<B>[[0]=B()])'
) -L tree.new <(
echo '$ cd ../src/tree'
cd ../src/tree 2>&1 ||
echo 'command failed: cd ../src/tree'

echo '$ export LD_LIBRARY_PATH=.:abc:cxxpy'
export LD_LIBRARY_PATH=.:abc:cxxpy 2>&1 ||
echo 'command failed: export LD_LIBRARY_PATH=.:abc:cxxpy'

echo '$ tree() { ./tree "$@" -- -i; }'
tree() { ./tree "$@" -- -i; } 2>&1 ||
echo 'command failed: tree() { ./tree "$@" -- -i; }'

echo '$ tree -p "A('\''x'\'')"'
tree -p "A('x')" 2>&1 ||
echo 'command failed: tree -p "A('\''x'\'')"'

echo '$ tree -u "A('\''x'\'')"'
tree -u "A('x')" 2>&1 ||
echo 'command failed: tree -u "A('\''x'\'')"'

echo '$ tree -P "A('\''x'\'')"'
tree -P "A('x')" 2>&1 ||
echo 'command failed: tree -P "A('\''x'\'')"'

echo '$ tree -p '\''D(.l=list<B>[[0]=B()])'\'''
tree -p 'D(.l=list<B>[[0]=B()])' 2>&1 ||
echo 'command failed: tree -p '\''D(.l=list<B>[[0]=B()])'\'''

echo '$ tree -u '\''D(.l=list<B>[[0]=B()])'\'''
tree -u 'D(.l=list<B>[[0]=B()])' 2>&1 ||
echo 'command failed: tree -u '\''D(.l=list<B>[[0]=B()])'\'''

echo '$ tree -P '\''D(.l=list<B>[[0]=B()])'\'''
tree -P 'D(.l=list<B>[[0]=B()])' 2>&1 ||
echo 'command failed: tree -P '\''D(.l=list<B>[[0]=B()])'\'''

echo '$ tree -T '\''D(.l=list<B>[[0]=B()])'\'''
tree -T 'D(.l=list<B>[[0]=B()])' 2>&1 ||
echo 'command failed: tree -T '\''D(.l=list<B>[[0]=B()])'\'''
)

