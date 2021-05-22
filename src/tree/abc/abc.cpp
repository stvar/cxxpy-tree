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

#include <cstring>

#include <iostream>

#include "tree/tree.hpp"
#include "tree/opts.hpp"
#include "tree/ast.hpp"
#include "tree/eval.hpp"

#include "ast.hpp"

const char program[] = "abc";
const char verdate[] = "0.2 -- 2014-11-21 23:16"; // $ date +'%F %R'

static const char extension[] = "abc";

#include "tree/eval-impl.hpp"

namespace Obj {

//
// cptr-type: bool
//
template<>
const cptr_type_t cptr_def_t<bool>::type = cptr_type_t("bool");

template<>
bool cptr_def_t<bool>::cast(const char* arg)
{
    if (strcmp(arg, "0") && strcmp(arg, "1"))
        invalid_value(arg);
    return arg[0] == '1';
}

ast_type_t const* const ast_type_t::types[] = {
// > AST_TYPE_CALLS $ grammar -NAC
    ast_type<AST::Node>(),
    ast_type<AST::A>(),
    ast_type<AST::B>(),
    ast_type<AST::C>(),
    ast_type<AST::D>(),
    ast_type<AST::N>(),
    ast_type<AST::M>(),
    ast_type<AST::P>(),
    ast_type<AST::Q>(),
    ast_type<AST::R>(),
    ast_type<AST::L>(),
    ast_type<AST::S>()
// < AST_TYPE_CALLS
};

const size_t ast_type_t::n_types = Ext::array_size(ast_type_t::types);

// > AST_TYPE_DEFS $ grammar2 -AD
//
// ast-type: Node
//
template<>
const ast_type_t ast_def_t<AST::Node>::type = ast_type_t(
    "Node", nullptr, ast_def_t<AST::Node>::args, ast_def_t<AST::Node>::narg);

template<>
const arg_def_t ast_def_t<AST::Node>::args[] = {};

template<>
const size_t ast_def_t<AST::Node>::narg = 0;

//
// ast-type: A
//
template<>
const ast_type_t ast_def_t<AST::A>::type = ast_type_t(
    "A", ast_type<AST::Node>(), ast_def_t<AST::A>::args, ast_def_t<AST::A>::narg);

template<>
const arg_def_t ast_def_t<AST::A>::args[] = {};

template<>
const size_t ast_def_t<AST::A>::narg = 0;

//
// ast-type: B
//
template<>
const ast_type_t ast_def_t<AST::B>::type = ast_type_t(
    "B", ast_type<AST::Node>(), ast_def_t<AST::B>::args, ast_def_t<AST::B>::narg);

template<>
const arg_def_t ast_def_t<AST::B>::args[] = {
    node<AST::A>("a")
};

template<>
const size_t ast_def_t<AST::B>::narg = 1;

//
// ast-type: C
//
template<>
const ast_type_t ast_def_t<AST::C>::type = ast_type_t(
    "C", ast_type<AST::Node>(), ast_def_t<AST::C>::args, ast_def_t<AST::C>::narg);

template<>
const arg_def_t ast_def_t<AST::C>::args[] = {
    list<AST::B>("b")
};

template<>
const size_t ast_def_t<AST::C>::narg = 1;

//
// ast-type: D
//
template<>
const ast_type_t ast_def_t<AST::D>::type = ast_type_t(
    "D", ast_type<AST::Node>(), ast_def_t<AST::D>::args, ast_def_t<AST::D>::narg);

template<>
const arg_def_t ast_def_t<AST::D>::args[] = {
    cptr<const char*>("c")
};

template<>
const size_t ast_def_t<AST::D>::narg = 1;

//
// ast-type: N
//
template<>
const ast_type_t ast_def_t<AST::N>::type = ast_type_t(
    "N", ast_type<AST::Node>(), ast_def_t<AST::N>::args, ast_def_t<AST::N>::narg);

template<>
const arg_def_t ast_def_t<AST::N>::args[] = {
    node<AST::B>("x"),
    node<AST::C>("y"),
    node<AST::D>("z")
};

template<>
const size_t ast_def_t<AST::N>::narg = 3;

//
// ast-type: M
//
template<>
const ast_type_t ast_def_t<AST::M>::type = ast_type_t(
    "M", ast_type<AST::Node>(), ast_def_t<AST::M>::args, ast_def_t<AST::M>::narg);

template<>
const arg_def_t ast_def_t<AST::M>::args[] = {
    node<AST::A>("a"),
    list<AST::B>("b"),
    cptr<const char*>("c")
};

template<>
const size_t ast_def_t<AST::M>::narg = 3;

//
// ast-type: P
//
template<>
const ast_type_t ast_def_t<AST::P>::type = ast_type_t(
    "P", ast_type<AST::Node>(), ast_def_t<AST::P>::args, ast_def_t<AST::P>::narg);

template<>
const arg_def_t ast_def_t<AST::P>::args[] = {
    node<AST::A>("x"),
    node<AST::A>("y")
};

template<>
const size_t ast_def_t<AST::P>::narg = 2;

//
// ast-type: Q
//
template<>
const ast_type_t ast_def_t<AST::Q>::type = ast_type_t(
    "Q", ast_type<AST::A>(), ast_def_t<AST::Q>::args, ast_def_t<AST::Q>::narg);

template<>
const arg_def_t ast_def_t<AST::Q>::args[] = {};

template<>
const size_t ast_def_t<AST::Q>::narg = 0;

//
// ast-type: R
//
template<>
const ast_type_t ast_def_t<AST::R>::type = ast_type_t(
    "R", ast_type<AST::Q>(), ast_def_t<AST::R>::args, ast_def_t<AST::R>::narg);

template<>
const arg_def_t ast_def_t<AST::R>::args[] = {};

template<>
const size_t ast_def_t<AST::R>::narg = 0;

//
// ast-type: L
//
template<>
const ast_type_t ast_def_t<AST::L>::type = ast_type_t(
    "L", ast_type<AST::Node>(), ast_def_t<AST::L>::args, ast_def_t<AST::L>::narg);

template<>
const arg_def_t ast_def_t<AST::L>::args[] = {
    list<AST::Q>("q")
};

template<>
const size_t ast_def_t<AST::L>::narg = 1;

//
// ast-type: S
//
template<>
const ast_type_t ast_def_t<AST::S>::type = ast_type_t(
    "S", ast_type<AST::Q>(), ast_def_t<AST::S>::args, ast_def_t<AST::S>::narg);

template<>
const arg_def_t ast_def_t<AST::S>::args[] = {
    cptr<bool>("b")
};

template<>
const size_t ast_def_t<AST::S>::narg = 1;
// < AST_TYPE_DEFS

const TypeMaker::new_funcs_t TypeMaker::new_funcs[] = {
// > AST_TYPEMAKER_NEW_FUNCS $ grammar -NAT=4
    new_funcs_t("A",    &TypeMaker::new_node<AST::A>,    &TypeMaker::new_list<AST::A>),
    new_funcs_t("B",    &TypeMaker::new_node<AST::B>,    &TypeMaker::new_list<AST::B>),
    new_funcs_t("C",    &TypeMaker::new_node<AST::C>,    &TypeMaker::new_list<AST::C>),
    new_funcs_t("D",    &TypeMaker::new_node<AST::D>,    &TypeMaker::new_list<AST::D>),
    new_funcs_t("L",    &TypeMaker::new_node<AST::L>,    &TypeMaker::new_list<AST::L>),
    new_funcs_t("M",    &TypeMaker::new_node<AST::M>,    &TypeMaker::new_list<AST::M>),
    new_funcs_t("N",    &TypeMaker::new_node<AST::N>,    &TypeMaker::new_list<AST::N>),
    new_funcs_t("Node", &TypeMaker::new_node<AST::Node>, &TypeMaker::new_list<AST::Node>),
    new_funcs_t("P",    &TypeMaker::new_node<AST::P>,    &TypeMaker::new_list<AST::P>),
    new_funcs_t("Q",    &TypeMaker::new_node<AST::Q>,    &TypeMaker::new_list<AST::Q>),
    new_funcs_t("R",    &TypeMaker::new_node<AST::R>,    &TypeMaker::new_list<AST::R>),
    new_funcs_t("S",    &TypeMaker::new_node<AST::S>,    &TypeMaker::new_list<AST::S>)
// < AST_TYPEMAKER_NEW_FUNCS
};

const size_t TypeMaker::n_new_func = Ext::array_size(TypeMaker::new_funcs);

// > AST_TYPE_NODE_MAKERS $ grammar2 -AM
template<>
obj_value_t NodeMaker::new_node<AST::Node>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Node>(node, args);
    return node_obj_value(type, ast->new_node());
}

template<>
obj_value_t NodeMaker::new_node<AST::A>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::A>(node, args);
    return node_obj_value(type, ast->new_a());
}

template<>
obj_value_t NodeMaker::new_node<AST::B>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::B>(node, args);
    return node_obj_value(type, ast->new_b(
        node_arg<AST::A>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::C>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::C>(node, args);
    return node_obj_value(type, ast->new_c(
        list_arg<AST::B>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::D>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::D>(node, args);
    return node_obj_value(type, ast->new_d(
        cptr_arg<const char*>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::N>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::N>(node, args);
    return node_obj_value(type, ast->new_n(
        node_arg<AST::B>(0, type, args),
        node_arg<AST::C>(1, type, args),
        node_arg<AST::D>(2, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::M>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::M>(node, args);
    return node_obj_value(type, ast->new_m(
        node_arg<AST::A>(0, type, args),
        list_arg<AST::B>(1, type, args),
        cptr_arg<const char*>(2, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::P>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::P>(node, args);
    return node_obj_value(type, ast->new_p(
        node_arg<AST::A>(0, type, args),
        node_arg<AST::A>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::Q>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Q>(node, args);
    return node_obj_value(type, ast->new_q());
}

template<>
obj_value_t NodeMaker::new_node<AST::R>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::R>(node, args);
    return node_obj_value(type, ast->new_r());
}

template<>
obj_value_t NodeMaker::new_node<AST::L>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::L>(node, args);
    return node_obj_value(type, ast->new_l(
        list_arg<AST::Q>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::S>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::S>(node, args);
    return node_obj_value(type, ast->new_s(
        cptr_arg<bool>(0, type, args)));
}
// < AST_TYPE_NODE_MAKERS

const NodeMaker::new_funcs_t NodeMaker::new_funcs[] = {
// > AST_NODEMAKER_NEW_FUNCS $ grammar -NAN=4
    new_funcs_t("A",    &NodeMaker::new_node<AST::A>,    &NodeMaker::new_list<AST::A>),
    new_funcs_t("B",    &NodeMaker::new_node<AST::B>,    &NodeMaker::new_list<AST::B>),
    new_funcs_t("C",    &NodeMaker::new_node<AST::C>,    &NodeMaker::new_list<AST::C>),
    new_funcs_t("D",    &NodeMaker::new_node<AST::D>,    &NodeMaker::new_list<AST::D>),
    new_funcs_t("L",    &NodeMaker::new_node<AST::L>,    &NodeMaker::new_list<AST::L>),
    new_funcs_t("M",    &NodeMaker::new_node<AST::M>,    &NodeMaker::new_list<AST::M>),
    new_funcs_t("N",    &NodeMaker::new_node<AST::N>,    &NodeMaker::new_list<AST::N>),
    new_funcs_t("Node", &NodeMaker::new_node<AST::Node>, &NodeMaker::new_list<AST::Node>),
    new_funcs_t("P",    &NodeMaker::new_node<AST::P>,    &NodeMaker::new_list<AST::P>),
    new_funcs_t("Q",    &NodeMaker::new_node<AST::Q>,    &NodeMaker::new_list<AST::Q>),
    new_funcs_t("R",    &NodeMaker::new_node<AST::R>,    &NodeMaker::new_list<AST::R>),
    new_funcs_t("S",    &NodeMaker::new_node<AST::S>,    &NodeMaker::new_list<AST::S>)
// < AST_NODEMAKER_NEW_FUNCS
};

const size_t NodeMaker::n_new_func = Ext::array_size(NodeMaker::new_funcs);

} // namespace Obj

EXT_FUNC_ENTRY
Sys::ext_func_result_t abc(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    return ext_func_entry(ast, opts);
}


