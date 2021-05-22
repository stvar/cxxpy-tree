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
#include <libgen.h>

#include <iomanip>
#include <iostream>
#include <exception>
#include <algorithm>
#include <iterator>

#include "std-ext.hpp"
#include "options.hpp"

namespace Opts {

options_t::options_t() :
    home_dir       (""),
    action         (ext_func_action),
    ext_func_name  (nullptr),
#ifdef DEBUG
    debug          (false),
    debug_ext_func (false),
#endif
    help_ext_func  (false),
    verbose        (false),
    argc           (0),
    argv           (nullptr),
    ext_argc       (0),
    ext_argv       (nullptr)
{}

options_t::~options_t()
{}

void options_t::version()
{
    using namespace std;
    cout << program << ": version " << verdate << "\n\n" << license;
}

void options_t::usage_acts() const
{} // stev: nop

void options_t::usage_opts() const
{} // stev: nop

void options_t::usage() const
{
    using namespace std;
    cout
        << "usage: " << program << " (action|option)* (input)*" << endl
        << "where the actions are:" << endl;
    usage_acts();
    cout
        << "  -F|--ext-function <module-name>[::<func-name>]" << endl
        << "                       load given module and run specified func -- " << endl
        << "                         when function name was not given take it as" << endl
        << "                         being the base name of the module name" << endl
        << "the options are:" << endl;
    usage_opts();
    cout
#ifdef DEBUG
        << "  -d|--debug           print some debugging output" << endl
        << "  -D|--no-debug        do not print debugging output (default)" << endl
        << "     --debug-ext-func  pass '--debug' to inner extension function" << endl
#endif
        << "     --help-ext-func   pass '--help' to inner extension function" << endl
        << "     --dump-options    print options and exit" << endl
        << "     --verbose         be verbose" << endl
        << "  -v|--version         print version numbers and exit" << endl
        << "  -?|--help            display this help info and exit" << endl;
}

char const* options_t::noyes[2] = {
    "no",
    "yes"
};

void options_t::dump_opts() const
{} // stev: nop

void options_t::dump_args(size_t argc, char *const* argv, const char* name)
{
    using namespace std;
    size_t w = name ? strlen(name) + 1 : 0;
    if (name)
        cout << name << '-';
    SYS_ASSERT(w <= 16);
    cout << left << setw(16 - w) << "argc:" << argc << endl;
    size_t i = 0;
    char *const *p = argv;
    for (; i < argc; i ++, p ++) {
        if (name)
            cout << name << '-';
        size_t w2 = w + Sys::digits(i);
        SYS_ASSERT(w2 <= 11);
        cout << "argv[" << i << left << setw(11 - w2) << "]:";
        if (*p)
            cout << Ext::repr(*p);
        cout << endl;
    }
}

const char* options_t::act_name() const
{
    static char const* actions[] = {
        "ext-func", // action_t::ext_func_action
    };
    return Ext::array(actions)[action];
}

void options_t::dump() const
{
    using namespace std;
    cout
        << "home-dir:       " << home_dir << endl
        << "action:         " << act_name() << endl
        << "ext-func-name:  " << (ext_func_name ? ext_func_name : "-") << endl;
    dump_opts();
    cout
#ifdef DEBUG
        << "debug:          " << Ext::array(noyes)[debug] << endl
        << "debug-ext-func: " << Ext::array(noyes)[debug_ext_func] << endl
#endif
        << "help-ext-func:  " << Ext::array(noyes)[help_ext_func] << endl
        << "verbose:        " << Ext::array(noyes)[verbose] << endl;
    dump_args(argc, argv);
    dump_args(ext_argc, ext_argv, "ext");
}

const options_t* options_t::options(size_t argc, char* const argv[])
{
    static options_t opts;
    opts.parse(argc, argv);
    return &opts;
}

void options_t::collect_opts(
    std::vector<char>& res_short_opts,
    std::vector<struct option>& res_long_opts) const
{
    static const char short_opts[] =
        ":F:v"
#ifdef DEBUG
        "dD"
#endif
        ;
    static const auto n_short_opts = Ext::array_size(short_opts) - 1;

    static struct option long_opts[] = {
        { "ext-function",   1, nullptr, opt_type_t::ext_func },
#ifdef DEBUG
        { "debug",          0, nullptr, opt_type_t::debug },
        { "no-debug",       0, nullptr, opt_type_t::no_debug },
        { "debug-ext-func", 0, nullptr, opt_type_t::debug_ext_func },
#endif
        { "help-ext-func",  0, nullptr, opt_type_t::help_ext_func },
        { "dump-options",   0, nullptr, opt_type_t::dump_options },
        { "verbose",        0, nullptr, opt_type_t::verbose },
        { "version",        0, nullptr, opt_type_t::version },
        { "help",           0, &optopt, opt_type_t::help },
    };
    static const auto n_long_opts = Ext::array_size(long_opts);

    std::copy(short_opts, short_opts + n_short_opts, back_inserter(res_short_opts));
    std::copy(long_opts, long_opts + n_long_opts, back_inserter(res_long_opts));
}

void options_t::verror(const char* msg, va_list args)
{
    char buf[256];
    vsnprintf(buf, sizeof buf -1, msg, args);
    buf[255] = 0;

    throw Error(buf);
}

void options_t::error(const char* msg, ...)
{
    CXX_ASSERT(CONFIG_VA_END_NOOP);
    va_list args;
    va_start(args, msg);
    verror(msg, args);
    //!!!VA_END va_end(args);
}

void options_t::invalid_opt_arg(const char* opt_name, const char* opt_arg)
{
    error("invalid argument for '%s' option: '%s'", opt_name, opt_arg);
}

void options_t::missing_opt_arg(const char* opt_name)
{
    error("argument for option '%s' not found", opt_name);
}

void options_t::missing_opt_arg(char opt_name)
{
    error("argument for option '-%c' not found", opt_name);
}

void options_t::invalid_opt(const char* opt_name)
{
    error("invalid command line option '%s'", opt_name);
}

void options_t::invalid_opt(char opt_name)
{
    error("invalid command line option '-%c'", opt_name);
}

const char* options_t::get_ext_func_name() const
{ return nullptr; }

bool options_t::parse_opt(opt_t opt UNUSED, const char* opt_arg UNUSED)
{ return false; }

void options_t::parse(const options_t* parent)
{
    parse(parent->ext_argc, parent->ext_argv);
    home_dir = parent->home_dir;
#ifdef DEBUG
    debug |= parent->debug_ext_func;
#endif
    if (parent->help_ext_func) {
        usage();
        exit(0);
    }
}

void options_t::parse(size_t argc, char* const argv[])
{
    static const struct option null = {
        nullptr, 0, nullptr, 0
    };

    std::vector<char> short_opts;
    std::vector<struct option> long_opts;
    collect_opts(short_opts, long_opts);

    short_opts.push_back(0);
    long_opts.push_back(null);

    struct bits_opts {
        unsigned dump: 1;
        unsigned usage: 1;
        unsigned version: 1;
    };
    struct bits_opts bits = {
        .dump    = false,
        .usage   = false,
        .version = false,
    };

    home_dir = argc > 0 && argv[0] ? dirname(argv[0]) : nullptr;

    // stev: when given no options
    // (thus when 'argv == nullptr')
    // skip over the getopt_long loop
    if (argc == 0) goto parse_done;

    // stev: prevent the compiler erring
    // about the goto above crossing the
    // initializations of 'end' and 'ptr'
    // below by having an inner block
    {
        auto end = argv + argc;
        auto ptr = std::find_if(argv, end,
            [](const char* p) { return strcmp(p, "--") == 0; });
        if (ptr != end) {
            ext_argc = Ext::ptr_diff(end, ptr);
            ext_argv = ptr;
            argc -= ext_argc;
        }
    }

    opt_t opt;
    optind = 1;
    opterr = 0;
    while ((opt = getopt_long(
        argc, argv, &short_opts[0], &long_opts[0], nullptr)) != EOF) {
        switch (opt) {
        case opt_type_t::ext_func:
            action = ext_func_action;
            ext_func_name = optarg;
            break;
#ifdef DEBUG
        case opt_type_t::debug:
            debug = true;
            break;
        case opt_type_t::no_debug:
            debug = false;
            break;
        case opt_type_t::debug_ext_func:
            debug_ext_func = true;
            break;
#endif
        case opt_type_t::help_ext_func:
            help_ext_func = true;
            break;
        case opt_type_t::dump_options:
            bits.dump = 1;
            break;
        case opt_type_t::verbose:
            verbose = true;
            break;
        case opt_type_t::version:
            bits.version = 1;
            break;
        case ':': {
            const char* opt = argv[optind - 1];
            if (opt[0] == '-' && opt[1] == '-')
                missing_opt_arg(opt);
            else
                missing_opt_arg(optopt);
            break;
        }
        case '?':
        inv_opt:
            if (optopt == 0)
                invalid_opt(argv[optind - 1]);
            else
            if (optopt != '?')
                invalid_opt(optopt);
            else
                bits.usage = 1;
            break;
        case 0:
            bits.usage = 1;
            break;
        default:
            if (!parse_opt(opt, optarg))
                goto inv_opt;
            break;
        }
    }

    argv += optind;
    argc -= optind;

parse_done:
    this->argc = argc;
    this->argv = argv;

    if (action != ext_func_action)
        ext_func_name = get_ext_func_name();

    if (ext_argc > 0)
        const_cast<const char**>(ext_argv)[0] = ext_func_name;

    if (bits.version)
        version();
    if (bits.dump)
        dump();
    if (bits.usage)
        usage();

    if (bits.dump ||
        bits.version ||
        bits.usage)
        exit(0);
}

input_options_t::input_options_t() :
    input_type(text_input)
{
    action = none_action;
}

const input_options_t* input_options_t::options(size_t argc, char* const argv[])
{
    static input_options_t opts;
    opts.parse(argc, argv);
    return &opts;
}

void input_options_t::collect_opts(
    std::vector<char>& res_short_opts,
    std::vector<struct option>& res_long_opts) const
{
    static const char short_opts[] = "fnptu";
    static const auto n_short_opts = Ext::array_size(short_opts) - 1;

    static struct option long_opts[] = {
        { "none",  0, nullptr, opt_type_t::none },
        { "print", 0, nullptr, opt_type_t::print },
        { "dump",  0, nullptr, opt_type_t::dump },
        { "text",  0, nullptr, opt_type_t::text },
        { "file",  0, nullptr, opt_type_t::file },
    };
    static const auto n_long_opts = Ext::array_size(long_opts);

    base_t::collect_opts(res_short_opts, res_long_opts);
    std::copy(short_opts, short_opts + n_short_opts, back_inserter(res_short_opts));
    std::copy(long_opts, long_opts + n_long_opts, back_inserter(res_long_opts));
}

void input_options_t::usage_acts() const
{
    base_t::usage_acts();
    using namespace std;
    cout
        << "  -n|--none            no special action -- just process input" << endl
        << "  -p|--print           print the process input" << endl
        << "  -u|--dump            dump the process input" << endl;
}

void input_options_t::usage_opts() const
{
    base_t::usage_opts();
    using namespace std;
    cout
        << "  -f|--file            input type: file" << endl
        << "  -t|--text            input type: text (default)" << endl;
}

void input_options_t::dump_opts() const
{
    static char const* input_types[] = {
        "text",     // input_type_t::text_input
        "file"      // input_type_t::file_input
    };
    base_t::dump_opts();
    using namespace std;
    cout
        << "input-type:     " << Ext::array(input_types)[input_type] << endl;
}

const char* input_options_t::act_name() const
{
    static char const* actions[] = {
        "none",     // action_t::none_action
        "print",    // action_t::print_action
        "dump",     // action_t::dump_action
    };
    return action >= none_action
        ? Ext::array(actions)[action - none_action]
        : base_t::act_name();
}

const char* input_options_t::get_ext_func_name() const
{
    static const char* ext_funcs[] = {
        nullptr,             // action_t::none_action
        "printer::print",    // action_t::print_action
        "printer::dump",     // action_t::dump_action
    };
    return action >= none_action
        ? Ext::array(ext_funcs)[action - none_action]
        : base_t::get_ext_func_name();
}

bool input_options_t::parse_opt(opt_t opt, const char* opt_arg)
{
    if (base_t::parse_opt(opt, opt_arg))
        return true;
    switch (opt) {
    case opt_type_t::none:
        action = none_action;
        break;
    case opt_type_t::print:
        action = print_action;
        break;
    case opt_type_t::dump:
        action = dump_action;
        break;
    case opt_type_t::text:
        input_type = text_input;
        break;
    case opt_type_t::file:
        input_type = file_input;
        break;
    default:
        return false;
    }
    return true;
}

} // namespace Opts

