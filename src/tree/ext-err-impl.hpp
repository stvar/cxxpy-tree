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

#ifndef __EXT_ERR_IMPL_HPP
#define __EXT_ERR_IMPL_HPP

namespace ExtErr {

enum {
    ext_func_not_supported,
    no_input_given,
};

static inline void print_error(const char* err, ...)
    PRINTF_FMT(1);

static inline void print_error(const char* err, ...)
{
    va_list args;

    va_start(args, err);
    auto r = Sys::svformat(err, args);
    va_end(args);

    std::cerr
        << program << ": "
        << extension << ": error: "
        << r << std::endl;
}

template<size_t E>
void print_error();

template<>
inline void print_error<ext_func_not_supported>()
{ print_error("extension functions are not supported"); }

template<>
inline void print_error<no_input_given>()
{ print_error("no input given"); }

} // namespace ExtErr

#endif /* __EXT_ERR_IMPL_HPP */


