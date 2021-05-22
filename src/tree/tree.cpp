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

#include <cstdarg>
#include <cstring>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <exception>

#include "sys.hpp"
#include "std-ext.hpp"
#include "parser.hpp"
#include "tree.hpp"

#include "demangle.hpp"

const char program[] = "tree";
const char verdate[] = "0.2 -- 2014-11-16 16:15"; // $ date +'%F %R'

const char license[] =
"Copyright (C) 2016, 2017, 2021  Stefan Vargyas.\n"
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n";

namespace Tree {

const options_t* options_t::options(size_t argc, char* const argv[])
{
    static options_t opts;
    opts.parse(argc, argv);
    return &opts;
}

options_t::options_t()
{
    action = pretty_print_action;
}

void options_t::collect_opts(
    std::vector<char>& res_short_opts,
    std::vector<struct option>& res_long_opts) const
{
    static const char short_opts[] = "PT";
    static const auto n_short_opts = Ext::array_size(short_opts) - 1;

    static struct option long_opts[] = {
        { "pretty-print", 0, nullptr, opt_type_t::pretty_print },
        { "terse-print",  0, nullptr, opt_type_t::terse_print },
    };
    static const auto n_long_opts = Ext::array_size(long_opts);

    base_t::collect_opts(res_short_opts, res_long_opts);
    std::copy(short_opts, short_opts + n_short_opts, back_inserter(res_short_opts));
    std::copy(long_opts, long_opts + n_long_opts, back_inserter(res_long_opts));
}

void options_t::usage_acts() const
{
    base2_t::usage_acts();
    using namespace std;
    cout
        << "  -n|--none            no special action -- just parse input" << endl
        << "  -p|--print           print the AST of the parsed input" << endl
        << "  -u|--dump            dump the AST of the parsed input" << endl
        << "  -P|--pretty-print    reprint prettily the parsed input (default)" << endl
        << "  -T|--terse-print     reprint tersely the parsed input" << endl;
}

const char* options_t::act_name() const
{
    static char const* actions[] = {
        "pretty-print", // pretty_print_action
        "terse-print",  // terse_print_action
    };
    return action >= pretty_print_action
        ? Ext::array(actions)[action - pretty_print_action]
        : base_t::act_name();
}

const char* options_t::get_ext_func_name() const
{
    static const char* ext_funcs[] = {
        "printer::pretty_print", // action_t::pretty_print_action
        "printer::terse_print",  // action_t::terse_print_action
    };
    return action >= pretty_print_action
        ? Ext::array(ext_funcs)[action - pretty_print_action]
        : base_t::get_ext_func_name();
}

bool options_t::parse_opt(opt_t opt, const char* opt_arg)
{
    if (base_t::parse_opt(opt, opt_arg))
        return true;
    switch (opt) {
    case opt_type_t::pretty_print:
        action = pretty_print_action;
        break;
    case opt_type_t::terse_print:
        action = terse_print_action;
        break;
    default:
        return false;
    }
    return true;
}

static inline std::unique_ptr<TreeAST::AST> parse_input(
    const char* input, const options_t* opts)
{
    using Parser::Parser;

    static const Parser::input_type_t input_types[] = {
        Parser::text_input, // options_t::input_type_t::text_input
        Parser::file_input  // options_t::input_type_t::file_input
    };

    Parser parser(
#ifdef DEBUG
        opts->debug,
#endif
        opts->verbose,
        Ext::array(input_types)[opts->input_type],
        opts->input_type == options_t::text_input
        ? "<text>" : input,
        input);

    return parser.parse();
}

} // namespace Tree

static inline void print_error(
    std::ostream& ost, const Parser::ParserError& err)
{ 
    ost << err.msg(
            std::string(program)
                .append(": parse error"))
        << std::endl;
}

static inline void print_error(
    std::ostream& ost, const std::runtime_error& err)
{ 
    ost << program
        << ": error: "
        << err.what()
        << std::endl;
}

static inline void print_error(
    std::ostream& ost, const std::exception& exc)
{ 
    ost << program
        << ": exception: "
        << Sys::demangle(exc)
        << ": "
        << exc.what()
        << std::endl;
}

static inline void print_error(std::ostream& ost)
{ 
    ost << program
        << ": error: unknown exception"
        << std::endl;
}

int main(int argc, char* const argv[])
try
{
    using namespace Sys;
    using namespace Tree;

    auto opts = options_t::options(argc, argv);
    if (opts->help_ext_func || opts->argc == 0) {
        if (auto r = run_ext_func<TreeAST::AST>(nullptr, opts))
            return r;
    }
    else {
        auto ptr = opts->argv;
        auto end = ptr + opts->argc;
        for (; ptr < end; ptr ++) {
            auto ast = parse_input(*ptr, opts);
            if (auto r = run_ext_func(ast, opts))
                return r;
        }
    }

    return 0;
}
catch (const Parser::ParserError& err) {
    print_error(std::cerr, err);
    return 1;
}
catch (const std::runtime_error& err) {
    print_error(std::cerr, err);
    return 1;
}
catch (const std::exception& exc) {
    print_error(std::cerr, exc);
    return 1;
}
catch (...) {
    print_error(std::cerr);
    return 1;
}


