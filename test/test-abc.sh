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
# $ gen-test -T abc
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L abc.old <(echo \
'$ cd ../src/tree
$ export LD_LIBRARY_PATH=.:abc:cxxpy
$ tree() { ./tree -F abc/abc.so "$@"; }
$ tree -- -t
<Node>;
<A> : Node;
<B> : Node @ <A>* [a];
<C> : Node @ list<B> [b];
<D> : Node @ <char*> [c];
<N> : Node @ <B>* [x], <C>* [y], <D>* [z];
<M> : Node @ <A>* [a], list<B> [b], <char*> [c];
<P> : Node @ <A>* [x], <A>* [y];
<Q> : A;
<R> : Q;
<L> : Node @ list<Q> [q];
<S> : Q @ <bool> [b];
$ tree "S('\''0'\'')" -- -o
S(.b='\''0'\'')
$ tree "S()"
tree: error: ast-type S: expected 1 args, but got 0
command failed: tree "S()"
$ tree "S(.a='\''0'\'')"
tree: error: ast-type S: arg #1: expected name '\''b'\'', but got '\''a'\''
command failed: tree "S(.a='\''0'\'')"
$ tree "S(.b=A())"
tree: error: ast-type S: arg #1: cannot cast from type '\''A'\'' to '\''bool'\''
command failed: tree "S(.b=A())"
$ tree "P(.x=A().y=Q())"
tree: error: ast-type P: arg #2: cannot cast from type '\''Q'\'' to '\''A'\''
command failed: tree "P(.x=A().y=Q())"
$ tree "P(.x=A().y=Q())" -- -or
P(.x=A().y=Q())
$ tree "L(.q=list<R>[])" -- -os
tree: error: ast-type L: arg #1: cannot cast from type '\''list<R>'\'' to '\''list<Q>'\''
command failed: tree "L(.q=list<R>[])" -- -os
$ tree "L(.q=list<R>[[0]=R()])" -- -os
tree: error: ast-type L: arg #1: cannot cast from type '\''list<R>'\'' to '\''list<Q>'\''
command failed: tree "L(.q=list<R>[[0]=R()])" -- -os
$ tree "L(.q=list<R>[])" -- -or
L(.q=list<Q>[])
$ tree "L(.q=list<R>[[0]=R()])" -- -or
L(.q=list<Q>[[0]=R()])
$ tree "L(.q=list<R>[[0]=A()])" -- -os
tree: error: list<R>: arg #0: cannot cast from type '\''A'\''
command failed: tree "L(.q=list<R>[[0]=A()])" -- -os
$ tree "L(.q=list<R>[[0]=A()])" -- -or
tree: error: list<R>: arg #0: cannot cast from type '\''A'\''
command failed: tree "L(.q=list<R>[[0]=A()])" -- -or'
) -L abc.new <(
echo '$ cd ../src/tree'
cd ../src/tree 2>&1 ||
echo 'command failed: cd ../src/tree'

echo '$ export LD_LIBRARY_PATH=.:abc:cxxpy'
export LD_LIBRARY_PATH=.:abc:cxxpy 2>&1 ||
echo 'command failed: export LD_LIBRARY_PATH=.:abc:cxxpy'

echo '$ tree() { ./tree -F abc/abc.so "$@"; }'
tree() { ./tree -F abc/abc.so "$@"; } 2>&1 ||
echo 'command failed: tree() { ./tree -F abc/abc.so "$@"; }'

echo '$ tree -- -t'
tree -- -t 2>&1 ||
echo 'command failed: tree -- -t'

echo '$ tree "S('\''0'\'')" -- -o'
tree "S('0')" -- -o 2>&1 ||
echo 'command failed: tree "S('\''0'\'')" -- -o'

echo '$ tree "S()"'
tree "S()" 2>&1 ||
echo 'command failed: tree "S()"'

echo '$ tree "S(.a='\''0'\'')"'
tree "S(.a='0')" 2>&1 ||
echo 'command failed: tree "S(.a='\''0'\'')"'

echo '$ tree "S(.b=A())"'
tree "S(.b=A())" 2>&1 ||
echo 'command failed: tree "S(.b=A())"'

echo '$ tree "P(.x=A().y=Q())"'
tree "P(.x=A().y=Q())" 2>&1 ||
echo 'command failed: tree "P(.x=A().y=Q())"'

echo '$ tree "P(.x=A().y=Q())" -- -or'
tree "P(.x=A().y=Q())" -- -or 2>&1 ||
echo 'command failed: tree "P(.x=A().y=Q())" -- -or'

echo '$ tree "L(.q=list<R>[])" -- -os'
tree "L(.q=list<R>[])" -- -os 2>&1 ||
echo 'command failed: tree "L(.q=list<R>[])" -- -os'

echo '$ tree "L(.q=list<R>[[0]=R()])" -- -os'
tree "L(.q=list<R>[[0]=R()])" -- -os 2>&1 ||
echo 'command failed: tree "L(.q=list<R>[[0]=R()])" -- -os'

echo '$ tree "L(.q=list<R>[])" -- -or'
tree "L(.q=list<R>[])" -- -or 2>&1 ||
echo 'command failed: tree "L(.q=list<R>[])" -- -or'

echo '$ tree "L(.q=list<R>[[0]=R()])" -- -or'
tree "L(.q=list<R>[[0]=R()])" -- -or 2>&1 ||
echo 'command failed: tree "L(.q=list<R>[[0]=R()])" -- -or'

echo '$ tree "L(.q=list<R>[[0]=A()])" -- -os'
tree "L(.q=list<R>[[0]=A()])" -- -os 2>&1 ||
echo 'command failed: tree "L(.q=list<R>[[0]=A()])" -- -os'

echo '$ tree "L(.q=list<R>[[0]=A()])" -- -or'
tree "L(.q=list<R>[[0]=A()])" -- -or 2>&1 ||
echo 'command failed: tree "L(.q=list<R>[[0]=A()])" -- -or'
)
