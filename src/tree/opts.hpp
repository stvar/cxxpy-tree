// Copyright (C) 2016, 2017, 2021  Stefan Vargyas
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

#ifndef __OPTS_HPP
#define __OPTS_HPP

#include "options.hpp"
#include "tree.hpp"

namespace Obj {

class options_t : public Opts::options_t
{
public:
    static const options_t* options(const Tree::options_t*);

    typedef Opts::options_t base_t;
    enum {
        type_check_action = base_t::ext_func_action + 1,
        ast_build_action,
        print_obj_action,
        print_types_action,
    };

    enum casts_type_t {
        relaxed_casts,
        strict_casts,
    };

    casts_type_t casts_type;
    bool trace_eval;

    const char* act_name() const;

protected:
    options_t();

    struct opt_type_t : base_t::opt_type_t
    {
        enum {
            type_check    = 'c',
            ast_build     = 'A',
            print_obj     = 'o',
            print_types   = 't',
            relaxed_casts = 'r',
            strict_casts  = 's',
            trace_eval    = 'T',
            no_trace_eval = 127,
        };
    };

    void collect_opts(
        std::vector<char>& short_opts,
        std::vector<struct option>& long_opts) const;
    const char* get_ext_func_name() const;
    void parse(const Tree::options_t* parent);
    bool parse_opt(opt_t opt, const char* opt_arg);

    using base_t::noyes;

    void usage_acts() const;
    void usage_opts() const;

    void dump_opts() const;
};

} // namespace Obj

#endif /* __OPTS_HPP */

