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

gcc-version()
{
    local p="${1:-gcc}"

    set -o pipefail
    "$p" --version|
    sed -nr '
        1 {
            s/^gcc.*\s([0-9]+\.[0-9]+\.[0-9]+)\s.*$/\1/p
            q
        }'|
    awk -F. '{
        printf("%d%02d%02d\n", $1, $2, $3)
    }'
}

gcc-version-check()
{
    local v="$1"
    local p="${2:-gcc}"

    local V
    V="$(gcc-version "$p" 2>/dev/null)" && [ -n "$V" ] || {
        echo >&2 "error: cannot determine the version of '$p'"
        return 1
    }
    [ "$v" -gt "$V" ] && {
        echo >&2 "error: expected GCC version no less than $v: got $V"
        return 1
    }

    return 0
}


