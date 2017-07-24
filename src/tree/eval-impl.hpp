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

#ifndef __EVAL_IMPL_HPP
#define __EVAL_IMPL_HPP

#include "sys.hpp"
#include "eval.hpp"
#include "opts.hpp"

#include "ext-err-impl.hpp"

namespace Obj {

class TypeMaker :
    public BasicMaker<TypeMaker, obj_type_t>
{
public:
    typedef BasicMaker<TypeMaker, obj_type_t> base_t;

    TypeMaker(const type_cast_info_t& _info) :
        base_t(_info)
    {}

private:
    friend class BasicMaker<TypeMaker, obj_type_t>;

    template<typename T>
    obj_type_t new_node(const TreeAST::TreeNode* node, obj_stack_t& args);

    template<typename T>
    obj_type_t new_list(const TreeAST::NodeList* list, obj_stack_t& args);

    static const new_funcs_t new_funcs[];
    static const size_t n_new_func;
};

template<typename T>
obj_type_t TypeMaker::new_node(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    check_node<T>(node, args);
    return node_obj_type<T>();
}

template<typename T>
obj_type_t TypeMaker::new_list(
    const TreeAST::NodeList* list, obj_stack_t& args)
{
    check_list<T>(list, args);
    return list_obj_type<T>();
}

class NodeMaker :
    public BasicMaker<NodeMaker, obj_value_t>
{
public:
    typedef BasicMaker<NodeMaker, obj_value_t> base_t;

    NodeMaker(AST::AST* _ast, const type_cast_info_t& _info) :
        base_t(_info),
        ast(_ast)
    {}

private:
    friend class BasicMaker<NodeMaker, obj_value_t>;

    template<typename T>
    obj_value_t new_node(const TreeAST::TreeNode* node, obj_stack_t& args);

    template<typename T>
    obj_value_t new_list(const TreeAST::NodeList* list, obj_stack_t& args);

    template<typename T>
    static const T* node_arg(
        size_t k, const ast_type_t* type, obj_stack_t& args);

    template<typename T>
    static const typename std::pair<size_t, T const* const*> list_arg(
        size_t k, const ast_type_t* type, obj_stack_t& args);

    template<typename T>
    static T cptr_arg(
        size_t k, const ast_type_t* type, obj_stack_t& args);

    template<typename V>
    struct list_elem_cast_t;

    template<typename R, typename V>
    const R* new_list(size_t n_arg, obj_stack_t& args);

    static const new_funcs_t new_funcs[];
    static const size_t n_new_func;

    AST::AST* ast;
};

template<typename T>
inline const T* NodeMaker::node_arg(
    size_t k, const ast_type_t* type, obj_stack_t& args)
{
    return static_cast<const T*>(
        args[type->narg - k - 1].value.as<obj_value_t::node_t>().ptr);
}

template<typename T>
inline const typename std::pair<size_t, T const* const*> NodeMaker::list_arg(
    size_t k, const ast_type_t* type, obj_stack_t& args)
{
    typedef T const* const* ptr_t;
    typedef obj_value_t::list_t val_t;
    typedef std::pair<size_t, ptr_t> list_t;
    auto list = args[type->narg - k - 1].value.as<val_t>();
    return list_t(list.size, reinterpret_cast<ptr_t>(list.ptr));
}

template<typename T>
inline T NodeMaker::cptr_arg(
    size_t k, const ast_type_t* type, obj_stack_t& args)
{
    return cptr_cast<T>(
        args[type->narg - k - 1].value.as<obj_value_t::cptr_t>().ptr);
}

template<>
struct NodeMaker::list_elem_cast_t<obj_value_t::node_t>
{
    typedef obj_value_t::node_t val_t;

    template<typename R>
    static R cast(val_t v)
    { return v.ptr; } // stev: NOP
};

template<>
struct NodeMaker::list_elem_cast_t<obj_value_t::cptr_t>
{
    typedef obj_value_t::cptr_t val_t;

    template<typename R>
    static R cast(val_t v)
    { return cptr_cast<R>(v.ptr); }
};

template<typename R, typename V>
const R* NodeMaker::new_list(size_t n_arg, obj_stack_t& args)
{
    if (n_arg == 0)
        return 0;
    auto ptr = ast->new_list<R>(n_arg);
    auto end = ptr + n_arg;
    for (size_t k = n_arg - 1; ptr != end; ++ ptr, -- k)
        *ptr = list_elem_cast_t<V>::template cast<R>(args[k].value.as<V>());
    return ptr - n_arg;
}

template<typename T>
obj_value_t NodeMaker::new_list(
    const TreeAST::NodeList* list, obj_stack_t& args)
{
    auto type = check_list<T>(list, args);
    return list_obj_value(type, list->elems.first,
        new_list<const void*, obj_value_t::node_t>(list->elems.first, args));
}

inline type_cast_info_t type_cast_info(const options_t* opt)
{
    return type_cast_info_t(
        opt->casts_type == options_t::strict_casts);
}

inline eval_opts_t eval_opts(const options_t* opt)
{
    return eval_opts_t (
        opt->trace_eval,
        opt->verbose);
}

inline print_info_t print_info(const options_t* opt)
{
    return print_info_t(
        opt->verbose);
}

template<typename T>
static Sys::ext_func_result_t exec(
    const TreeAST::Tree* tree, const options_t* opt);

template<>
inline Sys::ext_func_result_t exec<Obj::TypeMaker>(
    const TreeAST::Tree* tree, const options_t* opt)
{
    TypeMaker maker(type_cast_info(opt));
    auto obj = eval<obj_type_t>(tree, &maker, eval_opts(opt));

    std::cout << print(obj, print_info(opt)) << std::endl;

    return 0;
}

template<>
inline Sys::ext_func_result_t exec<Obj::NodeMaker>(
    const TreeAST::Tree* tree, const options_t* opt)
{
    using AST::AST;

    AST ast(
#ifdef DEBUG
        opt->debug
#endif
    );
    NodeMaker maker(&ast, type_cast_info(opt));
    auto obj = eval<obj_value_t>(tree, &maker, eval_opts(opt));

    std::cout << print(obj, print_info(opt)) << std::endl;

    return 0;
}

struct run_ext_t
{};

template<>
inline Sys::ext_func_result_t exec<run_ext_t>(
    const TreeAST::Tree* tree, const options_t* opt)
{
    using AST::Node;
    using AST::AST;

    AST ast(
#ifdef DEBUG
        opt->debug
#endif
    );
    NodeMaker maker(&ast, type_cast_info(opt));
    auto obj = eval<obj_value_t>(tree, &maker, eval_opts(opt));

    return Sys::run_ext_func(
        static_cast<const Node*>(obj.as<obj_value_t::node_t>().ptr), opt);
}

} // namespace Obj

namespace ExtErr {

enum {
    invalid_root_node_type = no_input_given + 1,
};

template<size_t E>
void print_error(const TreeAST::Node* node);

template<>
inline void print_error<invalid_root_node_type>(const TreeAST::Node* node)
{ print_error("invalid root node type: %s", node->name()); }

} // namespace ExtErr

static inline Sys::ext_func_result_t ext_func_entry(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    using namespace Obj;

    auto opt = options_t::options(opts);

    if (opt->action == options_t::print_types_action) {
        ast_type_t::print_types(std::cout);
        return 0;
    }
    if (ast == nullptr || opts->argc == 0) {
        using namespace ExtErr;
        print_error<no_input_given>();
        return 1;
    }

    auto tree = TreeAST::node_cast<TreeAST::Tree>(ast->root_node());
    if (tree == nullptr) {
        using namespace ExtErr;
        print_error<invalid_root_node_type>(ast->root_node());
        return 1;
    }

    static Sys::ext_func_result_t (*const funcs[4])(
        const TreeAST::Tree*, const Obj::options_t*) = {
        &exec<run_ext_t>, // options_t::ext_func_action
        &exec<TypeMaker>, // options_t::type_check_action
        &exec<NodeMaker>, // options_t::ast_build_action
        &exec<run_ext_t>, // options_t::print_obj_action
    };
    return Ext::array(funcs)[
        opt->action - options_t::ext_func_action](tree, opt);
}

#endif /* __EVAL_IMPL_HPP */

