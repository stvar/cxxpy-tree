#!/bin/bash

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

#
# File generated by a command like:
# $ gen-test -T print
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L print.old <(echo \
'$ cd ../src/tree
$ export LD_LIBRARY_PATH=.:abc:cxxpy
$ tree() { ./tree "$@" -- -i; }
$ tree -p '\''A("\"")'\''
Tree(
.   .root = TreeNode(
.   .   .name = NodeName('\''A'\'')
.   .   .args = LiteralArgs(
.   .   .   .literal = Literal('\''\"'\'')
.   .   )
.   )
)
$ tree -T '\''A("\"")'\''
A('\''\"'\'')
$ tree -p "A('\''\'\'''\'')"
Tree(
.   .root = TreeNode(
.   .   .name = NodeName('\''A'\'')
.   .   .args = LiteralArgs(
.   .   .   .literal = Literal('\''\'\'''\'')
.   .   )
.   )
)
$ tree -T "A('\''\'\'''\'')"
A('\''\'\'''\'')
$ tree -p "A(.x=B().y='\''C'\''.z=D('\''E'\''))"
Tree(
.   .root = TreeNode(
.   .   .name = NodeName('\''A'\'')
.   .   .args = ListArgs(
.   .   .   .list = list<NodeArg> [
.   .   .   .   [0] = NodeArg(
.   .   .   .   .   .name = ArgName('\''x'\'')
.   .   .   .   .   .value = NodeValue(
.   .   .   .   .   .   .node = TreeNode(
.   .   .   .   .   .   .   .name = NodeName('\''B'\'')
.   .   .   .   .   .   .   .args = ListArgs(
.   .   .   .   .   .   .   .   .list = list<NodeArg> []
.   .   .   .   .   .   .   )
.   .   .   .   .   .   )
.   .   .   .   .   )
.   .   .   .   )
.   .   .   .   [1] = NodeArg(
.   .   .   .   .   .name = ArgName('\''y'\'')
.   .   .   .   .   .value = LiteralValue(
.   .   .   .   .   .   .literal = Literal('\''C'\'')
.   .   .   .   .   )
.   .   .   .   )
.   .   .   .   [2] = NodeArg(
.   .   .   .   .   .name = ArgName('\''z'\'')
.   .   .   .   .   .value = NodeValue(
.   .   .   .   .   .   .node = TreeNode(
.   .   .   .   .   .   .   .name = NodeName('\''D'\'')
.   .   .   .   .   .   .   .args = LiteralArgs(
.   .   .   .   .   .   .   .   .literal = Literal('\''E'\'')
.   .   .   .   .   .   .   )
.   .   .   .   .   .   )
.   .   .   .   .   )
.   .   .   .   )
.   .   .   ]
.   .   )
.   )
)
$ tree -u "A(.x=B().y='\''C'\''.z=D('\''E'\''))"
Tree(
.   TreeNode(
.   .   NodeName('\''A'\'')
.   .   ListArgs(
.   .   .   list<NodeArg> [
.   .   .   .   NodeArg(
.   .   .   .   .   ArgName('\''x'\'')
.   .   .   .   .   NodeValue(
.   .   .   .   .   .   TreeNode(
.   .   .   .   .   .   .   NodeName('\''B'\'')
.   .   .   .   .   .   .   ListArgs(
.   .   .   .   .   .   .   .   list<NodeArg> []
.   .   .   .   .   .   .   )
.   .   .   .   .   .   )
.   .   .   .   .   )
.   .   .   .   )
.   .   .   .   NodeArg(
.   .   .   .   .   ArgName('\''y'\'')
.   .   .   .   .   LiteralValue(
.   .   .   .   .   .   Literal('\''C'\'')
.   .   .   .   .   )
.   .   .   .   )
.   .   .   .   NodeArg(
.   .   .   .   .   ArgName('\''z'\'')
.   .   .   .   .   NodeValue(
.   .   .   .   .   .   TreeNode(
.   .   .   .   .   .   .   NodeName('\''D'\'')
.   .   .   .   .   .   .   LiteralArgs(
.   .   .   .   .   .   .   .   Literal('\''E'\'')
.   .   .   .   .   .   .   )
.   .   .   .   .   .   )
.   .   .   .   .   )
.   .   .   .   )
.   .   .   ]
.   .   )
.   )
)
$ tree -P "A(.x=B().y='\''C'\''.z=D('\''E'\''))"
A(
.   .x = B()
.   .y = '\''C'\''
.   .z = D('\''E'\'')
)
$ tree -p "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"
Tree(
.   .root = TreeNode(
.   .   .name = NodeName('\''A'\'')
.   .   .args = ListArgs(
.   .   .   .list = list<NodeArg> [
.   .   .   .   [0] = NodeArg(
.   .   .   .   .   .name = ArgName('\''l'\'')
.   .   .   .   .   .value = ListValue(
.   .   .   .   .   .   .list = NodeList(
.   .   .   .   .   .   .   .base = NodeName('\''B'\'')
.   .   .   .   .   .   .   .elems = list<ListElem> [
.   .   .   .   .   .   .   .   [0] = ListElem(
.   .   .   .   .   .   .   .   .   .index = '\''1'\''
.   .   .   .   .   .   .   .   .   .node = TreeNode(
.   .   .   .   .   .   .   .   .   .   .name = NodeName('\''C'\'')
.   .   .   .   .   .   .   .   .   .   .args = ListArgs(
.   .   .   .   .   .   .   .   .   .   .   .list = list<NodeArg> []
.   .   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   [1] = ListElem(
.   .   .   .   .   .   .   .   .   .index = '\''0'\''
.   .   .   .   .   .   .   .   .   .node = TreeNode(
.   .   .   .   .   .   .   .   .   .   .name = NodeName('\''D'\'')
.   .   .   .   .   .   .   .   .   .   .args = LiteralArgs(
.   .   .   .   .   .   .   .   .   .   .   .literal = Literal('\''x'\'')
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
$ tree -u "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"
Tree(
.   TreeNode(
.   .   NodeName('\''A'\'')
.   .   ListArgs(
.   .   .   list<NodeArg> [
.   .   .   .   NodeArg(
.   .   .   .   .   ArgName('\''l'\'')
.   .   .   .   .   ListValue(
.   .   .   .   .   .   NodeList(
.   .   .   .   .   .   .   NodeName('\''B'\'')
.   .   .   .   .   .   .   list<ListElem> [
.   .   .   .   .   .   .   .   ListElem(
.   .   .   .   .   .   .   .   .   '\''1'\''
.   .   .   .   .   .   .   .   .   TreeNode(
.   .   .   .   .   .   .   .   .   .   NodeName('\''C'\'')
.   .   .   .   .   .   .   .   .   .   ListArgs(
.   .   .   .   .   .   .   .   .   .   .   list<NodeArg> []
.   .   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   )
.   .   .   .   .   .   .   .   ListElem(
.   .   .   .   .   .   .   .   .   '\''0'\''
.   .   .   .   .   .   .   .   .   TreeNode(
.   .   .   .   .   .   .   .   .   .   NodeName('\''D'\'')
.   .   .   .   .   .   .   .   .   .   LiteralArgs(
.   .   .   .   .   .   .   .   .   .   .   Literal('\''x'\'')
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
$ tree -P "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"
A(
.   .l = list<B> [
.   .   [1] = C()
.   .   [0] = D('\''x'\'')
.   ]
)'
) -L print.new <(
echo '$ cd ../src/tree'
cd ../src/tree 2>&1 ||
echo 'command failed: cd ../src/tree'

echo '$ export LD_LIBRARY_PATH=.:abc:cxxpy'
export LD_LIBRARY_PATH=.:abc:cxxpy 2>&1 ||
echo 'command failed: export LD_LIBRARY_PATH=.:abc:cxxpy'

echo '$ tree() { ./tree "$@" -- -i; }'
tree() { ./tree "$@" -- -i; } 2>&1 ||
echo 'command failed: tree() { ./tree "$@" -- -i; }'

echo '$ tree -p '\''A("\"")'\'''
tree -p 'A("\"")' 2>&1 ||
echo 'command failed: tree -p '\''A("\"")'\'''

echo '$ tree -T '\''A("\"")'\'''
tree -T 'A("\"")' 2>&1 ||
echo 'command failed: tree -T '\''A("\"")'\'''

echo '$ tree -p "A('\''\'\'''\'')"'
tree -p "A('\'')" 2>&1 ||
echo 'command failed: tree -p "A('\''\'\'''\'')"'

echo '$ tree -T "A('\''\'\'''\'')"'
tree -T "A('\'')" 2>&1 ||
echo 'command failed: tree -T "A('\''\'\'''\'')"'

echo '$ tree -p "A(.x=B().y='\''C'\''.z=D('\''E'\''))"'
tree -p "A(.x=B().y='C'.z=D('E'))" 2>&1 ||
echo 'command failed: tree -p "A(.x=B().y='\''C'\''.z=D('\''E'\''))"'

echo '$ tree -u "A(.x=B().y='\''C'\''.z=D('\''E'\''))"'
tree -u "A(.x=B().y='C'.z=D('E'))" 2>&1 ||
echo 'command failed: tree -u "A(.x=B().y='\''C'\''.z=D('\''E'\''))"'

echo '$ tree -P "A(.x=B().y='\''C'\''.z=D('\''E'\''))"'
tree -P "A(.x=B().y='C'.z=D('E'))" 2>&1 ||
echo 'command failed: tree -P "A(.x=B().y='\''C'\''.z=D('\''E'\''))"'

echo '$ tree -p "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"'
tree -p "A(.l=list<B>[[1]=C()[0]=D('x')])" 2>&1 ||
echo 'command failed: tree -p "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"'

echo '$ tree -u "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"'
tree -u "A(.l=list<B>[[1]=C()[0]=D('x')])" 2>&1 ||
echo 'command failed: tree -u "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"'

echo '$ tree -P "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"'
tree -P "A(.l=list<B>[[1]=C()[0]=D('x')])" 2>&1 ||
echo 'command failed: tree -P "A(.l=list<B>[[1]=C()[0]=D('\''x'\'')])"'
)

