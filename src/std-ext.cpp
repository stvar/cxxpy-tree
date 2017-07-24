// Copyright (C) 2016, 2017  Stefan Vargyas
// 
// This file is part of C++Py-Tree.
// 
// C++Py-Tree is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// C++Py-Tree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with C++Py-Tree.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"

#include <cctype>
#include <cstdlib>
#include <cerrno>

#include "sys.hpp"
#include "std-ext.hpp"

namespace Ext {

template<typename T>
static inline typename std::enable_if<
    is_unsigned<T>(),
    bool>::type
num_head(const char* str)
{ return isdigit(*str); }

template<typename T>
static inline typename std::enable_if<
    is_signed<T>(),
    bool>::type
num_head(const char* str)
{ return isdigit(*str) || *str == '-' || *str == '+'; }

template<typename T>
using cstdlib_func_t = T (*)(const char*, char**, int);

template<typename T, cstdlib_func_t<T> F>
static inline bool parse_num_cstdlib(
    const char* str, T& res, const char*& end)
{
    if (!num_head<T>(str)) {
        errno = EINVAL;
        return false;
    }
    end = nullptr;
    errno = 0;
    T val = F(
        str,
        const_cast<char**>(&end),
        10);
    if (errno)
        return false;
    if (end <= str) {
        errno = EINVAL;
        return false;
    }
    res = val;
    return true;
}

bool parse_num(
    const char* str, widest_sint_t& res, const char*& end)
{
    CXX_ASSERT((
        is_cv_same<widest_sint_t, long long>()));
    return
        parse_num_cstdlib<widest_sint_t, strtoll>(
            str, res, end);
}

// stev: according to the reference documentation the library
// function 'strtoull' does have a peculiar behavior regading
// inputs representing negative values:
//
//   Negative values are considered valid input and are
//   silently converted to the equivalent 'unsigned long
//   long' value.
//
// This latter behavior seems to be standard conformant: see
// ISO/IEC 9899:1999(TC3) 7.20.1.4 pt. 5 and ISO/IEC 9899:2011
// 7.22.1.4 pt. 5

bool parse_num(
    const char* str, widest_uint_t& res, const char*& end)
{
    CXX_ASSERT((
        is_cv_same<widest_uint_t, unsigned long long>()));
    return
        parse_num_cstdlib<widest_uint_t, strtoull>(
            str, res, end);
}

} // namespace Ext


