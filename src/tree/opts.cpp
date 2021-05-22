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

#include <iostream>

#include "sys.hpp"
#include "std-ext.hpp"
#include "opts.hpp"

namespace Obj {

options_t::options_t() :
    casts_type(strict_casts),
    trace_eval(false)
{
    action = type_check_action;
}

void options_t::parse(const Tree::options_t* parent)
{
    base_t::parse(parent->ext_argc, parent->ext_argv);
#ifdef DEBUG
    debug |= parent->debug_ext_func;
#endif
    if (parent->help_ext_func) {
        usage();
        exit(0);
    }
}

const options_t* options_t::options(const Tree::options_t* parent)
{
    static options_t opts;
    opts.parse(parent);
    return &opts;
}

void options_t::collect_opts(
    std::vector<char>& res_short_opts,
    std::vector<struct option>& res_long_opts) const
{
    static const char short_opts[] = "cAorstT";
    static const auto n_short_opts = Ext::array_size(short_opts) - 1;

    static struct option long_opts[] = {
        { "type-check",    0, nullptr, opt_type_t::type_check },
        { "ast-build",     0, nullptr, opt_type_t::ast_build },
        { "print-obj",     0, nullptr, opt_type_t::print_obj },
        { "print-types",   0, nullptr, opt_type_t::print_types },
        { "relaxed-casts", 0, nullptr, opt_type_t::relaxed_casts },
        { "strict-casts",  0, nullptr, opt_type_t::strict_casts },
        { "trace-eval",    0, nullptr, opt_type_t::trace_eval },
        { "no-trace-eval", 0, nullptr, opt_type_t::no_trace_eval },
    };
    static const auto n_long_opts = Ext::array_size(long_opts);

    base_t::collect_opts(res_short_opts, res_long_opts);
    std::copy(short_opts, short_opts + n_short_opts, back_inserter(res_short_opts));
    std::copy(long_opts, long_opts + n_long_opts, back_inserter(res_long_opts));
}

void options_t::usage_acts() const
{
    base_t::usage_acts();
    using namespace std;	
    cout
        << "  -c|--type-check      type check given input (default)" << endl
        << "  -A|--ast-build       build the AST object of given input" << endl
        << "  -t|--print-types     output the AST type definitions of given module" << endl
        << "  -o|--print-obj       build and print the AST object of given input" << endl;
}

void options_t::usage_opts() const
{
    base_t::usage_opts();
    using namespace std;
    cout
        << "  -r|--relaxed-casts   when evaluating AST objects make polymorphic type casts" << endl
        << "  -s|--strict-casts    when evaluating AST objects make exact type casts (default)" << endl
        << "  -T|--trace-eval      trace or not the tree evaluation algorithm (default not)" << endl
        << "     --no-trace-eval" << endl;
}

void options_t::dump_opts() const
{
    static char const* casts_types[] = {
        "relaxed-casts", // opt_type_t::relaxed_casts
        "strict-casts",  // opt_type_t::strict_casts
    };
    base_t::dump_opts();
    using namespace std;
    cout
        << "casts-type:     " << Ext::array(casts_types)[casts_type] << endl
        << "trace-eval:     " << Ext::array(noyes)[trace_eval] << endl;
}

const char* options_t::act_name() const
{
    static char const* actions[] = {
        "type-check",  // type_check_action
        "ast-build",   // ast_build_action
        "print-obj",   // print_obj_action
        "print-types", // print_types_action
    };
    return action >= type_check_action
        ? Ext::array(actions)[action - type_check_action]
        : base_t::act_name();
}

const char* options_t::get_ext_func_name() const
{
    static const char* ext_funcs[] = {
        "printer::print", // action_t::print_obj_action
        nullptr,          // action_t::print_types_action
    };
    return action >= print_obj_action
        ? Ext::array(ext_funcs)[action - print_obj_action]
        : base_t::get_ext_func_name();
}

bool options_t::parse_opt(opt_t opt, const char* opt_arg)
{
    if (base_t::parse_opt(opt, opt_arg))
        return true;
    switch (opt) {
    case opt_type_t::type_check:
        action = type_check_action;
        break;
    case opt_type_t::ast_build:
        action = ast_build_action;
        break;
    case opt_type_t::print_obj:
        action = print_obj_action;
        break;
    case opt_type_t::print_types:
        action = print_types_action;
        break;
    case opt_type_t::relaxed_casts:
        casts_type = relaxed_casts;
        break;
    case opt_type_t::strict_casts:
        casts_type = strict_casts;
        break;
    case opt_type_t::trace_eval:
        trace_eval = true;
        break;
    case opt_type_t::no_trace_eval:
        trace_eval = false;
        break;
    default:
        return false;
    }
    return true;
}

} // namespace Obj

