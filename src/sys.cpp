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

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cerrno>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>

#include <iostream>

#include "sys.hpp"
#ifdef SYS_OPTS
#include "prop.hpp"
#endif // SYS_OPTS

#include "std-ext.hpp"

extern const char program[];

namespace Sys {

void buf_t::enlarge(size_t n)
{
    auto p = get();
    SYS_ASSERT(ptr >= p);
    size_t s = ptr - p;
    auto r = base_t::realloc(size + n);
    if (r == nullptr)
        OUT_OF_MEMORY();
    size += n;
    ptr = r + s;
}

const char* buf_t::nadd(const char *key, size_t sz)
{
    auto s = 1 + sz;
    auto d = Ext::ptr_diff(ptr, get());
    if (d + s > size) {
        enlarge(incr > s ? incr : s);
        d = Ext::ptr_diff(ptr, get());
        SYS_ASSERT(d + s <= size);
    }
    auto r = ptr;
    memcpy(r, key, sz);
    r[sz] = 0;
    SYS_ASSERT(strncmp(r, key, sz) == 0);
    ptr += s;
    return r;
}

#ifdef SYS_OPTS

struct options_t opts = {
#ifdef DEBUG
    .debug = false,
#endif
#ifdef COLORIZE
    .colorize = false,
#endif
};

} // namespace Sys

namespace Prop {

enum opt_t {
#ifdef DEBUG
    debug,
#endif
#ifdef COLORIZE
    colorize,
#endif
};

// #include <boost/preprocessor/slot/slot.hpp>
// 
// # define PROP_SIZE_OPTS 0
// 
// #ifdef DEBUG
// #define BOOST_PP_VALUE 1 + PROP_SIZE_OPTS
// #include BOOST_PP_ASSIGN_SLOT(1)
// #undef PROP_SIZE_OPTS
// #define PROP_SIZE_OPTS BOOST_PP_SLOT(1)
// #endif
// 
// #ifdef COLORIZE
// #define BOOST_PP_VALUE 1 + PROP_SIZE_OPTS
// #include BOOST_PP_ASSIGN_SLOT(1)
// #undef PROP_SIZE_OPTS
// #define PROP_SIZE_OPTS BOOST_PP_SLOT(1)
// #endif

#if   !defined(DEBUG) && !defined(COLORIZE)
#define PROP_SIZE_OPTS 0
#elif  defined(DEBUG) && !defined(COLORIZE) || \
      !defined(DEBUG) &&  defined(COLORIZE)
#define PROP_SIZE_OPTS 1
#elif  defined(DEBUG) &&  defined(COLORIZE)
#define PROP_SIZE_OPTS 2
#endif

#define PROP_DECL_DEF_OPTS()     PROP_DECL_DEF(Sys::options_t, PROP_SIZE_OPTS)
#define PROP_IMPL_DEF_OPTS()     PROP_IMPL_DEF(Sys::options_t)
#define PROP_IMPL_PROP_OPTS(M)   PROP_IMPL_PROP(M, Sys::options_t, M)
#define PROP_IMPL_CONSTR_OPTS(M) PROP_IMPL_CONSTR(Sys::options_t, M)
#define PROP_IMPL_PRINT_OPTS(M)  PROP_IMPL_PRINT(Sys::options_t, M)

#define PROP_IMPL_CONSTR_OPTS(M) PROP_IMPL_CONSTR(Sys::options_t, M)

PROP_IMPL_CONSTR_OPTS(colorize)
{
    bool v;
    if (!Prop::constr<bool>(str, v))
        return false;
    PROP_SELF = v;
    return true;
}

PROP_DECL_DEF_OPTS();
PROP_IMPL_DEF_OPTS() {
#ifdef DEBUG
    PROP_IMPL_PROP_OPTS(debug),
#endif
#ifdef COLORIZE
    PROP_IMPL_PROP_OPTS(colorize),
#endif
};

} // namespace Prop

namespace Opts {

struct env_t
{
    const char* operator[](unsigned i) const
    {
        static const char* names[] = {
#ifdef DEBUG
            "DEBUG",    // Prop::opt_t::debug
#endif
#ifdef COLORIZE
            "COLORIZE", // Prop::opt_t::colorize
#endif
        };
        const char *r = nullptr;
        if (i < Ext::array_size(names))
            r = getenv(names[i]);
        return r ? r : "";
    }
};

static void init()
{
    using namespace Sys;
    Prop::Prop<options_t>::constr(env_t(), opts);
}

#ifdef DEBUG
static void dump()
{
    using namespace std;
    using namespace Sys;
    const auto dump= getenv("DUMP_SYS_OPTS");
    if (!dump || strcmp(dump, "1"))
        return;
    cerr << program << ": options:" << endl;
    Prop::Prop<options_t>::print(cerr, opts);
}
#endif

} // namespace Opts

namespace Sys {

static void __attribute__ ((constructor)) init()
{
    Opts::init();
#ifdef DEBUG
    Opts::dump();
#endif
}

#endif // SYS_OPTS

const char* svformat(const char *fmt, va_list args) noexcept
{
    static char buf[256];

    vsnprintf(buf, sizeof buf - 1, fmt, args);
    buf[255] = 0;

    return buf;
}

const char* sformat(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    auto r = svformat(fmt, args);
    va_end(args);

    return r;
}

void error(const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    auto err = svformat(msg, args);
    va_end(args);

    fprintf(stderr, "%s: error: %s\n", program, err);
    exit(1);
}

static void die(const char* msg, ...)
    PRINTF_FMT(1)
    NORETURN;

static void die(const char* msg, ...)
{
    char buf[256];

    va_list args;
    va_start(args, msg);
    vsnprintf(buf, sizeof buf - 1, msg, args);
    va_end(args);
    buf[255] = 0;

    fprintf(stderr, "%s: fatal error: %s\n", program, buf);

    exit(127);
}

void assert_failed(
    const char* file, int line, const char* func, const char* expr)
{
    char buf[256];

    snprintf(
        buf, sizeof buf - 1,
        "%s:%d:%s: %s", file, line, func, expr);
    buf[255] = 0;

    die("assertion failed: %s", buf);
}

void unexpect_error(
    const char* file, int line, const char* func, const char* msg, ...)
{
    char buf[256];

    va_list args;
    va_start(args, msg);
    vsnprintf(buf, sizeof buf - 1, msg, args);
    va_end(args);
    buf[255] = 0;

    die("unexpected error: %s:%d:%s: %s", file, line, func, buf);
}

const char file_t::stdin_name[] = "<stdin>";

file_t::Error::Error(const char* msg) :
    std::runtime_error(msg)
{}

file_t::Error::Error(const std::string& msg) :
    std::runtime_error(msg)
{}

file_t::file_t(const char* _name) :
    name(_name),
    desc(-1)
{
    if (name) {
        desc = ::open(name, O_RDONLY);
        if (desc == -1)
            sys_error("cannot open file");
    }
    else {
        name = stdin_name;
        desc = 0;
    }
}

file_t::~file_t()
{
    if (desc != -1 && ::close(desc) != 0)
        sys_error("failed closing file");
}

// stev: ISO/IEC 14882:2011 12.8.32 mandates that the 'buf'
// objects in 'file_t::read_regular' and 'file_t::read_stdin'
// by 'return buf' be either constructed directly in the
// returned value (i.e. the compiler is eliding the copy
// construction) or, otherwise, be considered as a rvalue --
// hence, no need for the return statement to be of form
// 'return std::move(buf)'. Note that a 'return' of this
// latter form actually prevents the compiler to eventually
// apply the copy-eliding return value optimization (RVO).

file_t::buf_t file_t::read_regular()
{
    struct stat stat;
    if (::fstat(desc, &stat))
        sys_error("cannot stat file");
    if (!S_ISREG(stat.st_mode))
        error("file is not regular");
    if (Ext::integer_cast<Ext::widest_uint_t>(stat.st_size) > max_size)
        error("file is too large");
    auto size = Ext::integer_cast<size_t>(stat.st_size);
    buf_t buf(size);
    if (size && buf.get() == nullptr)
        OUT_OF_MEMORY();
    auto size2 = ::read(desc, buf.get(), size);
    if (size2 < 0 || Ext::integer_cast<size_t>(size2) != size)
        sys_error("failed reading file");
    auto b = buf.end(buf.get() + size);
    SYS_ASSERT(b);
    // stev: ISO/IEC 14882:2011 12.8.32:
    // not returning 'std::move(buf)'
    return buf;
}

file_t::buf_t file_t::read_stdin()
{
    static const ssize_t inc = 4096;
    char chunk[1024];
    size_t size = 0;
    size_t d = 0;
    buf_t buf;
    while (1) {
        SYS_ASSERT(d <= size);
        auto n = ::read(desc, chunk, sizeof chunk);
        // => d + n <= size + n
        if (n < 0)
            sys_error("failed reading input");
        if (n == 0)
            break;
        if (buf.size() + n >= max_size)
            error("input is too large");
        auto q = buf.get();
        if (d + n > size) {
            size += n < inc ? inc : n;
            // => d + n <= size
            q = buf.realloc(size);
            if (q == nullptr)
                OUT_OF_MEMORY();
        }
        memcpy(q + d, chunk, n);
        d += n;
    }
    auto p = buf.get() + d;
    auto b = buf.end(p);
    SYS_ASSERT(b);
    // stev: ISO/IEC 14882:2011 12.8.32:
    // not returning 'std::move(buf)'
    return buf;
}

file_t::buf_t file_t::read()
{
    static buf_t (file_t::*const reads[])() = {
        &file_t::read_regular,
        &file_t::read_stdin
    };
    return (this->*Ext::array(reads)[!desc])();
}

void file_t::error(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    auto err = vformat(msg, args);
    va_end(args);

    throw Error(format("%s: %s", name, err.c_str()));
}

void file_t::sys_error(const char* msg, ...)
{
    int sys_err = errno;

    va_list args;
    va_start(args, msg);
    auto err = vformat(msg, args);
    va_end(args);

    error("%s: %s", err.c_str(), strerror(sys_err));
}

base_ext_func_t::Error::Error(const char* msg) :
    std::runtime_error(msg)
{}

base_ext_func_t::Error::Error(const std::string& msg) :
    std::runtime_error(msg)
{}

class base_ext_func_t::buf_t : private mem_t
{
public:
    buf_t(const buf_t&) = delete;
    buf_t& operator=(const buf_t&) = delete;

    buf_t(buf_t&&) = delete;
    buf_t& operator=(buf_t&&) = delete;

    explicit buf_t(size_t sz) :
        mem_t(sz)
    { if (get() == nullptr) OUT_OF_MEMORY(); }

    operator char*() { return get(); }

private:
    using mem_t::get;
};

void base_ext_func_t::load(
    const char* home_dir,
    const char* name,
#ifdef DEBUG
    bool debug,
#endif
    const char* prefix)
{
    unload();
    if (name == nullptr)
        return;
    size_t sz0 = 1 + strlen(home_dir);            // +1 because of '/' after home_dir
    size_t sz1 = prefix ? 1 + strlen(prefix) : 0; // +1 because of '-' between prefix and name
    size_t sz = strlen(name);
    buf_t buf(sz0 + sz1 + sz + 5);                // +5 because of: ".so\0" + EOS
    char *mod = buf;
    if (!strchr(name, '/')) {
        strcat(strcpy(mod, home_dir), "/");
        mod += sz0;
        if (sz1) {
            strcat(strcpy(mod, prefix), "-");
            mod += sz1;
        }
    }
    strcpy(mod, name);
    char *file = buf;
    char *func = nullptr;
    char *ptr = strchr(mod, ':');
    if (ptr) {
        if (ptr + 2 >= mod + sz || ptr[1] != ':')
            error("invalid ext func name '%s'", name);
        *ptr = 0;
        memmove(ptr + 4, ptr, sz - (ptr - mod) + 1);
        func = ptr + 6;
    }
    else
        ptr = mod + sz;
    static const char ext[] = ".so";
    if (sz < 3 ||
        strcmp(ptr - 3, ext))
        strcpy(ptr, ext);
    else
        ptr -= 3;
#ifdef DEBUG
    static const char ext_func[] = "ext-func";
    if (debug)
        std::cerr
            << sformat("%s: %s: file: '%s'", program, ext_func, file)
            << std::endl;
#endif
    handle = dlopen(file, RTLD_NOW);
    if (handle == nullptr)
        error("failed loading module '%s': %s", mod, dlerror());
    if (func == nullptr) {
        func = strrchr(mod, '/');
        if (func)
            *func ++ = 0;
        else
            func = mod;
        *ptr = 0;
    }
    dlerror();
#ifdef DEBUG
    if (debug)
        std::cerr
            << sformat("%s: %s: func: '%s'", program, ext_func, func)
            << std::endl;
#endif
    mod_func = dlsym(handle, func);
    const char *err = dlerror();
    if (err)
        error("cannot find func '%s': %s", func, err);
}

void base_ext_func_t::unload()
{
    if (handle) {
        if (dlclose(handle))
            error("error unloading module: %s", dlerror());
        handle = nullptr;
    }
    mod_func = nullptr;
}

void base_ext_func_t::error(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    auto err = vformat(msg, args);
    va_end(args);

    throw Error(err);
}

size_t digits(unsigned val)
{
    if (val == 0)
        return 1;
    size_t r = 0;
    for (; val != 0; val /= 10)
        r ++;
    return r;
}

} // namespace Sys

