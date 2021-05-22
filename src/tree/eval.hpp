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

#ifndef __EVAL_HPP
#define __EVAL_HPP

#include "config.h"

#include <cstring>

#include <iostream>
#include <algorithm>
#include <functional>

#include "std-ext.hpp"
#include "sys.hpp"

namespace Obj {

struct ObjError : public std::runtime_error
{
    ObjError(const char* msg);
    ObjError(const std::string& msg);
};

void error(const char* msg, ...) PRINTF_FMT(1);

class obj_type_t;
struct arg_def_t;
struct print_info_t;

struct type_cast_info_t
{
    type_cast_info_t() :
        strict(true)
    {}

    type_cast_info_t(bool _strict) :
        strict(_strict)
    {}

    bool strict;
};

struct ast_type_t
{
    ast_type_t(
        const char _name[],
        const ast_type_t* _base,
        const arg_def_t _args[],
        size_t _narg) :
        name(_name),
        base(_base),
        args(_args),
        narg(_narg)
    {}

    bool type_cast(const ast_type_t* src, const type_cast_info_t& info) const;
    bool list_type_cast(const obj_type_t& src, const type_cast_info_t& info) const;

    void print(std::ostream& ost, const print_info_t& info) const;

    char const*       const name;
    ast_type_t const* const base;
    arg_def_t const*  const args;
    size_t            const narg;

    static void print_types(std::ostream&);

    static ast_type_t const* const types[];
    static size_t            const n_types;
};

struct cptr_type_t
{
    cptr_type_t(const char _name[]) :
        name(_name)
    {}

    bool list_type_cast(const obj_type_t& src, const type_cast_info_t& info) const;

    void print(std::ostream& ost, const print_info_t& info) const;

    char const* const name;
};

template<typename T, typename V>
class obj_traits_t;

template<typename S, typename T, bool A>
class obj_set_type_traits_t;

template<typename S, typename T, bool A>
class obj_base_t : protected T
{
public:
    typedef S self_t;
    typedef typename T::type_t type_t;
    typedef typename T::node_t node_t;
    typedef typename T::list_t list_t;
    typedef typename T::cptr_t cptr_t;

    obj_base_t(const obj_base_t& o)
    { assign(o); }

    template<typename V>
    obj_base_t(V v)
    { obj_traits_t<T, V>::template assign<S, A>(*this, v); }

    obj_base_t& operator=(const obj_base_t& o)
    { assign(o); return *this; }

    template<typename V>
    obj_base_t& operator=(V v)
    { obj_traits_t<T, V>::template assign<S, A>(*this, v); return *this; }

    bool is(type_t type) const
    { return get() == type; }

    template<typename V>
    bool is() const
    { return T::template is<V>(get()); }

    template<typename V>
    V as() const
    { return obj_traits_t<T, V>::template value<S, A>(*this); }

    template<typename V>
    const V* cast() const
    { return obj_traits_t<T, V>::template cast<S, A>(*this); }

    bool operator==(const obj_base_t& o) const
    { return equal(o); }
    bool operator!=(const obj_base_t& o) const
    { return !equal(o); }

private:
    template<typename T1, typename V>
    friend class obj_traits_t;

    friend class obj_set_type_traits_t<S, T, A>;

    self_t* self()
    { return static_cast<self_t*>(this); }

    const self_t* self() const
    { return static_cast<const self_t*>(this); }

    type_t get() const
    { return self()->type; }

    void assert(type_t type) const
    { check_type(type); SYS_ASSERT(is(type)); }

    void assign(type_t type)
    { check_type(type); self()->type = type; }

    void set(type_t type)
    { obj_set_type_traits_t<S, T, A>::set(*this, type); }

    void assign(const obj_base_t& o)
    { set(o.get()); u = o.u; }

    bool equal(const obj_base_t& o) const
    { return get() == o.get() && u == o.u; }

    template<typename V>
    void set()
    {
        typedef typename T::
            template traits_t<V> traits_t;
        set(traits_t::type);
    }

    template<typename V>
    void check() const
    {
        typedef typename T::
            template traits_t<V> traits_t;
        auto type = get();
        if (type != traits_t::type)
            error("type mismatch '%s': actual type is '%s'",
                Ext::array(self_t::types)[traits_t::type],
                Ext::array(self_t::types)[type]);
    }

    static void check_type(type_t type)
    {
        if (type < self_t::first_type &&
            type > self_t::last_type)
            error("invalid type '%d'", type);
    }

    union union_t
    {
        node_t node;
        list_t list;
        cptr_t cptr;
    };

    union_t u;
};

template<typename S, typename T>
class obj_set_type_traits_t<S, T, false>
{
    friend class obj_base_t<S, T, false>;

    typedef obj_base_t<S, T, false> obj_t;
    typedef typename T::type_t type_t;

    static void set(obj_t& o, type_t t)
    { o.assert(t); }
};

template<typename T, typename S>
class obj_set_type_traits_t<S, T, true>
{
    friend class obj_base_t<S, T, true>;

    typedef obj_base_t<S, T, true> obj_t;
    typedef typename T::type_t type_t;

    static void set(obj_t& o, type_t t)
    { o.assign(t); }
};

template<typename T>
struct obj_traits_t<T, typename T::type_t>
{
    typedef typename T::type_t type_t;

    template<typename S, bool A>
    static type_t value(const obj_base_t<S, T, A>& o)
    { return o.get(); }
};

template<typename T>
struct obj_traits_t<T, typename T::node_t>
{
    typedef typename T::node_t type_t;

    template<typename S, bool A>
    static const type_t* cast(const obj_base_t<S, T, A>& o)
    { return o.template is<type_t>() ? &o.u.node : nullptr; }

    template<typename S, bool A>
    static type_t value(const obj_base_t<S, T, A>& o)
    { o.template check<type_t>(); return o.u.node; }

    template<typename S, bool A>
    static void assign(obj_base_t<S, T, A>& o, type_t v)
    { o.template set<type_t>(); o.u.node = v; }
};

template<typename T>
struct obj_traits_t<T, typename T::list_t>
{
    typedef typename T::list_t type_t;

    template<typename S, bool A>
    static const type_t* cast(const obj_base_t<S, T, A>& o)
    { return o.template is<type_t>() ? &o.u.list : nullptr; }

    template<typename S, bool A>
    static type_t value(const obj_base_t<S, T, A>& o)
    { o.template check<type_t>(); return o.u.list; }

    template<typename S, bool A>
    static void assign(obj_base_t<S, T, A>& o, type_t v)
    { o.template set<type_t>(); o.u.list = v; }
};

template<typename T>
struct obj_traits_t<T, typename T::cptr_t>
{
    typedef typename T::cptr_t type_t;

    template<typename S, bool A>
    static const type_t* cast(const obj_base_t<S, T, A>& o)
    { return o.template is<type_t>() ? &o.u.cptr : nullptr; }

    template<typename S, bool A>
    static type_t value(const obj_base_t<S, T, A>& o)
    { o.template check<type_t>(); return o.u.cptr; }

    template<typename S, bool A>
    static void assign(obj_base_t<S, T, A>& o, type_t v)
    { o.template set<type_t>(); o.u.cptr = v; }
};

struct obj_consts_t
{
    enum type_t {
        node_obj,
        list_obj,
        cptr_obj,

        first_type = node_obj,
        last_type = cptr_obj
    };

    static char const* const types[3];
};

struct obj_types_t : obj_consts_t
{
    struct node_t
    {
        const ast_type_t* ast_type;
    };
    struct list_t
    {
        const ast_type_t* ast_type;
    };
    struct cptr_t
    {
        const cptr_type_t* cptr_type;
    };

    static node_t make_node(const ast_type_t* ast_type)
    { node_t r; r.ast_type = ast_type; return r; }

    static list_t make_list(const ast_type_t* ast_type)
    { list_t r; r.ast_type = ast_type; return r; }

    static cptr_t make_cptr(const cptr_type_t* cptr_type)
    { cptr_t r; r.cptr_type = cptr_type; return r; }

    static node_t make() { return make_node(nullptr); }

    template<typename V>
    static bool is(type_t type)
    { return traits_t<V>::type == type; }

    template<typename T>
    struct traits_t;
};

template<> struct obj_types_t::traits_t<obj_types_t::node_t>
{ static const type_t type = node_obj; };

template<> struct obj_types_t::traits_t<obj_types_t::list_t>
{ static const type_t type = list_obj; };

template<> struct obj_types_t::traits_t<obj_types_t::cptr_t>
{ static const type_t type = cptr_obj; };

struct obj_values_t;
class obj_value_t;

class obj_type_t :
    private obj_base_t<obj_type_t, obj_types_t, true>
{
public:
    typedef obj_base_t<obj_type_t, obj_types_t, true> base_t;

    typedef base_t::type_t type_t;
    typedef base_t::node_t node_t;
    typedef base_t::list_t list_t;
    typedef base_t::cptr_t cptr_t;

    obj_type_t() : base_t(obj_types_t::make())
    // stev: type is initialized by base_t
    { SYS_ASSERT(is<node_t>()); }

    obj_type_t(const obj_type_t& o) : base_t(o.base())
    // stev: type is initialized by base_t
    { SYS_ASSERT(is(o.type)); }

    template<typename V>
    obj_type_t(V v) : base_t(v)
    // stev: type is initialized by base_t
    { SYS_ASSERT(is<V>()); }

    obj_type_t& operator=(const obj_type_t& o)
    // stev: type is assigned by base_t
    { base_t::operator=(o.base()); SYS_ASSERT(is(o.type)); return *this; }

    template<typename V>
    obj_type_t& operator=(V v)
    // stev: type is assigned by base_t
    { base_t::operator=(v); SYS_ASSERT(is<V>()); return *this; }

    using base_t::is;
    using base_t::as;
    using base_t::cast;

    std::string type_name() const;
    bool type_cast(const obj_type_t& src, const type_cast_info_t& info) const;
    void print(std::ostream& ost, const print_info_t& info) const;

    using base_t::types;

private:
    friend class obj_base_t<obj_type_t, obj_types_t, false>;
    friend class obj_base_t<obj_value_t, obj_values_t, false>;
    friend class obj_base_t<obj_type_t, obj_types_t, true>;
    friend class obj_base_t<obj_value_t, obj_values_t, true>;

    const base_t& base() const
    { return *this; }

    type_t type;
};

struct arg_def_t
{
    arg_def_t() :
        arg_name(nullptr),
        arg_type()
    {}

    arg_def_t(
        const char* _arg_name,
        const obj_type_t& _arg_type) :
        arg_name(_arg_name),
        arg_type(_arg_type)
    {}

    void print(std::ostream& ost, const print_info_t& info) const;

    char const* const arg_name;
    obj_type_t  const arg_type;
};

template<typename T>
class ast_def_t;

template<typename T>
class cptr_def_t;

template<typename T>
inline constexpr const ast_type_t* ast_type()
{
    return &ast_def_t<T>::type;
}

template<typename T>
inline constexpr const cptr_type_t* cptr_type()
{
    return &cptr_def_t<T>::type;
}

template<typename T>
inline T cptr_cast(const char* arg)
{
    return cptr_def_t<T>::cast(arg);
}

template<typename T>
obj_type_t node_obj_type();

template<typename T>
obj_type_t list_obj_type();

template<typename T>
obj_type_t cptr_obj_type();

template<typename T>
static inline arg_def_t node(const char* arg_name)
{
    return arg_def_t(arg_name, node_obj_type<T>());
}

template<typename T>
static inline arg_def_t list(const char* arg_name)
{
    return arg_def_t(arg_name, list_obj_type<T>());
}

template<typename T>
static arg_def_t cptr(const char* arg_name)
{
    return arg_def_t(arg_name, cptr_obj_type<T>());
}

template<typename T>
class ast_def_t
{
public:
    friend const ast_type_t* ast_type<T>();

private:
    static const ast_type_t type;
    static const arg_def_t  args[];
    static const size_t     narg;
};

template<typename T>
class cptr_def_t
{
public:
    friend const cptr_type_t* cptr_type<T>();
    friend T cptr_cast<T>(const char*);

private:
    static const cptr_type_t type;

    static T cast(const char*);
    static void invalid_value(const char*);
};

template<typename T>
inline void cptr_def_t<T>::invalid_value(const char* arg)
{
    error("invalid %s value '%s'", type.name, arg);
}

struct obj_values_t : obj_consts_t
{
    typedef obj_types_t::type_t type_t;

    struct node_t
    {
        const void* ptr;
    };
    struct list_t
    {
        size_t size;
        void const* const* ptr;
    };
    struct cptr_t
    {
        const char* ptr;
    };

    static node_t make_node(const void* ptr)
    { node_t r; r.ptr = ptr; return r; }

    static list_t make_list(size_t size, void const* const* ptr)
    { list_t r; r.size = size; r.ptr = ptr; return r; }

    static cptr_t make_cptr(const char* ptr)
    { cptr_t r; r.ptr = ptr; return r; }

    template<typename V>
    static bool is(type_t type)
    { return traits_t<V>::type == type; }

    template<typename T>
    struct traits_t;
};

template<> struct obj_values_t::traits_t<obj_values_t::node_t>
{ static const type_t type = node_obj; };

template<> struct obj_values_t::traits_t<obj_values_t::list_t>
{ static const type_t type = list_obj; };

template<> struct obj_values_t::traits_t<obj_values_t::cptr_t>
{ static const type_t type = cptr_obj; };

class obj_value_t :
    public  obj_type_t,
    private obj_base_t<obj_value_t, obj_values_t, false>
{
public:
    typedef obj_base_t<obj_value_t, obj_values_t, false> base_t;

    typedef base_t::type_t type_t;
    typedef base_t::node_t node_t;
    typedef base_t::list_t list_t;
    typedef base_t::cptr_t cptr_t;

    template<typename T, typename V>
    obj_value_t(T t, V v) : obj_type_t(t), base_t(v)
    {
        CXX_ASSERT(
            obj_type_t::traits_t<T>::type ==
            base_t::traits_t<V>::type);
        SYS_ASSERT(obj_type_t::is<T>());
        SYS_ASSERT(base_t::is<V>());
    }

    bool is(type_t type) const
    { return obj_type_t::is(type); }

    template<typename V>
    bool is() const
    { return traits_t<V>::is(*this); }

    template<typename V>
    V as() const
    { return traits_t<V>::as(*this); }

    template<typename V>
    const V* cast() const
    { return traits_t<V>::cast(*this); }

    void print(std::ostream& ost, const print_info_t& info) const;

private:
    friend class obj_base_t<obj_value_t, obj_values_t, false>;

    template<typename V>
    struct traits_t;
};

template<>
struct obj_value_t::traits_t<obj_type_t::type_t>
{
    typedef obj_type_t::type_t type_t;

    static type_t as(const obj_value_t& o)
    { return o.obj_type_t::as<type_t>(); }
};

template<>
struct obj_value_t::traits_t<obj_type_t::node_t>
{
    typedef obj_type_t::node_t type_t;

    static bool is(const obj_value_t& o)
    { return o.obj_type_t::is<type_t>(); }

    static type_t as(const obj_value_t& o)
    { return o.obj_type_t::as<type_t>(); }

    static const type_t* cast(const obj_value_t& o)
    { return o.obj_type_t::cast<type_t>(); }
};

template<>
struct obj_value_t::traits_t<obj_type_t::list_t>
{
    typedef obj_type_t::list_t type_t;

    static bool is(const obj_value_t& o)
    { return o.obj_type_t::is<type_t>(); }

    static type_t as(const obj_value_t& o)
    { return o.obj_type_t::as<type_t>(); }

    static const type_t* cast(const obj_value_t& o)
    { return o.obj_type_t::cast<type_t>(); }
};

template<>
struct obj_value_t::traits_t<obj_type_t::cptr_t>
{
    typedef obj_type_t::cptr_t type_t;

    static bool is(const obj_value_t& o)
    { return o.obj_type_t::is<type_t>(); }

    static type_t as(const obj_value_t& o)
    { return o.obj_type_t::as<type_t>(); }

    static const type_t* cast(const obj_value_t& o)
    { return o.obj_type_t::cast<type_t>(); }
};

template<>
struct obj_value_t::traits_t<obj_value_t::node_t>
{
    typedef obj_value_t::node_t type_t;

    static bool is(const obj_value_t& o)
    { return o.base_t::is<type_t>(); }

    static type_t as(const obj_value_t& o)
    { return o.base_t::as<type_t>(); }

    static const type_t* cast(const obj_value_t& o)
    { return o.base_t::cast<type_t>(); }
};

template<>
struct obj_value_t::traits_t<obj_value_t::list_t>
{
    typedef obj_value_t::list_t type_t;

    static bool is(const obj_value_t& o)
    { return o.base_t::is<type_t>(); }

    static type_t as(const obj_value_t& o)
    { return o.base_t::as<type_t>(); }

    static const type_t* cast(const obj_value_t& o)
    { return o.base_t::cast<type_t>(); }
};

template<>
struct obj_value_t::traits_t<obj_value_t::cptr_t>
{
    typedef obj_value_t::cptr_t type_t;

    static bool is(const obj_value_t& o)
    { return o.base_t::is<type_t>(); }

    static type_t as(const obj_value_t& o)
    { return o.base_t::as<type_t>(); }

    static const type_t* cast(const obj_value_t& o)
    { return o.base_t::cast<type_t>(); }
};

inline obj_type_t node_obj_type(const ast_type_t* ast_type)
{
    return obj_type_t(obj_types_t::make_node(ast_type));
}

template<typename T>
inline obj_type_t node_obj_type()
{
    return node_obj_type(ast_type<T>());
}

inline obj_type_t list_obj_type(const ast_type_t* ast_type)
{
    return obj_type_t(obj_types_t::make_list(ast_type));
}

template<typename T>
inline obj_type_t list_obj_type()
{
    return list_obj_type(ast_type<T>());
}

inline obj_type_t cptr_obj_type(const cptr_type_t* cptr_type)
{
    return obj_type_t(obj_types_t::make_cptr(cptr_type));
}

inline obj_type_t cptr_obj_type()
{
    return cptr_obj_type(cptr_type<const char*>());
}

template<typename T>
inline obj_type_t cptr_obj_type()
{
    return cptr_obj_type(cptr_type<T>());
}

inline obj_value_t node_obj_value(
    const ast_type_t* ast_type, const void* ptr)
{
    return obj_value_t(
        obj_types_t::make_node(ast_type),
        obj_values_t::make_node(ptr));
}

inline obj_value_t list_obj_value(
    const ast_type_t* ast_type, size_t sz, void const* const* ptr)
{
    return obj_value_t(
        obj_types_t::make_list(ast_type),
        obj_values_t::make_list(sz, ptr));
}

inline obj_value_t cptr_obj_value(const char* ptr)
{
    return obj_value_t(
        obj_types_t::make_cptr(cptr_type<const char*>()),
        obj_values_t::make_cptr(ptr));
}

template<typename T>
T cptr_obj(const char* cptr);

template<>
inline obj_type_t cptr_obj<obj_type_t>(const char* cptr UNUSED)
{
    return cptr_obj_type();
}

template<>
inline obj_value_t cptr_obj<obj_value_t>(const char* cptr)
{
    return cptr_obj_value(cptr);
}

std::string type_name(const obj_type_t& obj);

template<typename T>
struct obj_t
{
    typedef T value_t;

    obj_t(const char* _name, value_t _value) :
        name(_name),
        value(_value)
    {}

    obj_t(value_t _value) :
        name(nullptr),
        value(_value)
    {}

    void print(std::ostream& ost, const print_info_t& info) const;

    const char* name;
    value_t     value;
};

template<typename T>
class obj_stack_t
{
public:
    typedef Obj::obj_t<T> obj_t;

    obj_stack_t(const obj_stack_t&) = delete;
    obj_stack_t& operator=(const obj_stack_t&) = delete;

    obj_stack_t(obj_stack_t&&) = delete;
    obj_stack_t& operator=(obj_stack_t&&) = delete;

    virtual ~obj_stack_t() {}

    virtual bool empty() const = 0;
    virtual size_t size() const = 0;

    virtual const obj_t& operator[](size_t k) const = 0;
    virtual const obj_t& top() const = 0;
    virtual void  pop(size_t n) = 0;

protected:
    obj_stack_t() {}
};

template<typename T>
struct ObjMaker
{
    typedef T val_t;
    typedef Obj::obj_t<val_t> obj_t;
    typedef Obj::obj_stack_t<val_t> obj_stack_t;

    virtual ~ObjMaker() {}

    virtual val_t make_node(const TreeAST::TreeNode* node, obj_stack_t& args) = 0;
    virtual val_t make_list(const TreeAST::NodeList* list, obj_stack_t& args) = 0;
    virtual val_t make_cptr(const TreeAST::Literal* cptr) = 0;
};

struct eval_opts_t
{
    eval_opts_t() :
        trace_eval(false),
        verbose(false)
    {}

    eval_opts_t(
        bool _trace_eval,
        bool _verbose) :
        trace_eval(_trace_eval),
        verbose(_verbose)
    {}

    bool trace_eval;
    bool verbose;
};

template<typename T, typename V>
inline T eval(const TreeAST::Tree* tree, ObjMaker<V>* maker, const eval_opts_t& opt)
{
    extern V eval(const TreeAST::Tree* tree, ObjMaker<V>* maker, const eval_opts_t& opt);
    return static_cast<T>(eval(tree, maker, opt));
}

template<typename T>
class TypeCheck
{
public:
    typedef T val_t;
    typedef Obj::obj_t<val_t> obj_t;
    typedef Obj::obj_stack_t<val_t> obj_stack_t;

    TypeCheck(const type_cast_info_t& _info) :
        info(_info), narg_error(false)
    {}

    template<typename V>
    class auto_t
    {
    public:
        typedef V inst_t;
        typedef typename inst_t::type_t type_t;

        auto_t(const auto_t&) = delete;
        auto_t& operator=(const auto_t&) = delete;

    private:
        struct val_t
        {
            typedef unsigned bits_t;
            enum { init_bit = 1 << 0, done_bit = 1 << 1 };

            val_t(const inst_t& _inst) :
                bits(0), inst(_inst)
            {}

            void swap(val_t& v) noexcept
            {
                std::swap(bits, v.bits);
                std::swap(inst, v.inst);
            }

            bits_t bits;
            inst_t inst;
        };

    public:
        auto_t(const inst_t& inst) :
            v(inst)
        {
            v.bits |= val_t::init_bit;
            v.inst.init();
        }
        ~auto_t()
        {
            if (v.bits & val_t::done_bit)
                return;
            v.bits |= val_t::done_bit;
            v.inst.done();
        }

        auto_t(auto_t&& a) noexcept : v(a.v)
        { a.release(); }

        auto_t& operator=(auto_t&& a) noexcept
        // move-and-swap idiom
        { auto_t t(std::move(a)); swap(t); return *this; }

        // stev: a convenient conversion to const inst_t::type_t*
        operator const type_t*() const { return v.inst.type; }

    private:
        void release() noexcept
        { v.bits |= val_t::init_bit | val_t::done_bit; }

        void swap(auto_t& a) noexcept
        { v.swap(a.v); }

        val_t v;
    };

    template<typename O, typename N>
    class check0_t
    {
    public:
        typedef O type_t;
        typedef N node_t;

        check0_t(
            TypeCheck* _self,
            const type_t* _type,
            const node_t* _node,
            obj_stack_t& _args,
            size_t _narg) :
            self(_self),
            type(_type),
            node(_node),
            args(&_args),
            narg(_narg)
        {}

        TypeCheck*    const self;
        type_t const* const type;
        node_t const* const node;
        obj_stack_t*  const args;
        size_t        const narg;

    protected:
        void done()
        {
            self->pop_args(narg, *args);
        }
    };

    class check0_node_t : public check0_t<ast_type_t, TreeAST::TreeNode>
    {
    public:
        typedef check0_t<ast_type_t, TreeAST::TreeNode> base_t;
        typedef typename base_t::type_t type_t;
        typedef typename base_t::node_t node_t;

        check0_node_t(
            TypeCheck* _self,
            const type_t* _type,
            const node_t* _node,
            obj_stack_t& _args) :
            base_t(
                _self,
                _type,
                _node,
                _args,
                _type->narg)
        {}

    private:
        friend class auto_t<check0_node_t>;

        using base_t::self;
        using base_t::type;
        using base_t::node;
        using base_t::args;

        void init()
        {
            self->check_node_args(type, node, *args);
        }
    };

    class check0_list_t : public check0_t<ast_type_t, TreeAST::NodeList>
    {
    public:
        typedef check0_t<ast_type_t, TreeAST::NodeList> base_t;
        typedef typename base_t::type_t type_t;
        typedef typename base_t::node_t node_t;

        check0_list_t(
            TypeCheck* _self,
            const type_t* _type,
            const node_t* _node,
            obj_stack_t& _args) :
            base_t(
                _self,
                _type,
                _node,
                _args,
                _node->elems.first)
        {}

    private:
        friend class auto_t<check0_list_t>;

        using base_t::self;
        using base_t::type;
        using base_t::node;
        using base_t::args;

        void init()
        {
            self->check_list_args(type, node, *args);
        }
    };

    typedef auto_t<check0_node_t> check_node_t;
    typedef auto_t<check0_list_t> check_list_t;

    template<typename N>
    check_node_t check_node(const TreeAST::TreeNode* node, obj_stack_t& args)
    {
        return check_node(ast_type<N>(), node, args);
    }

    template<typename N>
    check_list_t check_list(const TreeAST::NodeList* list, obj_stack_t& args)
    {
        return check_list(ast_type<N>(), list, args);
    }

private:
    template<typename V>
    auto_t<V> make_auto(
        const typename V::type_t* type, const typename V::node_t* node, obj_stack_t& args)
    {
        return auto_t<V>(V(this, type, node, args));
    }

    check_node_t check_node(
        const ast_type_t* type, const TreeAST::TreeNode* node, obj_stack_t& args)
    {
        return make_auto<check0_node_t>(type, node, args);
    }

    check_list_t check_list(
        const ast_type_t* type, const TreeAST::NodeList* list, obj_stack_t& args)
    {
        return make_auto<check0_list_t>(type, list, args);
    }

    void check_node_arg_name(
        const ast_type_t* type, const arg_def_t* def, size_t k, const obj_t& obj) const;

    void check_node_arg_value(
        const ast_type_t* type, const arg_def_t* def, size_t k, const obj_t& obj) const;

    void check_node_arg(
        const ast_type_t* type, const arg_def_t* def, size_t k, const obj_t& obj) const;

    void check_node_args(
        const ast_type_t* type, const TreeAST::TreeNode* node, obj_stack_t& args);

    void check_list_arg(
        const ast_type_t* type, size_t k, const obj_t& obj) const;

    void check_list_args(
        const ast_type_t* type, const TreeAST::NodeList* list, obj_stack_t& args) const;

    void pop_args(size_t n_arg, obj_stack_t& args);

    const type_cast_info_t info;
    bool narg_error;
};

template<typename M, typename T>
class BasicMaker :
    public ObjMaker<T>,
    protected TypeCheck<T>
{
protected:
    typedef M self_t;
    typedef ObjMaker<T> base_t;
    typedef typename base_t::val_t val_t;
    typedef typename base_t::obj_t obj_t;
    typedef typename base_t::obj_stack_t obj_stack_t;
    typedef TypeCheck<T> type_check_t;

    BasicMaker(const type_cast_info_t& _info) :
        type_check_t(_info)
    {}

    val_t make_node(const TreeAST::TreeNode* node, obj_stack_t& args);
    val_t make_list(const TreeAST::NodeList* list, obj_stack_t& args);
    val_t make_cptr(const TreeAST::Literal* cptr);

    struct new_funcs_t
    {
        typedef val_t (self_t::*new_node_t)(const TreeAST::TreeNode*, obj_stack_t&);
        typedef val_t (self_t::*new_list_t)(const TreeAST::NodeList*, obj_stack_t&);

        new_funcs_t(const char* _name) :
            name(_name), new_node(nullptr), new_list(nullptr)
        {}

        new_funcs_t(const char* _name, new_node_t _new_node, new_list_t _new_list) :
            name(_name), new_node(_new_node), new_list(_new_list)
        {}

        const char* name;
        new_node_t  new_node;
        new_list_t  new_list;
    };

private:
    self_t* self() { return static_cast<self_t*>(this); }

    static const new_funcs_t* find(const char* name);
};

template<typename M, typename T>
const typename BasicMaker<M, T>::new_funcs_t* BasicMaker<M, T>::find(
    const char* name)
{
    using namespace std;
    static const new_funcs_t* cache = nullptr;
    if (cache && strcmp(cache->name, name) == 0)
        return cache;
    auto beg = self_t::new_funcs;
    auto end = beg + self_t::n_new_func;
    auto rng = equal_range(beg, end, name,
        [](const new_funcs_t& a, const new_funcs_t& b)
            { return strcmp(a.name, b.name) < 0; });
    SYS_ASSERT(
        rng.second == rng.first ||
        rng.second == rng.first + 1);
    if (rng.second == rng.first)
        error("unknown AST type name '%s'", name);
    size_t k = Ext::ptr_diff(rng.first, beg);
    SYS_ASSERT(k < self_t::n_new_func);
    cache = &self_t::new_funcs[k];
    return cache;
}

template<typename M, typename T>
typename BasicMaker<M, T>::val_t BasicMaker<M, T>::make_node(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    return (self()->*find(node->name->ident)->new_node)(node, args);
}

template<typename M, typename T>
typename BasicMaker<M, T>::val_t BasicMaker<M, T>::make_list(
    const TreeAST::NodeList* list, obj_stack_t& args)
{
    return (self()->*find(list->base->ident)->new_list)(list, args);
}

template<typename M, typename T>
typename BasicMaker<M, T>::val_t BasicMaker<M, T>::make_cptr(
    const TreeAST::Literal* cptr)
{
    return cptr_obj<val_t>(cptr->str);
}

struct print_info_t
{
    print_info_t() :
        verbose(false)
    {}

    print_info_t(bool _verbose) :
        verbose(_verbose)
    {}

    print_info_t(const eval_opts_t& opt) :
        verbose(opt.verbose)
    {}

    bool verbose;
};

template<typename T>
struct print_t;

template<>
struct print_t<const ast_type_t*>
{
    print_t(const ast_type_t* _obj, const print_info_t& _info) :
        obj(_obj), info(_info)
    {}

    void print(std::ostream& ost) const
    { obj->print(ost, info); }

    const ast_type_t* obj;
    const print_info_t info;
};

template<>
struct print_t<const cptr_type_t*>
{
    print_t(const cptr_type_t* _obj, const print_info_t& _info) :
        obj(_obj), info(_info)
    {}

    void print(std::ostream& ost) const
    { obj->print(ost, info); }

    const cptr_type_t* obj;
    const print_info_t info;
};

template<>
struct print_t<const arg_def_t*>
{
    print_t(const arg_def_t* _obj, const print_info_t& _info) :
        obj(_obj), info(_info)
    {}

    void print(std::ostream& ost) const
    { obj->print(ost, info); }

    const arg_def_t* obj;
    const print_info_t info;
};

template<>
struct print_t<obj_type_t>
{
    print_t(const obj_type_t& _obj, const print_info_t& _info) :
        obj(_obj), info(_info)
    {}

    void print(std::ostream& ost) const
    { obj.print(ost, info); }

    const obj_type_t& obj;
    const print_info_t info;
};

template<>
struct print_t<obj_value_t>
{
    print_t(const obj_value_t& _obj, const print_info_t& _info) :
        obj(_obj), info(_info)
    {}

    void print(std::ostream& ost) const
    { obj.print(ost, info); }

    const obj_value_t& obj;
    const print_info_t info;
};

template<typename S, typename T, bool A>
struct print_t<obj_base_t<S, T, A>>
{
    print_t(const obj_base_t<S, T, A>& _obj, const print_info_t& _info) :
        obj(_obj), info(_info)
    {}

    void print(std::ostream& ost) const
    { obj.print(ost, info); }

    const obj_base_t<S, T, A>& obj;
    const print_info_t info;
};

template<typename T>
struct print_t<obj_t<T>>
{
    print_t(const obj_t<T>& _obj, const print_info_t& _info) :
        obj(_obj), info(_info)
    {}

    void print(std::ostream& ost) const
    { obj.print(ost, info); }

    const obj_t<T>& obj;
    const print_info_t info;
};

template<typename T>
struct print_t<const T*>
{
    print_t(const T* _obj, const print_info_t& _info UNUSED) :
        obj(_obj)
    {}

    void print(std::ostream& ost) const
    {
        if (obj) {
            using namespace std;
            auto w = ost.width();
            auto c = ost.fill();
            auto f = ost.flags();
            ost << "Ptr('0x" << right << hex << setfill('0')
                << setw(UINTPTR_FMT_WIDTH)
                << Ext::ptr_to_int(obj)
                << setw(w) << setfill(c) << "')";
            ost.flags(f);
        }
        else
            ost << "Null()";
    }

    const T* obj;
};

template<>
struct print_t<const char*>
{
    print_t(const char* _obj, const print_info_t& _info UNUSED) :
        obj(_obj)
    {}

    void print(std::ostream& ost) const
    {
        if (obj)
            ost << '\'' << obj << '\'';
        else
            ost << "Null()";
    }

    const char* obj;
};

template<>
struct print_t<size_t>
{
    print_t(size_t _obj, const print_info_t& _info UNUSED) :
        obj(_obj)
    {}

    void print(std::ostream& ost) const
    { ost << "Size('" << obj << "\')"; }

    size_t obj;
};

template<typename T>
inline print_t<T> print(const T& obj, const print_info_t& info)
{ return print_t<T>(obj, info); }

template<typename T>
inline std::ostream& operator<<(std::ostream& ost, const print_t<T>& obj)
{ obj.print(ost); return ost; }

} // namespace Obj

#endif /* __EVAL_HPP */

