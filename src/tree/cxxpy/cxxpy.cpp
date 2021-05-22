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
#include <algorithm>
#include <functional>

#include "tree/tree.hpp"
#include "tree/opts.hpp"
#include "tree/ast.hpp"
#include "tree/eval.hpp"

#include "std-ext.hpp"
#include "ast.hpp"

const char program[] = "c++py";
const char verdate[] = "0.2 -- 2014-11-21 23:17"; // $ date +'%F %R'

static const char extension[] = "cxxpy";

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

//
// cptr-type: AST::comp_op_t::type_t
//
template<>
const cptr_type_t cptr_def_t<AST::AST::comp_op_t::type_t>::type = cptr_type_t(
    "AST::comp_op_t::type_t");

template<>
AST::AST::comp_op_t::type_t cptr_def_t<AST::AST::comp_op_t::type_t>::cast(
    const char* arg)
{
    size_t type = 0;
    if (!AST::AST::comp_op_t::lookup(arg, type))
        invalid_value(arg);
    return Ext::enum_cast<AST::AST::comp_op_t::type_t>(type);
}

ast_type_t const* const ast_type_t::types[] = {
// > AST_TYPE_CALLS $ grammar -NAC
    ast_type<AST::Node>(),
    ast_type<AST::Ident>(),
    ast_type<AST::Str>(),
    ast_type<AST::Expr>(),
    ast_type<AST::PrimaryExpr>(),
    ast_type<AST::IdentExpr>(),
    ast_type<AST::StrExpr>(),
    ast_type<AST::ParenExpr>(),
    ast_type<AST::DictExpr>(),
    ast_type<AST::EmptyDictExpr>(),
    ast_type<AST::KeyDatumDictExpr>(),
    ast_type<AST::KeyDatum>(),
    ast_type<AST::KeyDatumList>(),
    ast_type<AST::AttrRef>(),
    ast_type<AST::CallArg>(),
    ast_type<AST::CallArgList>(),
    ast_type<AST::CallExpr>(),
    ast_type<AST::EmptyCallExpr>(),
    ast_type<AST::ArgsCallExpr>(),
    ast_type<AST::OrTest>(),
    ast_type<AST::AndTest>(),
    ast_type<AST::NotTest>(),
    ast_type<AST::Comp>(),
    ast_type<AST::CompExpr>(),
    ast_type<AST::Stmt>(),
    ast_type<AST::SimpleStmt>(),
    ast_type<AST::ExprStmt>(),
    ast_type<AST::AssertStmt>(),
    ast_type<AST::AssertStmt1>(),
    ast_type<AST::AssertStmt2>(),
    ast_type<AST::Target>(),
    ast_type<AST::IdentTarget>(),
    ast_type<AST::AttrRefTarget>(),
    ast_type<AST::TargetList>(),
    ast_type<AST::AssignStmt>(),
    ast_type<AST::DelStmt>(),
    ast_type<AST::ExprList>(),
    ast_type<AST::PrintStmt>(),
    ast_type<AST::SimpleStmtList>(),
    ast_type<AST::StmtList>(),
    ast_type<AST::FileInput>(),
    ast_type<AST::InteractInput>(),
    ast_type<AST::EvalInput>(),
    ast_type<AST::InputInput>()
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
// ast-type: Ident
//
template<>
const ast_type_t ast_def_t<AST::Ident>::type = ast_type_t(
    "Ident", ast_type<AST::Node>(), ast_def_t<AST::Ident>::args, ast_def_t<AST::Ident>::narg);

template<>
const arg_def_t ast_def_t<AST::Ident>::args[] = {
    cptr<const char*>("ident")
};

template<>
const size_t ast_def_t<AST::Ident>::narg = 1;

//
// ast-type: Str
//
template<>
const ast_type_t ast_def_t<AST::Str>::type = ast_type_t(
    "Str", ast_type<AST::Node>(), ast_def_t<AST::Str>::args, ast_def_t<AST::Str>::narg);

template<>
const arg_def_t ast_def_t<AST::Str>::args[] = {
    cptr<const char*>("str")
};

template<>
const size_t ast_def_t<AST::Str>::narg = 1;

//
// ast-type: Expr
//
template<>
const ast_type_t ast_def_t<AST::Expr>::type = ast_type_t(
    "Expr", ast_type<AST::Node>(), ast_def_t<AST::Expr>::args, ast_def_t<AST::Expr>::narg);

template<>
const arg_def_t ast_def_t<AST::Expr>::args[] = {};

template<>
const size_t ast_def_t<AST::Expr>::narg = 0;

//
// ast-type: PrimaryExpr
//
template<>
const ast_type_t ast_def_t<AST::PrimaryExpr>::type = ast_type_t(
    "PrimaryExpr", ast_type<AST::Expr>(), ast_def_t<AST::PrimaryExpr>::args, ast_def_t<AST::PrimaryExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::PrimaryExpr>::args[] = {};

template<>
const size_t ast_def_t<AST::PrimaryExpr>::narg = 0;

//
// ast-type: IdentExpr
//
template<>
const ast_type_t ast_def_t<AST::IdentExpr>::type = ast_type_t(
    "IdentExpr", ast_type<AST::PrimaryExpr>(), ast_def_t<AST::IdentExpr>::args, ast_def_t<AST::IdentExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::IdentExpr>::args[] = {
    node<AST::Ident>("ident")
};

template<>
const size_t ast_def_t<AST::IdentExpr>::narg = 1;

//
// ast-type: StrExpr
//
template<>
const ast_type_t ast_def_t<AST::StrExpr>::type = ast_type_t(
    "StrExpr", ast_type<AST::PrimaryExpr>(), ast_def_t<AST::StrExpr>::args, ast_def_t<AST::StrExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::StrExpr>::args[] = {
    node<AST::Str>("str")
};

template<>
const size_t ast_def_t<AST::StrExpr>::narg = 1;

//
// ast-type: ParenExpr
//
template<>
const ast_type_t ast_def_t<AST::ParenExpr>::type = ast_type_t(
    "ParenExpr", ast_type<AST::PrimaryExpr>(), ast_def_t<AST::ParenExpr>::args, ast_def_t<AST::ParenExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::ParenExpr>::args[] = {
    node<AST::Expr>("expr")
};

template<>
const size_t ast_def_t<AST::ParenExpr>::narg = 1;

//
// ast-type: DictExpr
//
template<>
const ast_type_t ast_def_t<AST::DictExpr>::type = ast_type_t(
    "DictExpr", ast_type<AST::PrimaryExpr>(), ast_def_t<AST::DictExpr>::args, ast_def_t<AST::DictExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::DictExpr>::args[] = {};

template<>
const size_t ast_def_t<AST::DictExpr>::narg = 0;

//
// ast-type: EmptyDictExpr
//
template<>
const ast_type_t ast_def_t<AST::EmptyDictExpr>::type = ast_type_t(
    "EmptyDictExpr", ast_type<AST::DictExpr>(), ast_def_t<AST::EmptyDictExpr>::args, ast_def_t<AST::EmptyDictExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::EmptyDictExpr>::args[] = {};

template<>
const size_t ast_def_t<AST::EmptyDictExpr>::narg = 0;

//
// ast-type: KeyDatumDictExpr
//
template<>
const ast_type_t ast_def_t<AST::KeyDatumDictExpr>::type = ast_type_t(
    "KeyDatumDictExpr", ast_type<AST::DictExpr>(), ast_def_t<AST::KeyDatumDictExpr>::args, ast_def_t<AST::KeyDatumDictExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::KeyDatumDictExpr>::args[] = {
    node<AST::KeyDatumList>("key_datums")
};

template<>
const size_t ast_def_t<AST::KeyDatumDictExpr>::narg = 1;

//
// ast-type: KeyDatum
//
template<>
const ast_type_t ast_def_t<AST::KeyDatum>::type = ast_type_t(
    "KeyDatum", ast_type<AST::Node>(), ast_def_t<AST::KeyDatum>::args, ast_def_t<AST::KeyDatum>::narg);

template<>
const arg_def_t ast_def_t<AST::KeyDatum>::args[] = {
    node<AST::Expr>("expr1"),
    node<AST::Expr>("expr2")
};

template<>
const size_t ast_def_t<AST::KeyDatum>::narg = 2;

//
// ast-type: KeyDatumList
//
template<>
const ast_type_t ast_def_t<AST::KeyDatumList>::type = ast_type_t(
    "KeyDatumList", ast_type<AST::Node>(), ast_def_t<AST::KeyDatumList>::args, ast_def_t<AST::KeyDatumList>::narg);

template<>
const arg_def_t ast_def_t<AST::KeyDatumList>::args[] = {
    list<AST::KeyDatum>("list")
};

template<>
const size_t ast_def_t<AST::KeyDatumList>::narg = 1;

//
// ast-type: AttrRef
//
template<>
const ast_type_t ast_def_t<AST::AttrRef>::type = ast_type_t(
    "AttrRef", ast_type<AST::PrimaryExpr>(), ast_def_t<AST::AttrRef>::args, ast_def_t<AST::AttrRef>::narg);

template<>
const arg_def_t ast_def_t<AST::AttrRef>::args[] = {
    node<AST::PrimaryExpr>("expr"),
    node<AST::IdentExpr>("ident")
};

template<>
const size_t ast_def_t<AST::AttrRef>::narg = 2;

//
// ast-type: CallArg
//
template<>
const ast_type_t ast_def_t<AST::CallArg>::type = ast_type_t(
    "CallArg", ast_type<AST::Node>(), ast_def_t<AST::CallArg>::args, ast_def_t<AST::CallArg>::narg);

template<>
const arg_def_t ast_def_t<AST::CallArg>::args[] = {
    node<AST::Expr>("expr")
};

template<>
const size_t ast_def_t<AST::CallArg>::narg = 1;

//
// ast-type: CallArgList
//
template<>
const ast_type_t ast_def_t<AST::CallArgList>::type = ast_type_t(
    "CallArgList", ast_type<AST::Node>(), ast_def_t<AST::CallArgList>::args, ast_def_t<AST::CallArgList>::narg);

template<>
const arg_def_t ast_def_t<AST::CallArgList>::args[] = {
    list<AST::CallArg>("list")
};

template<>
const size_t ast_def_t<AST::CallArgList>::narg = 1;

//
// ast-type: CallExpr
//
template<>
const ast_type_t ast_def_t<AST::CallExpr>::type = ast_type_t(
    "CallExpr", ast_type<AST::PrimaryExpr>(), ast_def_t<AST::CallExpr>::args, ast_def_t<AST::CallExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::CallExpr>::args[] = {};

template<>
const size_t ast_def_t<AST::CallExpr>::narg = 0;

//
// ast-type: EmptyCallExpr
//
template<>
const ast_type_t ast_def_t<AST::EmptyCallExpr>::type = ast_type_t(
    "EmptyCallExpr", ast_type<AST::CallExpr>(), ast_def_t<AST::EmptyCallExpr>::args, ast_def_t<AST::EmptyCallExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::EmptyCallExpr>::args[] = {
    node<AST::PrimaryExpr>("expr")
};

template<>
const size_t ast_def_t<AST::EmptyCallExpr>::narg = 1;

//
// ast-type: ArgsCallExpr
//
template<>
const ast_type_t ast_def_t<AST::ArgsCallExpr>::type = ast_type_t(
    "ArgsCallExpr", ast_type<AST::CallExpr>(), ast_def_t<AST::ArgsCallExpr>::args, ast_def_t<AST::ArgsCallExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::ArgsCallExpr>::args[] = {
    node<AST::PrimaryExpr>("expr"),
    node<AST::CallArgList>("args")
};

template<>
const size_t ast_def_t<AST::ArgsCallExpr>::narg = 2;

//
// ast-type: OrTest
//
template<>
const ast_type_t ast_def_t<AST::OrTest>::type = ast_type_t(
    "OrTest", ast_type<AST::Expr>(), ast_def_t<AST::OrTest>::args, ast_def_t<AST::OrTest>::narg);

template<>
const arg_def_t ast_def_t<AST::OrTest>::args[] = {
    node<AST::Expr>("expr1"),
    node<AST::Expr>("expr2")
};

template<>
const size_t ast_def_t<AST::OrTest>::narg = 2;

//
// ast-type: AndTest
//
template<>
const ast_type_t ast_def_t<AST::AndTest>::type = ast_type_t(
    "AndTest", ast_type<AST::Expr>(), ast_def_t<AST::AndTest>::args, ast_def_t<AST::AndTest>::narg);

template<>
const arg_def_t ast_def_t<AST::AndTest>::args[] = {
    node<AST::Expr>("expr1"),
    node<AST::Expr>("expr2")
};

template<>
const size_t ast_def_t<AST::AndTest>::narg = 2;

//
// ast-type: NotTest
//
template<>
const ast_type_t ast_def_t<AST::NotTest>::type = ast_type_t(
    "NotTest", ast_type<AST::Expr>(), ast_def_t<AST::NotTest>::args, ast_def_t<AST::NotTest>::narg);

template<>
const arg_def_t ast_def_t<AST::NotTest>::args[] = {
    node<AST::Expr>("expr")
};

template<>
const size_t ast_def_t<AST::NotTest>::narg = 1;

//
// ast-type: Comp
//
template<>
const ast_type_t ast_def_t<AST::Comp>::type = ast_type_t(
    "Comp", ast_type<AST::Node>(), ast_def_t<AST::Comp>::args, ast_def_t<AST::Comp>::narg);

template<>
const arg_def_t ast_def_t<AST::Comp>::args[] = {
    cptr<AST::AST::comp_op_t::type_t>("op"),
    node<AST::PrimaryExpr>("prim")
};

template<>
const size_t ast_def_t<AST::Comp>::narg = 2;

//
// ast-type: CompExpr
//
template<>
const ast_type_t ast_def_t<AST::CompExpr>::type = ast_type_t(
    "CompExpr", ast_type<AST::Expr>(), ast_def_t<AST::CompExpr>::args, ast_def_t<AST::CompExpr>::narg);

template<>
const arg_def_t ast_def_t<AST::CompExpr>::args[] = {
    node<AST::PrimaryExpr>("prim"),
    list<AST::Comp>("comps")
};

template<>
const size_t ast_def_t<AST::CompExpr>::narg = 2;

//
// ast-type: Stmt
//
template<>
const ast_type_t ast_def_t<AST::Stmt>::type = ast_type_t(
    "Stmt", ast_type<AST::Node>(), ast_def_t<AST::Stmt>::args, ast_def_t<AST::Stmt>::narg);

template<>
const arg_def_t ast_def_t<AST::Stmt>::args[] = {};

template<>
const size_t ast_def_t<AST::Stmt>::narg = 0;

//
// ast-type: SimpleStmt
//
template<>
const ast_type_t ast_def_t<AST::SimpleStmt>::type = ast_type_t(
    "SimpleStmt", ast_type<AST::Stmt>(), ast_def_t<AST::SimpleStmt>::args, ast_def_t<AST::SimpleStmt>::narg);

template<>
const arg_def_t ast_def_t<AST::SimpleStmt>::args[] = {};

template<>
const size_t ast_def_t<AST::SimpleStmt>::narg = 0;

//
// ast-type: ExprStmt
//
template<>
const ast_type_t ast_def_t<AST::ExprStmt>::type = ast_type_t(
    "ExprStmt", ast_type<AST::SimpleStmt>(), ast_def_t<AST::ExprStmt>::args, ast_def_t<AST::ExprStmt>::narg);

template<>
const arg_def_t ast_def_t<AST::ExprStmt>::args[] = {
    node<AST::Expr>("expr")
};

template<>
const size_t ast_def_t<AST::ExprStmt>::narg = 1;

//
// ast-type: AssertStmt
//
template<>
const ast_type_t ast_def_t<AST::AssertStmt>::type = ast_type_t(
    "AssertStmt", ast_type<AST::SimpleStmt>(), ast_def_t<AST::AssertStmt>::args, ast_def_t<AST::AssertStmt>::narg);

template<>
const arg_def_t ast_def_t<AST::AssertStmt>::args[] = {};

template<>
const size_t ast_def_t<AST::AssertStmt>::narg = 0;

//
// ast-type: AssertStmt1
//
template<>
const ast_type_t ast_def_t<AST::AssertStmt1>::type = ast_type_t(
    "AssertStmt1", ast_type<AST::AssertStmt>(), ast_def_t<AST::AssertStmt1>::args, ast_def_t<AST::AssertStmt1>::narg);

template<>
const arg_def_t ast_def_t<AST::AssertStmt1>::args[] = {
    node<AST::Expr>("expr1")
};

template<>
const size_t ast_def_t<AST::AssertStmt1>::narg = 1;

//
// ast-type: AssertStmt2
//
template<>
const ast_type_t ast_def_t<AST::AssertStmt2>::type = ast_type_t(
    "AssertStmt2", ast_type<AST::AssertStmt>(), ast_def_t<AST::AssertStmt2>::args, ast_def_t<AST::AssertStmt2>::narg);

template<>
const arg_def_t ast_def_t<AST::AssertStmt2>::args[] = {
    node<AST::Expr>("expr1"),
    node<AST::Expr>("expr2")
};

template<>
const size_t ast_def_t<AST::AssertStmt2>::narg = 2;

//
// ast-type: Target
//
template<>
const ast_type_t ast_def_t<AST::Target>::type = ast_type_t(
    "Target", ast_type<AST::Node>(), ast_def_t<AST::Target>::args, ast_def_t<AST::Target>::narg);

template<>
const arg_def_t ast_def_t<AST::Target>::args[] = {};

template<>
const size_t ast_def_t<AST::Target>::narg = 0;

//
// ast-type: IdentTarget
//
template<>
const ast_type_t ast_def_t<AST::IdentTarget>::type = ast_type_t(
    "IdentTarget", ast_type<AST::Target>(), ast_def_t<AST::IdentTarget>::args, ast_def_t<AST::IdentTarget>::narg);

template<>
const arg_def_t ast_def_t<AST::IdentTarget>::args[] = {
    node<AST::IdentExpr>("ident")
};

template<>
const size_t ast_def_t<AST::IdentTarget>::narg = 1;

//
// ast-type: AttrRefTarget
//
template<>
const ast_type_t ast_def_t<AST::AttrRefTarget>::type = ast_type_t(
    "AttrRefTarget", ast_type<AST::Target>(), ast_def_t<AST::AttrRefTarget>::args, ast_def_t<AST::AttrRefTarget>::narg);

template<>
const arg_def_t ast_def_t<AST::AttrRefTarget>::args[] = {
    node<AST::AttrRef>("attr")
};

template<>
const size_t ast_def_t<AST::AttrRefTarget>::narg = 1;

//
// ast-type: TargetList
//
template<>
const ast_type_t ast_def_t<AST::TargetList>::type = ast_type_t(
    "TargetList", ast_type<AST::Node>(), ast_def_t<AST::TargetList>::args, ast_def_t<AST::TargetList>::narg);

template<>
const arg_def_t ast_def_t<AST::TargetList>::args[] = {
    list<AST::Target>("list")
};

template<>
const size_t ast_def_t<AST::TargetList>::narg = 1;

//
// ast-type: AssignStmt
//
template<>
const ast_type_t ast_def_t<AST::AssignStmt>::type = ast_type_t(
    "AssignStmt", ast_type<AST::SimpleStmt>(), ast_def_t<AST::AssignStmt>::args, ast_def_t<AST::AssignStmt>::narg);

template<>
const arg_def_t ast_def_t<AST::AssignStmt>::args[] = {
    node<AST::TargetList>("targets"),
    node<AST::Expr>("expr")
};

template<>
const size_t ast_def_t<AST::AssignStmt>::narg = 2;

//
// ast-type: DelStmt
//
template<>
const ast_type_t ast_def_t<AST::DelStmt>::type = ast_type_t(
    "DelStmt", ast_type<AST::SimpleStmt>(), ast_def_t<AST::DelStmt>::args, ast_def_t<AST::DelStmt>::narg);

template<>
const arg_def_t ast_def_t<AST::DelStmt>::args[] = {
    node<AST::TargetList>("targets")
};

template<>
const size_t ast_def_t<AST::DelStmt>::narg = 1;

//
// ast-type: ExprList
//
template<>
const ast_type_t ast_def_t<AST::ExprList>::type = ast_type_t(
    "ExprList", ast_type<AST::Node>(), ast_def_t<AST::ExprList>::args, ast_def_t<AST::ExprList>::narg);

template<>
const arg_def_t ast_def_t<AST::ExprList>::args[] = {
    list<AST::Expr>("list")
};

template<>
const size_t ast_def_t<AST::ExprList>::narg = 1;

//
// ast-type: PrintStmt
//
template<>
const ast_type_t ast_def_t<AST::PrintStmt>::type = ast_type_t(
    "PrintStmt", ast_type<AST::SimpleStmt>(), ast_def_t<AST::PrintStmt>::args, ast_def_t<AST::PrintStmt>::narg);

template<>
const arg_def_t ast_def_t<AST::PrintStmt>::args[] = {
    node<AST::ExprList>("exprs"),
    cptr<bool>("trail_comma")
};

template<>
const size_t ast_def_t<AST::PrintStmt>::narg = 2;

//
// ast-type: SimpleStmtList
//
template<>
const ast_type_t ast_def_t<AST::SimpleStmtList>::type = ast_type_t(
    "SimpleStmtList", ast_type<AST::Stmt>(), ast_def_t<AST::SimpleStmtList>::args, ast_def_t<AST::SimpleStmtList>::narg);

template<>
const arg_def_t ast_def_t<AST::SimpleStmtList>::args[] = {
    list<AST::SimpleStmt>("list")
};

template<>
const size_t ast_def_t<AST::SimpleStmtList>::narg = 1;

//
// ast-type: StmtList
//
template<>
const ast_type_t ast_def_t<AST::StmtList>::type = ast_type_t(
    "StmtList", ast_type<AST::Stmt>(), ast_def_t<AST::StmtList>::args, ast_def_t<AST::StmtList>::narg);

template<>
const arg_def_t ast_def_t<AST::StmtList>::args[] = {
    list<AST::Stmt>("list")
};

template<>
const size_t ast_def_t<AST::StmtList>::narg = 1;

//
// ast-type: FileInput
//
template<>
const ast_type_t ast_def_t<AST::FileInput>::type = ast_type_t(
    "FileInput", ast_type<AST::Node>(), ast_def_t<AST::FileInput>::args, ast_def_t<AST::FileInput>::narg);

template<>
const arg_def_t ast_def_t<AST::FileInput>::args[] = {
    node<AST::StmtList>("stmts")
};

template<>
const size_t ast_def_t<AST::FileInput>::narg = 1;

//
// ast-type: InteractInput
//
template<>
const ast_type_t ast_def_t<AST::InteractInput>::type = ast_type_t(
    "InteractInput", ast_type<AST::Node>(), ast_def_t<AST::InteractInput>::args, ast_def_t<AST::InteractInput>::narg);

template<>
const arg_def_t ast_def_t<AST::InteractInput>::args[] = {
    node<AST::SimpleStmtList>("stmts")
};

template<>
const size_t ast_def_t<AST::InteractInput>::narg = 1;

//
// ast-type: EvalInput
//
template<>
const ast_type_t ast_def_t<AST::EvalInput>::type = ast_type_t(
    "EvalInput", ast_type<AST::Node>(), ast_def_t<AST::EvalInput>::args, ast_def_t<AST::EvalInput>::narg);

template<>
const arg_def_t ast_def_t<AST::EvalInput>::args[] = {
    node<AST::Expr>("expr")
};

template<>
const size_t ast_def_t<AST::EvalInput>::narg = 1;

//
// ast-type: InputInput
//
template<>
const ast_type_t ast_def_t<AST::InputInput>::type = ast_type_t(
    "InputInput", ast_type<AST::Node>(), ast_def_t<AST::InputInput>::args, ast_def_t<AST::InputInput>::narg);

template<>
const arg_def_t ast_def_t<AST::InputInput>::args[] = {
    node<AST::Expr>("expr")
};

template<>
const size_t ast_def_t<AST::InputInput>::narg = 1;
// < AST_TYPE_DEFS

const TypeMaker::new_funcs_t TypeMaker::new_funcs[] = {
// > AST_TYPEMAKER_NEW_FUNCS $ grammar -NAT=16
    new_funcs_t("AndTest",          &TypeMaker::new_node<AST::AndTest>,          &TypeMaker::new_list<AST::AndTest>),
    new_funcs_t("ArgsCallExpr",     &TypeMaker::new_node<AST::ArgsCallExpr>,     &TypeMaker::new_list<AST::ArgsCallExpr>),
    new_funcs_t("AssertStmt",       &TypeMaker::new_node<AST::AssertStmt>,       &TypeMaker::new_list<AST::AssertStmt>),
    new_funcs_t("AssertStmt1",      &TypeMaker::new_node<AST::AssertStmt1>,      &TypeMaker::new_list<AST::AssertStmt1>),
    new_funcs_t("AssertStmt2",      &TypeMaker::new_node<AST::AssertStmt2>,      &TypeMaker::new_list<AST::AssertStmt2>),
    new_funcs_t("AssignStmt",       &TypeMaker::new_node<AST::AssignStmt>,       &TypeMaker::new_list<AST::AssignStmt>),
    new_funcs_t("AttrRef",          &TypeMaker::new_node<AST::AttrRef>,          &TypeMaker::new_list<AST::AttrRef>),
    new_funcs_t("AttrRefTarget",    &TypeMaker::new_node<AST::AttrRefTarget>,    &TypeMaker::new_list<AST::AttrRefTarget>),
    new_funcs_t("CallArg",          &TypeMaker::new_node<AST::CallArg>,          &TypeMaker::new_list<AST::CallArg>),
    new_funcs_t("CallArgList",      &TypeMaker::new_node<AST::CallArgList>,      &TypeMaker::new_list<AST::CallArgList>),
    new_funcs_t("CallExpr",         &TypeMaker::new_node<AST::CallExpr>,         &TypeMaker::new_list<AST::CallExpr>),
    new_funcs_t("Comp",             &TypeMaker::new_node<AST::Comp>,             &TypeMaker::new_list<AST::Comp>),
    new_funcs_t("CompExpr",         &TypeMaker::new_node<AST::CompExpr>,         &TypeMaker::new_list<AST::CompExpr>),
    new_funcs_t("DelStmt",          &TypeMaker::new_node<AST::DelStmt>,          &TypeMaker::new_list<AST::DelStmt>),
    new_funcs_t("DictExpr",         &TypeMaker::new_node<AST::DictExpr>,         &TypeMaker::new_list<AST::DictExpr>),
    new_funcs_t("EmptyCallExpr",    &TypeMaker::new_node<AST::EmptyCallExpr>,    &TypeMaker::new_list<AST::EmptyCallExpr>),
    new_funcs_t("EmptyDictExpr",    &TypeMaker::new_node<AST::EmptyDictExpr>,    &TypeMaker::new_list<AST::EmptyDictExpr>),
    new_funcs_t("EvalInput",        &TypeMaker::new_node<AST::EvalInput>,        &TypeMaker::new_list<AST::EvalInput>),
    new_funcs_t("Expr",             &TypeMaker::new_node<AST::Expr>,             &TypeMaker::new_list<AST::Expr>),
    new_funcs_t("ExprList",         &TypeMaker::new_node<AST::ExprList>,         &TypeMaker::new_list<AST::ExprList>),
    new_funcs_t("ExprStmt",         &TypeMaker::new_node<AST::ExprStmt>,         &TypeMaker::new_list<AST::ExprStmt>),
    new_funcs_t("FileInput",        &TypeMaker::new_node<AST::FileInput>,        &TypeMaker::new_list<AST::FileInput>),
    new_funcs_t("Ident",            &TypeMaker::new_node<AST::Ident>,            &TypeMaker::new_list<AST::Ident>),
    new_funcs_t("IdentExpr",        &TypeMaker::new_node<AST::IdentExpr>,        &TypeMaker::new_list<AST::IdentExpr>),
    new_funcs_t("IdentTarget",      &TypeMaker::new_node<AST::IdentTarget>,      &TypeMaker::new_list<AST::IdentTarget>),
    new_funcs_t("InputInput",       &TypeMaker::new_node<AST::InputInput>,       &TypeMaker::new_list<AST::InputInput>),
    new_funcs_t("InteractInput",    &TypeMaker::new_node<AST::InteractInput>,    &TypeMaker::new_list<AST::InteractInput>),
    new_funcs_t("KeyDatum",         &TypeMaker::new_node<AST::KeyDatum>,         &TypeMaker::new_list<AST::KeyDatum>),
    new_funcs_t("KeyDatumDictExpr", &TypeMaker::new_node<AST::KeyDatumDictExpr>, &TypeMaker::new_list<AST::KeyDatumDictExpr>),
    new_funcs_t("KeyDatumList",     &TypeMaker::new_node<AST::KeyDatumList>,     &TypeMaker::new_list<AST::KeyDatumList>),
    new_funcs_t("Node",             &TypeMaker::new_node<AST::Node>,             &TypeMaker::new_list<AST::Node>),
    new_funcs_t("NotTest",          &TypeMaker::new_node<AST::NotTest>,          &TypeMaker::new_list<AST::NotTest>),
    new_funcs_t("OrTest",           &TypeMaker::new_node<AST::OrTest>,           &TypeMaker::new_list<AST::OrTest>),
    new_funcs_t("ParenExpr",        &TypeMaker::new_node<AST::ParenExpr>,        &TypeMaker::new_list<AST::ParenExpr>),
    new_funcs_t("PrimaryExpr",      &TypeMaker::new_node<AST::PrimaryExpr>,      &TypeMaker::new_list<AST::PrimaryExpr>),
    new_funcs_t("PrintStmt",        &TypeMaker::new_node<AST::PrintStmt>,        &TypeMaker::new_list<AST::PrintStmt>),
    new_funcs_t("SimpleStmt",       &TypeMaker::new_node<AST::SimpleStmt>,       &TypeMaker::new_list<AST::SimpleStmt>),
    new_funcs_t("SimpleStmtList",   &TypeMaker::new_node<AST::SimpleStmtList>,   &TypeMaker::new_list<AST::SimpleStmtList>),
    new_funcs_t("Stmt",             &TypeMaker::new_node<AST::Stmt>,             &TypeMaker::new_list<AST::Stmt>),
    new_funcs_t("StmtList",         &TypeMaker::new_node<AST::StmtList>,         &TypeMaker::new_list<AST::StmtList>),
    new_funcs_t("Str",              &TypeMaker::new_node<AST::Str>,              &TypeMaker::new_list<AST::Str>),
    new_funcs_t("StrExpr",          &TypeMaker::new_node<AST::StrExpr>,          &TypeMaker::new_list<AST::StrExpr>),
    new_funcs_t("Target",           &TypeMaker::new_node<AST::Target>,           &TypeMaker::new_list<AST::Target>),
    new_funcs_t("TargetList",       &TypeMaker::new_node<AST::TargetList>,       &TypeMaker::new_list<AST::TargetList>)
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
obj_value_t NodeMaker::new_node<AST::Ident>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Ident>(node, args);
    return node_obj_value(type, ast->new_ident(
        cptr_arg<const char*>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::Str>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Str>(node, args);
    return node_obj_value(type, ast->new_str(
        cptr_arg<const char*>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::Expr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Expr>(node, args);
    return node_obj_value(type, ast->new_expr());
}

template<>
obj_value_t NodeMaker::new_node<AST::PrimaryExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::PrimaryExpr>(node, args);
    return node_obj_value(type, ast->new_primaryexpr());
}

template<>
obj_value_t NodeMaker::new_node<AST::IdentExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::IdentExpr>(node, args);
    return node_obj_value(type, ast->new_identexpr(
        node_arg<AST::Ident>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::StrExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::StrExpr>(node, args);
    return node_obj_value(type, ast->new_strexpr(
        node_arg<AST::Str>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::ParenExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::ParenExpr>(node, args);
    return node_obj_value(type, ast->new_parenexpr(
        node_arg<AST::Expr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::DictExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::DictExpr>(node, args);
    return node_obj_value(type, ast->new_dictexpr());
}

template<>
obj_value_t NodeMaker::new_node<AST::EmptyDictExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::EmptyDictExpr>(node, args);
    return node_obj_value(type, ast->new_emptydictexpr());
}

template<>
obj_value_t NodeMaker::new_node<AST::KeyDatumDictExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::KeyDatumDictExpr>(node, args);
    return node_obj_value(type, ast->new_keydatumdictexpr(
        node_arg<AST::KeyDatumList>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::KeyDatum>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::KeyDatum>(node, args);
    return node_obj_value(type, ast->new_keydatum(
        node_arg<AST::Expr>(0, type, args),
        node_arg<AST::Expr>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::KeyDatumList>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::KeyDatumList>(node, args);
    return node_obj_value(type, ast->new_keydatumlist(
        list_arg<AST::KeyDatum>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::AttrRef>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::AttrRef>(node, args);
    return node_obj_value(type, ast->new_attrref(
        node_arg<AST::PrimaryExpr>(0, type, args),
        node_arg<AST::IdentExpr>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::CallArg>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::CallArg>(node, args);
    return node_obj_value(type, ast->new_callarg(
        node_arg<AST::Expr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::CallArgList>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::CallArgList>(node, args);
    return node_obj_value(type, ast->new_callarglist(
        list_arg<AST::CallArg>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::CallExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::CallExpr>(node, args);
    return node_obj_value(type, ast->new_callexpr());
}

template<>
obj_value_t NodeMaker::new_node<AST::EmptyCallExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::EmptyCallExpr>(node, args);
    return node_obj_value(type, ast->new_emptycallexpr(
        node_arg<AST::PrimaryExpr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::ArgsCallExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::ArgsCallExpr>(node, args);
    return node_obj_value(type, ast->new_argscallexpr(
        node_arg<AST::PrimaryExpr>(0, type, args),
        node_arg<AST::CallArgList>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::OrTest>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::OrTest>(node, args);
    return node_obj_value(type, ast->new_ortest(
        node_arg<AST::Expr>(0, type, args),
        node_arg<AST::Expr>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::AndTest>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::AndTest>(node, args);
    return node_obj_value(type, ast->new_andtest(
        node_arg<AST::Expr>(0, type, args),
        node_arg<AST::Expr>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::NotTest>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::NotTest>(node, args);
    return node_obj_value(type, ast->new_nottest(
        node_arg<AST::Expr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::Comp>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Comp>(node, args);
    return node_obj_value(type, ast->new_comp(
        cptr_arg<AST::AST::comp_op_t::type_t>(0, type, args),
        node_arg<AST::PrimaryExpr>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::CompExpr>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::CompExpr>(node, args);
    return node_obj_value(type, ast->new_compexpr(
        node_arg<AST::PrimaryExpr>(0, type, args),
        list_arg<AST::Comp>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::Stmt>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Stmt>(node, args);
    return node_obj_value(type, ast->new_stmt());
}

template<>
obj_value_t NodeMaker::new_node<AST::SimpleStmt>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::SimpleStmt>(node, args);
    return node_obj_value(type, ast->new_simplestmt());
}

template<>
obj_value_t NodeMaker::new_node<AST::ExprStmt>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::ExprStmt>(node, args);
    return node_obj_value(type, ast->new_exprstmt(
        node_arg<AST::Expr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::AssertStmt>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::AssertStmt>(node, args);
    return node_obj_value(type, ast->new_assertstmt());
}

template<>
obj_value_t NodeMaker::new_node<AST::AssertStmt1>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::AssertStmt1>(node, args);
    return node_obj_value(type, ast->new_assertstmt1(
        node_arg<AST::Expr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::AssertStmt2>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::AssertStmt2>(node, args);
    return node_obj_value(type, ast->new_assertstmt2(
        node_arg<AST::Expr>(0, type, args),
        node_arg<AST::Expr>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::Target>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::Target>(node, args);
    return node_obj_value(type, ast->new_target());
}

template<>
obj_value_t NodeMaker::new_node<AST::IdentTarget>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::IdentTarget>(node, args);
    return node_obj_value(type, ast->new_identtarget(
        node_arg<AST::IdentExpr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::AttrRefTarget>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::AttrRefTarget>(node, args);
    return node_obj_value(type, ast->new_attrreftarget(
        node_arg<AST::AttrRef>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::TargetList>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::TargetList>(node, args);
    return node_obj_value(type, ast->new_targetlist(
        list_arg<AST::Target>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::AssignStmt>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::AssignStmt>(node, args);
    return node_obj_value(type, ast->new_assignstmt(
        node_arg<AST::TargetList>(0, type, args),
        node_arg<AST::Expr>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::DelStmt>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::DelStmt>(node, args);
    return node_obj_value(type, ast->new_delstmt(
        node_arg<AST::TargetList>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::ExprList>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::ExprList>(node, args);
    return node_obj_value(type, ast->new_exprlist(
        list_arg<AST::Expr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::PrintStmt>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::PrintStmt>(node, args);
    return node_obj_value(type, ast->new_printstmt(
        node_arg<AST::ExprList>(0, type, args),
        cptr_arg<bool>(1, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::SimpleStmtList>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::SimpleStmtList>(node, args);
    return node_obj_value(type, ast->new_simplestmtlist(
        list_arg<AST::SimpleStmt>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::StmtList>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::StmtList>(node, args);
    return node_obj_value(type, ast->new_stmtlist(
        list_arg<AST::Stmt>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::FileInput>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::FileInput>(node, args);
    return node_obj_value(type, ast->new_fileinput(
        node_arg<AST::StmtList>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::InteractInput>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::InteractInput>(node, args);
    return node_obj_value(type, ast->new_interactinput(
        node_arg<AST::SimpleStmtList>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::EvalInput>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::EvalInput>(node, args);
    return node_obj_value(type, ast->new_evalinput(
        node_arg<AST::Expr>(0, type, args)));
}

template<>
obj_value_t NodeMaker::new_node<AST::InputInput>(
    const TreeAST::TreeNode* node, obj_stack_t& args)
{
    auto type = check_node<AST::InputInput>(node, args);
    return node_obj_value(type, ast->new_inputinput(
        node_arg<AST::Expr>(0, type, args)));
}
// < AST_TYPE_NODE_MAKERS

const NodeMaker::new_funcs_t NodeMaker::new_funcs[] = {
// > AST_NODEMAKER_NEW_FUNCS $ grammar -NAN=16
    new_funcs_t("AndTest",          &NodeMaker::new_node<AST::AndTest>,          &NodeMaker::new_list<AST::AndTest>),
    new_funcs_t("ArgsCallExpr",     &NodeMaker::new_node<AST::ArgsCallExpr>,     &NodeMaker::new_list<AST::ArgsCallExpr>),
    new_funcs_t("AssertStmt",       &NodeMaker::new_node<AST::AssertStmt>,       &NodeMaker::new_list<AST::AssertStmt>),
    new_funcs_t("AssertStmt1",      &NodeMaker::new_node<AST::AssertStmt1>,      &NodeMaker::new_list<AST::AssertStmt1>),
    new_funcs_t("AssertStmt2",      &NodeMaker::new_node<AST::AssertStmt2>,      &NodeMaker::new_list<AST::AssertStmt2>),
    new_funcs_t("AssignStmt",       &NodeMaker::new_node<AST::AssignStmt>,       &NodeMaker::new_list<AST::AssignStmt>),
    new_funcs_t("AttrRef",          &NodeMaker::new_node<AST::AttrRef>,          &NodeMaker::new_list<AST::AttrRef>),
    new_funcs_t("AttrRefTarget",    &NodeMaker::new_node<AST::AttrRefTarget>,    &NodeMaker::new_list<AST::AttrRefTarget>),
    new_funcs_t("CallArg",          &NodeMaker::new_node<AST::CallArg>,          &NodeMaker::new_list<AST::CallArg>),
    new_funcs_t("CallArgList",      &NodeMaker::new_node<AST::CallArgList>,      &NodeMaker::new_list<AST::CallArgList>),
    new_funcs_t("CallExpr",         &NodeMaker::new_node<AST::CallExpr>,         &NodeMaker::new_list<AST::CallExpr>),
    new_funcs_t("Comp",             &NodeMaker::new_node<AST::Comp>,             &NodeMaker::new_list<AST::Comp>),
    new_funcs_t("CompExpr",         &NodeMaker::new_node<AST::CompExpr>,         &NodeMaker::new_list<AST::CompExpr>),
    new_funcs_t("DelStmt",          &NodeMaker::new_node<AST::DelStmt>,          &NodeMaker::new_list<AST::DelStmt>),
    new_funcs_t("DictExpr",         &NodeMaker::new_node<AST::DictExpr>,         &NodeMaker::new_list<AST::DictExpr>),
    new_funcs_t("EmptyCallExpr",    &NodeMaker::new_node<AST::EmptyCallExpr>,    &NodeMaker::new_list<AST::EmptyCallExpr>),
    new_funcs_t("EmptyDictExpr",    &NodeMaker::new_node<AST::EmptyDictExpr>,    &NodeMaker::new_list<AST::EmptyDictExpr>),
    new_funcs_t("EvalInput",        &NodeMaker::new_node<AST::EvalInput>,        &NodeMaker::new_list<AST::EvalInput>),
    new_funcs_t("Expr",             &NodeMaker::new_node<AST::Expr>,             &NodeMaker::new_list<AST::Expr>),
    new_funcs_t("ExprList",         &NodeMaker::new_node<AST::ExprList>,         &NodeMaker::new_list<AST::ExprList>),
    new_funcs_t("ExprStmt",         &NodeMaker::new_node<AST::ExprStmt>,         &NodeMaker::new_list<AST::ExprStmt>),
    new_funcs_t("FileInput",        &NodeMaker::new_node<AST::FileInput>,        &NodeMaker::new_list<AST::FileInput>),
    new_funcs_t("Ident",            &NodeMaker::new_node<AST::Ident>,            &NodeMaker::new_list<AST::Ident>),
    new_funcs_t("IdentExpr",        &NodeMaker::new_node<AST::IdentExpr>,        &NodeMaker::new_list<AST::IdentExpr>),
    new_funcs_t("IdentTarget",      &NodeMaker::new_node<AST::IdentTarget>,      &NodeMaker::new_list<AST::IdentTarget>),
    new_funcs_t("InputInput",       &NodeMaker::new_node<AST::InputInput>,       &NodeMaker::new_list<AST::InputInput>),
    new_funcs_t("InteractInput",    &NodeMaker::new_node<AST::InteractInput>,    &NodeMaker::new_list<AST::InteractInput>),
    new_funcs_t("KeyDatum",         &NodeMaker::new_node<AST::KeyDatum>,         &NodeMaker::new_list<AST::KeyDatum>),
    new_funcs_t("KeyDatumDictExpr", &NodeMaker::new_node<AST::KeyDatumDictExpr>, &NodeMaker::new_list<AST::KeyDatumDictExpr>),
    new_funcs_t("KeyDatumList",     &NodeMaker::new_node<AST::KeyDatumList>,     &NodeMaker::new_list<AST::KeyDatumList>),
    new_funcs_t("Node",             &NodeMaker::new_node<AST::Node>,             &NodeMaker::new_list<AST::Node>),
    new_funcs_t("NotTest",          &NodeMaker::new_node<AST::NotTest>,          &NodeMaker::new_list<AST::NotTest>),
    new_funcs_t("OrTest",           &NodeMaker::new_node<AST::OrTest>,           &NodeMaker::new_list<AST::OrTest>),
    new_funcs_t("ParenExpr",        &NodeMaker::new_node<AST::ParenExpr>,        &NodeMaker::new_list<AST::ParenExpr>),
    new_funcs_t("PrimaryExpr",      &NodeMaker::new_node<AST::PrimaryExpr>,      &NodeMaker::new_list<AST::PrimaryExpr>),
    new_funcs_t("PrintStmt",        &NodeMaker::new_node<AST::PrintStmt>,        &NodeMaker::new_list<AST::PrintStmt>),
    new_funcs_t("SimpleStmt",       &NodeMaker::new_node<AST::SimpleStmt>,       &NodeMaker::new_list<AST::SimpleStmt>),
    new_funcs_t("SimpleStmtList",   &NodeMaker::new_node<AST::SimpleStmtList>,   &NodeMaker::new_list<AST::SimpleStmtList>),
    new_funcs_t("Stmt",             &NodeMaker::new_node<AST::Stmt>,             &NodeMaker::new_list<AST::Stmt>),
    new_funcs_t("StmtList",         &NodeMaker::new_node<AST::StmtList>,         &NodeMaker::new_list<AST::StmtList>),
    new_funcs_t("Str",              &NodeMaker::new_node<AST::Str>,              &NodeMaker::new_list<AST::Str>),
    new_funcs_t("StrExpr",          &NodeMaker::new_node<AST::StrExpr>,          &NodeMaker::new_list<AST::StrExpr>),
    new_funcs_t("Target",           &NodeMaker::new_node<AST::Target>,           &NodeMaker::new_list<AST::Target>),
    new_funcs_t("TargetList",       &NodeMaker::new_node<AST::TargetList>,       &NodeMaker::new_list<AST::TargetList>)
// < AST_NODEMAKER_NEW_FUNCS
};

const size_t NodeMaker::n_new_func = Ext::array_size(NodeMaker::new_funcs);

} // namespace Obj

EXT_FUNC_ENTRY
Sys::ext_func_result_t cxxpy(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    return ext_func_entry(ast, opts);
}


