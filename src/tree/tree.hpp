// Copyright (C) 2016  Stefan Vargyas
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

#ifndef __TREE_HPP
#define __TREE_HPP

#include "options.hpp"

namespace Tree {

class options_t : public Opts::input_options_t
{
public:
    static const options_t* options(size_t argc, char* const argv[]);

    typedef Opts::input_options_t base_t;
    enum {
        pretty_print_action = base_t::dump_action + 1,
        terse_print_action
    };

    const char* act_name() const;

protected:
    options_t();

    typedef Opts::options_t base2_t;

    struct opt_type_t : base_t::opt_type_t
    {
        enum {
            pretty_print = 'P',
            terse_print  = 'T',
        };
    };

    void collect_opts(
        std::vector<char>& short_opts,
        std::vector<struct option>& long_opts) const;
    const char* get_ext_func_name() const;
    bool parse_opt(opt_t opt, const char* opt_arg);

    void usage_acts() const;
};

} // namespace Tree

#endif /* __TREE_HPP */

