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

#include <cctype>
#include <cstring>

#include <deque>
#include <functional>
#include <algorithm>

#include <sstream>

#include "ast.hpp"
#include "eval.hpp"

#include "std-ext.hpp"

namespace Obj {

namespace AST = TreeAST;

ObjError::ObjError(const char* msg) :
    std::runtime_error(msg)
{}

ObjError::ObjError(const std::string& msg) :
    std::runtime_error(msg)
{}

void error(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    auto err = Sys::vformat(msg, args);
    va_end(args);

    throw ObjError(err);
}

static const char stack_name[] = "AST expression stack";

template<typename T>
class Evaluator : private AST::Visitor
{
public:
    typedef T val_t;
    typedef Obj::obj_t<val_t> obj_t;
    typedef ObjMaker<val_t> obj_maker_t;

    Evaluator(obj_maker_t* _maker, const eval_opts_t& _opt) :
        maker(_maker),
        opt(_opt)
    {}

    val_t eval(const AST::Tree* tree);

private:
    void not_yet_impl(const char* func);

    template<typename V>
    void dump(const char* where, const V&) const;

    void eval(const AST::Node* node);
    template<typename L, typename E, typename V, const V* E::*mem>
    void eval(const L* list);

    void abstract(const AST::Node* node);
    void unexpected(const AST::Node* node);

    void visit(const AST::Node*); //!!!???
// > EVALUATOR_VISITOR_DECL $ grammar -aVD
    void visit(const AST::Literal*);
    void visit(const AST::ListElem*);
    void visit(const AST::NodeName*);
    void visit(const AST::NodeList*);
    void visit(const AST::ArgName*);
    void visit(const AST::ArgValue*);
    void visit(const AST::NodeValue*);
    void visit(const AST::ListValue*);
    void visit(const AST::LiteralValue*);
    void visit(const AST::NodeArg*);
    void visit(const AST::NodeArgs*);
    void visit(const AST::ListArgs*);
    void visit(const AST::LiteralArgs*);
    void visit(const AST::TreeNode*);
    void visit(const AST::Tree*);
// < EVALUATOR_VISITOR_DECL

    struct obj_stack_t :
        public Obj::obj_stack_t<val_t>,
        private std::deque<obj_t>
    {
        typedef std::deque<obj_t> base_t;

        obj_stack_t() {}

        void check_not_empty() const;
        void check_range(size_t k) const;
        void check_size(size_t n) const;

        bool empty() const { return base_t::empty(); }
        size_t size() const { return base_t::size(); }

        const obj_t& operator[](size_t k) const;

        obj_t& top();
        const obj_t& top() const;

        void pop(size_t n);

        void push(const obj_t& obj);
    };

    obj_maker_t *maker;
    obj_stack_t  stack;
    eval_opts_t  opt;
};

struct where_print_t
{
    where_print_t(const char* _where) :
        where(_where)
    {}
    void print(std::ostream&) const;
    const char* where;
};

void where_print_t::print(std::ostream&) const
{
    using namespace std;
    static const char visit[] = "::visit(";
    auto end = where + strlen(where);
    auto ptr = search(where, end, visit, visit + 8);
    auto ptr2 = find_if(ptr, end, [](char c) { return c == ')'; });
    if (ptr != end)
        copy(
            ptr != end ? ptr + 2 : where,
            ptr2 != end ? ptr2 + 1 : end,
            ostream_iterator<char>(cout));
    else
        cout << where;
}

static inline where_print_t where_print(const char* where)
{ return where_print_t(where); }

static std::ostream& operator<<(std::ostream& ost, const where_print_t& obj)
{ obj.print(ost); return ost; }

template<typename T>
inline void Evaluator<T>::obj_stack_t::check_not_empty() const
{
    if (empty())
        error("%s is empty", stack_name);
}

template<typename T>
inline void Evaluator<T>::obj_stack_t::check_range(size_t k) const
{
    if (k >= size())
        error("%s index '%zu' out of range", stack_name, k);
}

template<typename T>
inline void Evaluator<T>::obj_stack_t::check_size(size_t n) const
{
    if (n <= 0 || n > size())
        error("%s size '%zu' out of range", stack_name, n);
}

template<typename T>
typename Evaluator<T>::obj_t& Evaluator<T>::obj_stack_t::top()
{
    check_not_empty();
    return base_t::front();
}

template<typename T>
const typename Evaluator<T>::obj_t& Evaluator<T>::obj_stack_t::top() const
{
    check_not_empty();
    return base_t::front();
}

template<typename T>
const typename Evaluator<T>::obj_t& Evaluator<T>::obj_stack_t::operator[](size_t k) const
{
    check_range(k);
    return base_t::at(k);
}

template<typename T>
void Evaluator<T>::obj_stack_t::pop(size_t n)
{
    check_size(n);
    auto beg = base_t::begin();
    base_t::erase(beg, beg + n);
}

template<typename T>
void Evaluator<T>::obj_stack_t::push(const obj_t& obj)
{
    base_t::push_front(obj);
}

template<typename T>
typename Evaluator<T>::val_t Evaluator<T>::eval(const AST::Tree* tree)
{
    eval(static_cast<const AST::Node*>(tree));
    if (stack.size() != 1)
        error("invalid %s: size is %zu", stack_name, stack.size());
    auto top = stack.top();
    if (!top.value.template is<obj_type_t::node_t>())
        error("top of %s is not of type 'node'", stack_name);
    return top.value;
}

template<typename T>
template<typename L, typename E, typename V, const V* E::*mem>
void Evaluator<T>::eval(const L* list)
{
    size_t k = 0;
    auto ptr = list->elems.second;
    auto end = ptr + list->elems.first;
    for (; ptr != end; ++ ptr, ++ k) { //!!! use std::for_each
        auto elem = *ptr;
        if (elem->index != k)
            error("invalid list index '%u' -- expected '%zu'", elem->index, k);
        eval(elem->*mem);
    }
}

template<typename T>
void Evaluator<T>::not_yet_impl(const char* func)
{
    error("function '%s' not yet implemented", func);
}

template<typename T>
inline void Evaluator<T>::eval(const AST::Node* node)
{
    node->accept(this);
}

template<typename T>
void Evaluator<T>::abstract(const AST::Node* node)
{
    error("cannot visit abstract node '%s'", node->name());
}

template<typename T>
void Evaluator<T>::unexpected(const AST::Node* node)
{
    error("unexpected visit to node '%s'", node->name());
}

template<typename T>
void Evaluator<T>::visit(const AST::Node* arg) //!!!???
{
    abstract(arg);
}

// stev: TODO: the grammar shell function is not allowing a classname to include
//             template parameters as bellow; maybe it should define a new command
//             line option allowing to specify 'T' as an argument and then generating
//             properly something like: 'template<typename T> ... class-name<T>::...'.
//!!! // > EVALUATOR_VISITOR_IMPL $ grammar -aVI=Evaluator<T>

// > EVALUATOR_VISITOR_IMPL $ grammar -aVI=Evaluator
template<typename T>
void Evaluator<T>::visit(const AST::Literal* arg)
{
    obj_t obj = maker->make_cptr(arg);
    if (opt.trace_eval)
        dump(__PRETTY_FUNCTION__, obj.value);
    stack.push(obj);
}

template<typename T>
void Evaluator<T>::visit(const AST::ListElem* arg)
{
    unexpected(arg);
}

template<typename T>
void Evaluator<T>::visit(const AST::NodeName* arg)
{
    unexpected(arg);
}

template<typename T>
void Evaluator<T>::visit(const AST::NodeList* arg)
{
    eval<
        AST::NodeList,
        AST::ListElem,
        AST::TreeNode,
        &AST::ListElem::node>(
        arg);
    obj_t obj = maker->make_list(arg, stack);
    if (opt.trace_eval)
        dump(__PRETTY_FUNCTION__, obj.value);
    stack.push(obj);
}

template<typename T>
void Evaluator<T>::visit(const AST::ArgName* arg)
{
    auto& obj = stack.top();
    if (obj.name)
        error("invalid %s: obj name is not null: '%s'", stack_name, obj.name);
    obj.name = arg->str;
}

template<typename T>
void Evaluator<T>::visit(const AST::ArgValue* arg)
{
    abstract(arg);
}

template<typename T>
void Evaluator<T>::visit(const AST::NodeValue* arg)
{
    eval(arg->node);
}

template<typename T>
void Evaluator<T>::visit(const AST::ListValue* arg)
{
    eval(arg->list);
}

template<typename T>
void Evaluator<T>::visit(const AST::LiteralValue* arg)
{
    eval(arg->literal);
}

template<typename T>
void Evaluator<T>::visit(const AST::NodeArg* arg)
{
    eval(arg->value);
    eval(arg->name);
}

template<typename T>
void Evaluator<T>::visit(const AST::NodeArgs* arg)
{
    abstract(arg);
}

template<typename T>
void Evaluator<T>::visit(const AST::ListArgs* arg)
{
    using namespace std;
    auto beg = arg->list.second;
    auto end = beg + arg->list.first;
    for_each(beg, end, [this](const AST::Node* node)
        { eval(node); });
}

template<typename T>
void Evaluator<T>::visit(const AST::LiteralArgs* arg)
{
    eval(arg->literal);
}

template<typename T>
void Evaluator<T>::visit(const AST::TreeNode* arg)
{
    eval(arg->args);
    obj_t obj = maker->make_node(arg, stack);
    if (opt.trace_eval)
        dump(__PRETTY_FUNCTION__, obj.value);
    stack.push(obj);
}

template<typename T>
void Evaluator<T>::visit(const AST::Tree* arg)
{
    eval(arg->root);
}
// < EVALUATOR_VISITOR_IMPL

template<typename T>
template<typename V>
void Evaluator<T>::dump(const char* where, const V& val) const
{
    using namespace std;
    cout << where_print(where) << ": " << print(val, opt) << endl;
}

template<typename T>
void TypeCheck<T>::check_node_arg_name(
    const ast_type_t* type, const arg_def_t* def, size_t k, const obj_t& obj) const
{
    if (strcmp(def->arg_name, obj.name))
        error("ast-type %s: arg #%zu: expected name '%s', but got '%s'",
            type->name, k, def->arg_name, obj.name);
}

template<typename T>
void TypeCheck<T>::check_node_arg_value(
    const ast_type_t* type, const arg_def_t* def, size_t k, const obj_t& obj) const
{
    if (!def->arg_type.type_cast(obj.value, info))
        error("ast-type %s: arg #%zu: cannot cast from type '%s' to '%s'",
            type->name, k, obj.value.type_name().c_str(), def->arg_type.type_name().c_str());
}

template<typename T>
void TypeCheck<T>::check_node_arg(
    const ast_type_t* type, const arg_def_t* def, size_t k, const obj_t& obj) const
{
    check_node_arg_name(type, def, k, obj);
    check_node_arg_value(type, def, k, obj);
}

template<typename T>
void TypeCheck<T>::check_node_args(
    const ast_type_t* type, const AST::TreeNode* node, obj_stack_t& args)
{
    using namespace AST;

    if (node_cast<LiteralArgs>(node->args)) {
        if (type->narg != 1) {
            narg_error = true;
            error("ast-type %s: expected %zu args, but got one",
                type->name, type->narg);
        }
        check_node_arg_value(type, type->args, 1, args[0]);
    }
    else
    if (const ListArgs* list_args = node_cast<ListArgs>(node->args)) {
        if (type->narg != list_args->list.first) {
            narg_error = true;
            error("ast-type %s: expected %zu args, but got %zu",
                type->name, type->narg, list_args->list.first);
        }
        size_t k = type->narg - 1;
        auto ptr = type->args;
        auto end = ptr + type->narg;
        for (; ptr != end; ++ ptr, -- k)
            check_node_arg(type, ptr, type->narg - k, args[k]);
    }
    else
        SYS_UNEXPECT_ERR("node args instance is not literal nor list");
}

template<typename T>
void TypeCheck<T>::check_list_arg(
    const ast_type_t* type, size_t k, const obj_t& obj) const
{
    if (!type->list_type_cast(obj.value, info))
        error("list<%s>: arg #%zu: cannot cast from type '%s'",
            type->name, k, obj.value.type_name().c_str());
}

template<typename T>
void TypeCheck<T>::check_list_args(
    const ast_type_t* type, const AST::NodeList* list, obj_stack_t& args) const
{
    size_t n = list->elems.first;
    for (auto k = n; k; k --)
        check_list_arg(type, n - k, args[k - 1]);
}

template<typename T>
void TypeCheck<T>::pop_args(size_t n_arg, obj_stack_t& args)
{
    if (!narg_error) {
        if (n_arg > 0) args.pop(n_arg);
    }
    else
        narg_error = false;
}

// stev: shared library template instance: TypeCheck<obj_type_t>

template
void TypeCheck<obj_type_t>::check_node_args(
    const ast_type_t*, const TreeAST::TreeNode*, obj_stack_t&);

template
void TypeCheck<obj_type_t>::check_list_args(
    const ast_type_t*, const TreeAST::NodeList*, obj_stack_t&) const;

template
void TypeCheck<obj_type_t>::pop_args(
    size_t n_arg, obj_stack_t& args);

// stev: shared library template instance: TypeCheck<obj_value_t>

template
void TypeCheck<obj_value_t>::check_node_args(
    const ast_type_t*, const TreeAST::TreeNode*, obj_stack_t&);

template
void TypeCheck<obj_value_t>::check_list_args(
    const ast_type_t*, const TreeAST::NodeList*, obj_stack_t&) const;

template
void TypeCheck<obj_value_t>::pop_args(
    size_t n_arg, obj_stack_t& args);

// stev: shared library template instance: ObjMaker<obj_type_t>

template
ObjMaker<obj_type_t>::~ObjMaker();

obj_type_t eval(
    const AST::Tree* tree, Obj::ObjMaker<obj_type_t>* maker, const eval_opts_t& opt)
{
    return Obj::Evaluator<obj_type_t>(maker, opt).eval(tree);
}

// stev: shared library template instance: ObjMaker<obj_value_t>

template
ObjMaker<obj_value_t>::~ObjMaker();

obj_value_t eval(
    const AST::Tree* tree, Obj::ObjMaker<obj_value_t>* maker, const eval_opts_t& opt)
{
    return Obj::Evaluator<obj_value_t>(maker, opt).eval(tree);
}

char const* const obj_consts_t::types[3] = {
    "node", // node_obj
    "list", // list_obj
    "cptr", // cptr_obj
};

static inline std::string list_type_name(const char* name)
{
    return std::string("list<").append(name).append(">");
}

std::string obj_type_t::type_name() const
{
    if (auto node = cast<node_t>())
        return node->ast_type->name;
    if (auto cptr = cast<cptr_t>())
        return cptr->cptr_type->name;
    if (auto list = cast<list_t>())
        return list_type_name(list->ast_type->name);
    else {
        SYS_UNEXPECT_ERR("type='%d'", type);
        return std::string(); // make the compiler happy
    }
}

bool ast_type_t::type_cast(const ast_type_t* src, const type_cast_info_t& info) const
{
    if (info.strict)
        return src == this;
    auto ptr = src;
    do {
        if (ptr == this)
            return true;
        if (ptr->base == nullptr)
            return false;
        ptr = ptr->base;
    }
    while (ptr != src);
    SYS_UNEXPECT_ERR(
        "cycle in ast-type: src='" UINTPTR_FORMAT "'",
        Ext::ptr_to_int(src));
    return false; // make the compiler happy
}

bool ast_type_t::list_type_cast(const obj_type_t& src, const type_cast_info_t& info) const
{
    if (auto node = src.cast<obj_type_t::node_t>())
        return type_cast(node->ast_type, info);
    else
        return false;
}

bool cptr_type_t::list_type_cast(const obj_type_t& src, const type_cast_info_t& info UNUSED) const
{
    if (auto cptr = src.cast<obj_type_t::cptr_t>())
        return cptr->cptr_type == this;
    else
        return false;
}

bool obj_type_t::type_cast(const obj_type_t& src, const type_cast_info_t& info) const
{
    if (type != src.type)
        return false;
    if (auto node = cast<node_t>())
        return node->ast_type->type_cast(src.as<node_t>().ast_type, info);
    if (auto list = cast<list_t>())
        return list->ast_type->type_cast(src.as<list_t>().ast_type, info);
    if (is<cptr_t>())
        return true;
    else {
        SYS_UNEXPECT_ERR("type='%d'", type);
        return false; // make the compiler happy
    }
}

struct type_name_print_t
{
    type_name_print_t(const char* _name) :
        name(_name)
    {}
    void print(std::ostream& ost) const;
    const char* name;
};

inline type_name_print_t type_name_print(const char* name)
{ return type_name_print_t(name); }

inline std::ostream& operator<<(std::ostream& ost, const type_name_print_t& obj)
{ obj.print(ost); return ost; }

void type_name_print_t::print(std::ostream& ost) const
{
    size_t n = strlen(name);
    auto ptr = name;
    auto end = ptr + n;
    if (n > 2 && strncmp(end - 2, "_t", 2) == 0)
        end -= 2;
    if (n > 0)
        ost << Ext::integer_cast<char>(toupper(*ptr ++));
    auto skip = false;
    for (; ptr != end; ++ ptr) {
        if (*ptr == '_') {
            skip = true;
            continue;
        }
        if (skip)
            ost << Ext::integer_cast<char>(toupper(*ptr));
        else
            ost << *ptr;
        skip = false;
    }
}

struct obj_head_print_t
{
    obj_head_print_t(
        const char* _name,
        obj_type_t::type_t _type,
        bool _is_type,
        const print_info_t& _info) :
        name(_name),
        type(_type),
        is_type(_is_type),
        info(_info)
    {}
    void print(std::ostream& ost) const;

    const char* name;
    obj_type_t::type_t type;
    bool is_type;
    const print_info_t& info;
};

inline obj_head_print_t obj_head_print(
    const char* name, obj_type_t::type_t type, bool is_type,
    const print_info_t& info)
{ return obj_head_print_t(name, type, is_type, info); }

inline std::ostream& operator<<(
    std::ostream& ost, const obj_head_print_t& obj)
{ obj.print(ost); return ost; }

void obj_head_print_t::print(std::ostream& ost) const
{
    auto name = Ext::array(obj_type_t::types)[type];
    ost << Obj::type_name_print(this->name)
        << "(.type = " << Obj::print(name, info)
        << " ." << name << " = " << Obj::type_name_print(name);
    if (is_type)
        ost << "ObjType";
    else
        ost << "ObjValue";
}

void obj_type_t::print(std::ostream& ost, const print_info_t& info) const
{
    ost << Obj::obj_head_print("obj_type_t", type, true, info) << '(';
    if (const node_t* node = cast<node_t>())
        ost << ".ast_type = " << Obj::print(node->ast_type, info);
    else
    if (const list_t* list = cast<list_t>())
        ost << ".ast_type = " << Obj::print(list->ast_type, info);
    else
    if (const cptr_t* cptr = cast<cptr_t>())
        ost << ".cptr_type = " << Obj::print(cptr->cptr_type, info);
    else
        SYS_UNEXPECT_ERR("type = '%d'", type);
    ost << "))";
}

void arg_def_t::print(std::ostream& ost, const print_info_t& info) const
{
    ost << Obj::type_name_print("arg_def_t")
        << "(.arg_name = " << Obj::print(arg_name, info)
        << " .arg_type = " << Obj::print(arg_type, info)
        << ')';
}

void ast_type_t::print(std::ostream& ost, const print_info_t& info) const
{
    if (info.verbose) {
        ost << "Node"
            << Obj::type_name_print("ast_type_t")
            << "(.name = " << Obj::print(name, info)
            << " .base = ";
        if (base)
            ost << Obj::print(base, info);
        else
            ost << "NullAstType()";
        ost << " .args = list<" << Obj::type_name_print("arg_def_t")
            << "> [";
        size_t n = 0;
        const arg_def_t* ptr = args;
        const arg_def_t* end = ptr + narg;
        for (; ptr != end; ++ ptr, ++ n) {
            if (n) ost << ' ';
            ost << '[' << n << "] = " << Obj::print(ptr, info);
        }
        ost << "])";
    }
    else {
        ost << Obj::print(name, info);
    }
}

void cptr_type_t::print(std::ostream& ost, const print_info_t& info) const
{
    if (info.verbose)
        ost << Obj::type_name_print("cptr_type_t")
            << '(' << Obj::print(name, info) << ')';
    else
        ost << Obj::print(name, info);
}

void obj_value_t::print(std::ostream& ost, const print_info_t& info) const
{
    ost << Obj::obj_head_print("obj_value_t", as<type_t>(), false, info) << '(';
    if (auto node = cast<node_t>())
        ost << ".ast_type = " << Obj::print(as<obj_type_t::node_t>().ast_type, info)
            << " .ptr = " << Obj::print(node->ptr, info);
    else
    if (auto list = cast<list_t>())
        ost << ".ast_type = " << Obj::print(as<obj_type_t::list_t>().ast_type, info)
            << " .size = " << Obj::print(list->size, info)
            << " .ptr = " << Obj::print(list->ptr, info);
    else
    if (auto cptr = cast<cptr_t>())
        ost << ".cptr_type = " << Obj::print(as<obj_type_t::cptr_t>().cptr_type, info)
            << " .ptr = " << Obj::print(cptr->ptr, info);
    else
        SYS_UNEXPECT_ERR("type = '%d'", as<type_t>());
    ost << "))";
}

template<>
void obj_t<obj_type_t>::print(std::ostream& ost, const print_info_t& info) const
{
    ost << "TypeObj"
        << "(.name = " << Obj::print(name, info)
        << " .value = " << Obj::print(value, info)
        << ')';
}

template<>
void obj_t<obj_value_t>::print(std::ostream& ost, const print_info_t& info) const
{
    ost << "ValueObj"
        << "(.name = " << Obj::print(name, info)
        << " .value = " << Obj::print(value, info)
        << ')';
}

template<>
const cptr_type_t cptr_def_t<const char*>::type = cptr_type_t("char*");

template<>
const char* cptr_def_t<const char*>::cast(const char* arg)
{ return arg; }

void ast_type_t::print_types(std::ostream& ost)
{
    auto ptr = ast_type_t::types;
    auto end = ptr + ast_type_t::n_types;
    for (; ptr != end; ptr ++) {
        auto type = *ptr;
        ost << '<' << type->name << '>';
        if (type->base) {
            ost << " : " << type->base->name;
            if (type->narg > 0)
                ost << " @ ";
            auto beg = type->args;
            auto ptr = beg;
            auto end = beg + type->narg;
            for (; ptr != end; ptr ++) {
                if (ptr != beg)
                    ost << ", ";
                if (auto node = ptr->arg_type.cast<obj_type_t::node_t>())
                    ost << '<' << node->ast_type->name << ">*";
                else
                if (auto list = ptr->arg_type.cast<obj_type_t::list_t>())
                    ost << "list<" << list->ast_type->name << '>';
                else
                if (auto cptr = ptr->arg_type.cast<obj_type_t::cptr_t>())
                    ost << '<' << cptr->cptr_type->name << '>';
                else
                    SYS_UNEXPECT_ERR(
                        "ptr->arg_type.type = '%d'",
                        ptr->arg_type.as<obj_type_t::type_t>());
                ost << " [" << ptr->arg_name << ']';
            }
        }
        ost << ';' << std::endl;
    }
}

} // namespace Obj


