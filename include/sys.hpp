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

#ifndef __SYS_HPP
#define __SYS_HPP

#include "config.h"

// stev: request PRIxPTR be defined
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <climits>

#include <limits>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>

#define STRINGIFY0(S) #S
#define STRINGIFY(S)  STRINGIFY0(S)

#define PRINTF_FMT(F) \
    __attribute__ ((format(printf, F, F + 1)))
#define NORETURN \
    __attribute__ ((noreturn))
#define UNUSED \
    __attribute__ ((unused))

// stev: assert 8 bits per byte 
#if CHAR_BIT != 8
# error expected CHAR_BIT == 8
#endif

// stev: below assume 8 bits per byte 
#if __WORDSIZE == 32
# define UINTPTR_FMT_WIDTH 8
#elif __WORDSIZE == 64
# define UINTPTR_FMT_WIDTH 16
#else
# error expected __WORDSIZE to be 32 or 64
#endif

#define UINTPTR_FORMAT \
    "0x%0" STRINGIFY(UINTPTR_FMT_WIDTH) PRIxPTR

#define EXT_FUNC_ENTRY extern "C"

namespace Sys {

#if __cplusplus >= 201103L
# define CXX_ASSERT2(E, M) \
    static_assert(E, M)
#else
template<bool b>
struct cxx_assert_t;

template<>
struct cxx_assert_t<true>
{ enum { value }; };

# define CXX_ASSERT2(E, M) \
    do { \
        (void) Sys::cxx_assert_t<(X)>::value; \
    } \
    while (0)
#endif

#define CXX_ASSERT(E) \
    CXX_ASSERT2(E, #E)

void assert_failed(
    const char *file, int line,
    const char *func, const char* expr)
    NORETURN;

void unexpect_error(
    const char* file, int line,
    const char* func, const char* msg, ...)
    PRINTF_FMT(4)
    NORETURN;

#define OUT_OF_MEMORY() \
    do { \
        SYS_UNEXPECT_ERR("out of memory"); \
    } \
    while (0)

#define SYS_UNEXPECT_ERR(M, ...) \
    do { \
        Sys::unexpect_error(__FILE__, __LINE__, __func__, M, ## __VA_ARGS__); \
    } \
    while (0)

#ifdef DEBUG
# define SYS_ASSERT(E) \
    do { \
        if (!(E)) \
            Sys::assert_failed(__FILE__, __LINE__, __func__, #E); \
    } \
    while (0)
#else
# define SYS_ASSERT(E) \
    do {} while (0)
#endif

class mem_t
{
public:
    typedef char* ptr_t;

    mem_t(const mem_t&) = delete;
    mem_t& operator=(const mem_t&) = delete;

    mem_t() noexcept :
        p(nullptr), n(0), e(nullptr)
    {}

    mem_t(size_t _n) noexcept :
        mem_t()
    { set(realloc(nullptr, _n), _n); }

    mem_t(mem_t&& m) noexcept :
        p(m.p), n(m.n), e(m.e)
    { m.reset(); }

    ~mem_t() noexcept
    { if (p) free(p); reset(); }

    mem_t& operator=(mem_t&& m) noexcept
    // move-and-swap idiom
    { mem_t t(std::move(m)); swap(t); return *this; }

    ptr_t get() const noexcept
    { return p; }

    size_t size() const noexcept
    { return n; }

    ptr_t end() const noexcept
    { return e; }

    bool end(ptr_t _e) noexcept
    { bool r = _e >= p && _e <= p + n; if (r) e = _e; return r; }

    ptr_t realloc(size_t _n) noexcept
    { set(realloc(p, _n), _n); return p; }

    ptr_t release() noexcept
    { ptr_t r = p; reset(); return r; }

private:
    void swap(mem_t& m) noexcept
    { std::swap(p, m.p); std::swap(n, m.n); std::swap(e, m.e); }

    void reset() noexcept
    { p = nullptr; n = 0; e = nullptr; }

    void set(ptr_t _p, size_t _n) noexcept
    { p = _p; n = _p ? _n : 0; e = _p + n; }

    static ptr_t realloc(ptr_t p, size_t n) noexcept
    {
        ptr_t r = n ? static_cast<ptr_t>(::realloc(p, n)) : nullptr;
        if (p && !r) ::free(p);
        return r;
    }

    static void free(ptr_t p) noexcept
    { ::free(p); }

    ptr_t  p;
    size_t n;
    ptr_t  e;
};

class buf_t : private mem_t
{
public:
    typedef mem_t base_t;

    buf_t(const buf_t&) = delete;
    buf_t& operator=(const buf_t&) = delete;

    buf_t(buf_t&&) = delete;
    buf_t& operator=(buf_t&&) = delete;

    buf_t(size_t _incr = 256) :
        base_t(),
        incr(_incr),
        size(0),
        ptr(nullptr)
    {
        enlarge(incr);
    }

#ifdef DEBUG
    class iterator :
        public std::iterator<std::input_iterator_tag, const char>
    {
    public:
        iterator& operator++()
        {
            while (*ptr && ptr < end)
                ++ ptr;
            if (ptr < end)
                ++ ptr;
            return *this;
        }

        iterator operator++(int)
        { auto r = *this; operator++(); return r; }

        const char* operator*() const
        { return ptr; }

        bool operator==(const iterator& a) const
        { return ptr == a.ptr && end == a.end; }
        bool operator!=(const iterator& a) const
        { return !operator==(a); }

    private:
        iterator(char *_ptr, char *_end) : ptr(_ptr), end(_end) {}

        char *ptr;
        char *end;

        friend class buf_t;
    };

    inline iterator begin() const
    { return iterator(get(), ptr); }

    inline iterator end() const
    { return iterator(ptr, ptr); }
#endif // DEBUG

    const char* nassign(const char *key, size_t sz)
    { ptr = get(); return nadd(key, sz); }

    const char* nadd(const char *key, size_t sz);

    const char* assign(const char *key)
    { return nassign(key, strlen(key)); }

    const char* add(const char *key)
    { return nadd(key, strlen(key)); }

protected:
    using base_t::get;

private:
    size_t incr;
    size_t size;
    char  *ptr;

    void enlarge(size_t n);
};

#ifdef SYS_OPTS

struct options_t
{
#ifdef DEBUG
    unsigned debug;
#endif
#ifdef COLORIZE
    unsigned colorize;
#endif
};

extern struct options_t opts;

#endif // SYS_OPTS

const char* svformat(const char *fmt, va_list args) noexcept;
const char* sformat(const char *fmt, ...) PRINTF_FMT(1);
std::string format(const char *fmt, ...) PRINTF_FMT(1);

inline std::string vformat(const char *fmt, va_list args) noexcept
{
    char buf[256];

    vsnprintf(buf, sizeof buf - 1, fmt, args);
    buf[255] = 0;

    return buf;
}

inline std::string format(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    auto str = vformat(fmt, args);
    va_end(args);

    return str;
}

class file_t
{
public:
    static const size_t max_size = 10485760; // ten megabytes
    static const char stdin_name[];

    struct Error : public std::runtime_error
    {
        Error(const char* msg);
        Error(const std::string& msg);
    };
    typedef mem_t buf_t;

    file_t(const file_t&) = delete;
    file_t& operator=(const file_t&) = delete;

    file_t(file_t&&) = delete;
    file_t& operator=(file_t&&) = delete;

    file_t(const char* _name);
    ~file_t();

    buf_t read();

private:
    void error(const char* msg, ...) PRINTF_FMT(2);
    void sys_error(const char* msg, ...) PRINTF_FMT(2);

    buf_t read_regular();
    buf_t read_stdin();

    const char *name;
    int         desc;
};

typedef int ext_func_result_t;

class base_ext_func_t
{
public:
    struct Error : public std::runtime_error
    {
        Error(const char* msg);
        Error(const std::string& msg);
    };

    base_ext_func_t(const base_ext_func_t&) = delete;
    base_ext_func_t& operator=(const base_ext_func_t&) = delete;

    base_ext_func_t(base_ext_func_t&&) = delete;
    base_ext_func_t& operator=(base_ext_func_t&&) = delete;

    base_ext_func_t() :
        mod_func(nullptr),
        handle(nullptr)
    {}
    ~base_ext_func_t()
    { unload(); }

    void load(
        const char* home_dir,
        const char* name,
#ifdef DEBUG
        bool debug, 
#endif
        const char* prefix);

    void unload();

    bool is_loaded() const
    { return mod_func; }

protected:
    void *mod_func;

private:
    class buf_t;

    void error(const char* msg, ...) PRINTF_FMT(2);

    void *handle;
};

template<typename O, typename P>
class ext_func_t : public base_ext_func_t
{
public:
    typedef O obj_t;
    typedef P opt_t;

    typedef ext_func_result_t (*func_t)(const obj_t*, const opt_t*);

    ext_func_result_t call(const obj_t* obj, const opt_t* opt) const
    {
        SYS_ASSERT(mod_func != nullptr);
        return reinterpret_cast<func_t>(mod_func)(obj, opt);
    }
};

template<typename O, typename P>
ext_func_result_t run_ext_func(
    const O* obj, const P* opt, const char* prefix = nullptr)
{
    static ext_func_t<O, P> ext_func;

    // stev: opt->ext_func_name == nullptr means: 
    // do not run any extension func; only return
    // zero -- indicating a successful completion
    if (opt->ext_func_name == nullptr)
        return 0;

    if (!ext_func.is_loaded())
        ext_func.load(
            opt->home_dir,
            opt->ext_func_name,
#ifdef DEBUG
            opt->debug,
#endif
            prefix);

    return ext_func.call(obj, opt);
}

template<typename O, typename P>
inline ext_func_result_t run_ext_func(
    const std::unique_ptr<O>& obj, const P* opt,
    const char* prefix = nullptr)
{ return run_ext_func(obj.get(), opt, prefix); }

size_t digits(unsigned val);

} // namespace Sys

#endif /*  __SYS_HPP */

