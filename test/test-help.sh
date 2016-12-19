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
# $ gen-test -T help
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L help.old <(echo \
'$ cd ../src/tree
$ export LD_LIBRARY_PATH=.:abc:cxxpy
$ ./tree --help
usage: tree (action|option)* (input)*
where the actions are:
  -n|--none            no special action -- just parse input
  -p|--print           print the AST of the parsed input
  -u|--dump            dump the AST of the parsed input
  -P|--pretty-print    reprint prettily the parsed input (default)
  -T|--terse-print     reprint tersely the parsed input
  -F|--ext-function <module-name>[::<func-name>]
                       load given module and run specified func -- 
                         when function name was not given take it as
                         being the base name of the module name
the options are:
  -f|--file            input type: file
  -t|--text            input type: text (default)
  -d|--debug           print some debugging output
  -D|--no-debug        do not print debugging output (default)
     --debug-ext-func  pass '\''--debug'\'' to inner extension function
     --help-ext-func   pass '\''--help'\'' to inner extension function
     --dump-options    print options and exit
     --verbose         be verbose
  -v|--version         print version numbers and exit
  -?|--help            display this help info and exit
$ ./tree -F printer.so --help-ext
usage: tree (action|option)* (input)*
where the actions are:
  -p|--print-ast       print the given AST (default)
  -u|--dump-ast        dump the given AST
  -P|--pretty-ast      print prettily the given AST
  -T|--terse-ast       print tersely the given AST
  -F|--ext-function <module-name>[::<func-name>]
                       load given module and run specified func -- 
                         when function name was not given take it as
                         being the base name of the module name
the options are:
  -i|--indent-dots     print or not indenting dots (default not)
  -I|--no-indent-dots
  -d|--debug           print some debugging output
  -D|--no-debug        do not print debugging output (default)
     --debug-ext-func  pass '\''--debug'\'' to inner extension function
     --help-ext-func   pass '\''--help'\'' to inner extension function
     --dump-options    print options and exit
     --verbose         be verbose
  -v|--version         print version numbers and exit
  -?|--help            display this help info and exit
$ ./tree -F cxxpy/cxxpy.so --help-ext
usage: tree (action|option)* (input)*
where the actions are:
  -c|--type-check      type check given input (default)
  -A|--ast-build       build the AST object of given input
  -t|--print-types     output the AST type definitions of given module
  -o|--print-obj       build and print the AST object of given input
  -F|--ext-function <module-name>[::<func-name>]
                       load given module and run specified func -- 
                         when function name was not given take it as
                         being the base name of the module name
the options are:
  -r|--relaxed-casts   when evaluating AST objects make polymorphic type casts
  -s|--strict-casts    when evaluating AST objects make exact type casts (default)
  -T|--trace-eval      trace or not the tree evaluation algorithm (default not)
     --no-trace-eval
  -d|--debug           print some debugging output
  -D|--no-debug        do not print debugging output (default)
     --debug-ext-func  pass '\''--debug'\'' to inner extension function
     --help-ext-func   pass '\''--help'\'' to inner extension function
     --dump-options    print options and exit
     --verbose         be verbose
  -v|--version         print version numbers and exit
  -?|--help            display this help info and exit
$ ./tree -F abc/abc.so --help-ext
usage: tree (action|option)* (input)*
where the actions are:
  -c|--type-check      type check given input (default)
  -A|--ast-build       build the AST object of given input
  -t|--print-types     output the AST type definitions of given module
  -o|--print-obj       build and print the AST object of given input
  -F|--ext-function <module-name>[::<func-name>]
                       load given module and run specified func -- 
                         when function name was not given take it as
                         being the base name of the module name
the options are:
  -r|--relaxed-casts   when evaluating AST objects make polymorphic type casts
  -s|--strict-casts    when evaluating AST objects make exact type casts (default)
  -T|--trace-eval      trace or not the tree evaluation algorithm (default not)
     --no-trace-eval
  -d|--debug           print some debugging output
  -D|--no-debug        do not print debugging output (default)
     --debug-ext-func  pass '\''--debug'\'' to inner extension function
     --help-ext-func   pass '\''--help'\'' to inner extension function
     --dump-options    print options and exit
     --verbose         be verbose
  -v|--version         print version numbers and exit
  -?|--help            display this help info and exit'
) -L help.new <(
echo '$ cd ../src/tree'
cd ../src/tree 2>&1 ||
echo 'command failed: cd ../src/tree'

echo '$ export LD_LIBRARY_PATH=.:abc:cxxpy'
export LD_LIBRARY_PATH=.:abc:cxxpy 2>&1 ||
echo 'command failed: export LD_LIBRARY_PATH=.:abc:cxxpy'

echo '$ ./tree --help'
./tree --help 2>&1 ||
echo 'command failed: ./tree --help'

echo '$ ./tree -F printer.so --help-ext'
./tree -F printer.so --help-ext 2>&1 ||
echo 'command failed: ./tree -F printer.so --help-ext'

echo '$ ./tree -F cxxpy/cxxpy.so --help-ext'
./tree -F cxxpy/cxxpy.so --help-ext 2>&1 ||
echo 'command failed: ./tree -F cxxpy/cxxpy.so --help-ext'

echo '$ ./tree -F abc/abc.so --help-ext'
./tree -F abc/abc.so --help-ext 2>&1 ||
echo 'command failed: ./tree -F abc/abc.so --help-ext'
)

