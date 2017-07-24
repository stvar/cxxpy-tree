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

#ifndef __EXT_PRINTER_IMPL_HPP
#define __EXT_PRINTER_IMPL_HPP

#include "tree/opts.hpp"
#include "tree/ext-err-impl.hpp"

static inline Sys::ext_func_result_t ext_print_func_entry(
    const AST::Node* obj, const Obj::options_t* opt)
{
    if (opt->action == Obj::options_t::ext_func_action) {
        using namespace ExtErr;
        print_error<ext_func_not_supported>();
        return 1;
    }
    if (obj == nullptr) {
        using namespace ExtErr;
        print_error<no_input_given>();
        return 1;
    }
    if (!opt->help_ext_func)
        AST::Printer(std::cout).print(obj);
    return 0;
}

#endif /* __EXT_PRINTER_IMPL_HPP */


