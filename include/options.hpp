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

#ifndef __OPTIONS_HPP
#define __OPTIONS_HPP

#include "config.h"

#include <cstdarg>
#include <unistd.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>

#include <vector>
#include <stdexcept>

#include "sys.hpp"

extern const char program[];
extern const char verdate[];
extern const char license[];

namespace Opts {

struct Error : public std::runtime_error
{
    Error(const char* what) : std::runtime_error(what) {}
};

void error(const char* msg, ...) PRINTF_FMT(1);
void verror(const char* msg, va_list arg);

class options_t
{
public:
    options_t(const options_t&) = delete;
    options_t& operator=(const options_t&) = delete;

    options_t(options_t&&) = delete;
    options_t& operator=(options_t&&) = delete;

    static const options_t* options(size_t argc, char* const argv[]);

    typedef size_t action_t;
    enum {
        ext_func_action
    };

    const char  *home_dir;
    action_t     action;
    const char  *ext_func_name;
#ifdef DEBUG
    bool         debug;
    bool         debug_ext_func;
#endif
    bool         help_ext_func;
    bool         verbose;
    size_t       argc;
    char *const *argv;
    size_t       ext_argc;
    char *const *ext_argv;

protected:
    options_t();
    virtual ~options_t();

    typedef int opt_t;
    struct opt_type_t
    {
        enum {
            ext_func       = 'F',
#ifdef DEBUG
            debug          = 'd',
            no_debug       = 'D',
            debug_ext_func = 256,
#endif
            help_ext_func  = 257,
            verbose        = 258,
            help           = '?',
            version        = 'v',
            dump_options   = 259
        };
    };

    virtual void collect_opts(
        std::vector<char>& short_opts,
        std::vector<struct option>& long_opts) const;
    virtual const char* get_ext_func_name() const;
    virtual bool parse_opt(opt_t opt, const char* opt_arg);
    void parse(const options_t* parent);
    void parse(size_t argc, char* const argv[]);

    static void verror(const char* msg, va_list args);
    static void error(const char* msg, ...) PRINTF_FMT(1);
    static void invalid_opt_arg(const char* opt_name, const char* opt_arg);
    static void missing_opt_arg(const char* opt_name);
    static void missing_opt_arg(char opt_name);
    static void invalid_opt(const char* opt_name);
    static void invalid_opt(char opt_name);

    static void version();

    virtual void usage_acts() const;
    virtual void usage_opts() const;
    void usage() const;

    static char const* noyes[2];

    static void dump_args(
        size_t argc, char *const* argv, const char* name = nullptr);

    virtual void dump_opts() const;
    virtual const char* act_name() const;
    void dump() const;
};

class input_options_t : public options_t
{
public:
    static const input_options_t* options(size_t argc, char* const argv[]);

    typedef options_t base_t;
    enum {
        none_action = base_t::ext_func_action + 1,
        print_action,
        dump_action,
    };

    enum input_type_t {
        text_input,
        file_input,
    };

    input_type_t input_type;

protected:
    input_options_t();

    struct opt_type_t : base_t::opt_type_t
    {
        enum {
            none  = 'n',
            print = 'p',
            dump  = 'u',
            text  = 't',
            file  = 'f',
        };
    };

    void collect_opts(
        std::vector<char>& short_opts,
        std::vector<struct option>& long_opts) const;
    const char* get_ext_func_name() const;
    bool parse_opt(int opt, const char* opt_arg);

    void usage_acts() const;
    void usage_opts() const;

    void dump_opts() const;
    const char* act_name() const;
};

} // namespace Opts

#endif /* __OPTIONS_HPP */

