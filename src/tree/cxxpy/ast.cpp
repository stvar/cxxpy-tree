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

#include <functional>
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "sys.hpp"
#include "std-ext.hpp"
#include "ast.hpp"
#include "ast-base-impl.hpp"

namespace AST {

AstError::AstError(const char* msg) :
    std::runtime_error(msg)
{}

AstError::AstError(const std::string& msg) :
    std::runtime_error(msg)
{}

void error(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    auto err = Sys::vformat(msg, args);
    va_end(args);

    throw AstError(err);
}

AST::AST(
#ifdef DEBUG
    bool _debug
#endif
) :
    base_t(
#ifdef DEBUG
        _debug,
#endif
        8192, 131072, //!!! 4096, 65536,
        4096, 65536,
        base_t::node_constr,
        base_t::node_destr)
{}

AST::~AST()
{}

const char* AST::node_t::names[44] = {
// > AST_NODE_ENUM_NAMES $ grammar -NS
    "Node",             // node_t::type_t::Node
    "Ident",            // node_t::type_t::Ident
    "Str",              // node_t::type_t::Str
    "Expr",             // node_t::type_t::Expr
    "PrimaryExpr",      // node_t::type_t::PrimaryExpr
    "IdentExpr",        // node_t::type_t::IdentExpr
    "StrExpr",          // node_t::type_t::StrExpr
    "ParenExpr",        // node_t::type_t::ParenExpr
    "DictExpr",         // node_t::type_t::DictExpr
    "EmptyDictExpr",    // node_t::type_t::EmptyDictExpr
    "KeyDatumDictExpr", // node_t::type_t::KeyDatumDictExpr
    "KeyDatum",         // node_t::type_t::KeyDatum
    "KeyDatumList",     // node_t::type_t::KeyDatumList
    "AttrRef",          // node_t::type_t::AttrRef
    "CallArg",          // node_t::type_t::CallArg
    "CallArgList",      // node_t::type_t::CallArgList
    "CallExpr",         // node_t::type_t::CallExpr
    "EmptyCallExpr",    // node_t::type_t::EmptyCallExpr
    "ArgsCallExpr",     // node_t::type_t::ArgsCallExpr
    "OrTest",           // node_t::type_t::OrTest
    "AndTest",          // node_t::type_t::AndTest
    "NotTest",          // node_t::type_t::NotTest
    "Comp",             // node_t::type_t::Comp
    "CompExpr",         // node_t::type_t::CompExpr
    "Stmt",             // node_t::type_t::Stmt
    "SimpleStmt",       // node_t::type_t::SimpleStmt
    "ExprStmt",         // node_t::type_t::ExprStmt
    "AssertStmt",       // node_t::type_t::AssertStmt
    "AssertStmt1",      // node_t::type_t::AssertStmt1
    "AssertStmt2",      // node_t::type_t::AssertStmt2
    "Target",           // node_t::type_t::Target
    "IdentTarget",      // node_t::type_t::IdentTarget
    "AttrRefTarget",    // node_t::type_t::AttrRefTarget
    "TargetList",       // node_t::type_t::TargetList
    "AssignStmt",       // node_t::type_t::AssignStmt
    "DelStmt",          // node_t::type_t::DelStmt
    "ExprList",         // node_t::type_t::ExprList
    "PrintStmt",        // node_t::type_t::PrintStmt
    "SimpleStmtList",   // node_t::type_t::SimpleStmtList
    "StmtList",         // node_t::type_t::StmtList
    "FileInput",        // node_t::type_t::FileInput
    "InteractInput",    // node_t::type_t::InteractInput
    "EvalInput",        // node_t::type_t::EvalInput
    "InputInput",       // node_t::type_t::InputInput
// < AST_NODE_ENUM_NAMES
};

const char* AST::comp_op_t::names[4] = {
    "==",    // comp_op_t::type_t::equal
    "!=",    // comp_op_t::type_t::not_equal
    "is",    // comp_op_t::type_t::is
    "is not" // comp_op_t::type_t::is_not
};

// > NODE_LIST_ARGS_IMPL $ grammar -NLI
template<>
struct ArgList<CallArg>
{
    typedef call_arg_list_t list_t;
};

template<>
struct ArgList<Comp>
{
    typedef comp_list_t list_t;
};

template<>
struct ArgList<Expr>
{
    typedef expr_list_t list_t;
};

template<>
struct ArgList<KeyDatum>
{
    typedef key_datum_list_t list_t;
};

template<>
struct ArgList<SimpleStmt>
{
    typedef simple_stmt_list_t list_t;
};

template<>
struct ArgList<Stmt>
{
    typedef stmt_list_t list_t;
};

template<>
struct ArgList<Target>
{
    typedef target_list_t list_t;
};
// < NODE_LIST_ARGS_IMPL

template<>
struct Arg<Node, bool> : public ArgResult<bool>
{
    static bool alloc(BaseAST<Node>* self UNUSED, bool arg)
    { return arg; }
};

template<>
struct Arg<Node, AST::comp_op_t::type_t> :
    public ArgResult<AST::comp_op_t::type_t>
{
    static AST::comp_op_t::type_t alloc(
        BaseAST<Node>* self UNUSED, AST::comp_op_t::type_t arg)
    { return arg; }
};

void AST::root_node(const Node* node)
{
    Arg<Node>::check(this, node);
    root = node;
}

// > AST_NEW_NODE_IMPL $ grammar -NNI
const Node* AST::new_node()
{
    return new_node<Node>();
}

const Ident* AST::new_ident(const char* ident)
{
    return new_node<Ident>(alloc_arg(ident));
}

const Str* AST::new_str(const char* str)
{
    return new_node<Str>(alloc_arg(str));
}

const Expr* AST::new_expr()
{
    return new_node<Expr>();
}

const PrimaryExpr* AST::new_primaryexpr()
{
    return new_node<PrimaryExpr>();
}

const IdentExpr* AST::new_identexpr(const Ident* ident)
{
    return new_node<IdentExpr>(alloc_arg(ident));
}

const StrExpr* AST::new_strexpr(const Str* str)
{
    return new_node<StrExpr>(alloc_arg(str));
}

const ParenExpr* AST::new_parenexpr(const Expr* expr)
{
    return new_node<ParenExpr>(alloc_arg(expr));
}

const DictExpr* AST::new_dictexpr()
{
    return new_node<DictExpr>();
}

const EmptyDictExpr* AST::new_emptydictexpr()
{
    return new_node<EmptyDictExpr>();
}

const KeyDatumDictExpr* AST::new_keydatumdictexpr(const KeyDatumList* key_datums)
{
    return new_node<KeyDatumDictExpr>(alloc_arg(key_datums));
}

const KeyDatum* AST::new_keydatum(const Expr* expr1, const Expr* expr2)
{
    return new_node<KeyDatum>(alloc_arg(expr1), alloc_arg(expr2));
}

const KeyDatumList* AST::new_keydatumlist(const std::list<const KeyDatum*>* list)
{
    return new_node<KeyDatumList>(alloc_arg(list));
}

const AttrRef* AST::new_attrref(const PrimaryExpr* expr, const IdentExpr* ident)
{
    return new_node<AttrRef>(alloc_arg(expr), alloc_arg(ident));
}

const CallArg* AST::new_callarg(const Expr* expr)
{
    return new_node<CallArg>(alloc_arg(expr));
}

const CallArgList* AST::new_callarglist(const std::list<const CallArg*>* list)
{
    return new_node<CallArgList>(alloc_arg(list));
}

const CallExpr* AST::new_callexpr()
{
    return new_node<CallExpr>();
}

const EmptyCallExpr* AST::new_emptycallexpr(const PrimaryExpr* expr)
{
    return new_node<EmptyCallExpr>(alloc_arg(expr));
}

const ArgsCallExpr* AST::new_argscallexpr(const PrimaryExpr* expr, const CallArgList* args)
{
    return new_node<ArgsCallExpr>(alloc_arg(expr), alloc_arg(args));
}

const OrTest* AST::new_ortest(const Expr* expr1, const Expr* expr2)
{
    return new_node<OrTest>(alloc_arg(expr1), alloc_arg(expr2));
}

const AndTest* AST::new_andtest(const Expr* expr1, const Expr* expr2)
{
    return new_node<AndTest>(alloc_arg(expr1), alloc_arg(expr2));
}

const NotTest* AST::new_nottest(const Expr* expr)
{
    return new_node<NotTest>(alloc_arg(expr));
}

const Comp* AST::new_comp(const AST::comp_op_t::type_t op, const PrimaryExpr* prim)
{
    return new_node<Comp>(alloc_arg(op), alloc_arg(prim));
}

const CompExpr* AST::new_compexpr(const PrimaryExpr* prim, const std::list<const Comp*>* comps)
{
    return new_node<CompExpr>(alloc_arg(prim), alloc_arg(comps));
}

const Stmt* AST::new_stmt()
{
    return new_node<Stmt>();
}

const SimpleStmt* AST::new_simplestmt()
{
    return new_node<SimpleStmt>();
}

const ExprStmt* AST::new_exprstmt(const Expr* expr)
{
    return new_node<ExprStmt>(alloc_arg(expr));
}

const AssertStmt* AST::new_assertstmt()
{
    return new_node<AssertStmt>();
}

const AssertStmt1* AST::new_assertstmt1(const Expr* expr1)
{
    return new_node<AssertStmt1>(alloc_arg(expr1));
}

const AssertStmt2* AST::new_assertstmt2(const Expr* expr1, const Expr* expr2)
{
    return new_node<AssertStmt2>(alloc_arg(expr1), alloc_arg(expr2));
}

const Target* AST::new_target()
{
    return new_node<Target>();
}

const IdentTarget* AST::new_identtarget(const IdentExpr* ident)
{
    return new_node<IdentTarget>(alloc_arg(ident));
}

const AttrRefTarget* AST::new_attrreftarget(const AttrRef* attr)
{
    return new_node<AttrRefTarget>(alloc_arg(attr));
}

const TargetList* AST::new_targetlist(const std::list<const Target*>* list)
{
    return new_node<TargetList>(alloc_arg(list));
}

const AssignStmt* AST::new_assignstmt(const TargetList* targets, const Expr* expr)
{
    return new_node<AssignStmt>(alloc_arg(targets), alloc_arg(expr));
}

const DelStmt* AST::new_delstmt(const TargetList* targets)
{
    return new_node<DelStmt>(alloc_arg(targets));
}

const ExprList* AST::new_exprlist(const std::list<const Expr*>* list)
{
    return new_node<ExprList>(alloc_arg(list));
}

const PrintStmt* AST::new_printstmt(const ExprList* exprs, const bool trail_comma)
{
    return new_node<PrintStmt>(alloc_arg(exprs), alloc_arg(trail_comma));
}

const SimpleStmtList* AST::new_simplestmtlist(const std::list<const SimpleStmt*>* list)
{
    return new_node<SimpleStmtList>(alloc_arg(list));
}

const StmtList* AST::new_stmtlist(const std::list<const Stmt*>* list)
{
    return new_node<StmtList>(alloc_arg(list));
}

const FileInput* AST::new_fileinput(const StmtList* stmts)
{
    return new_node<FileInput>(alloc_arg(stmts));
}

const InteractInput* AST::new_interactinput(const SimpleStmtList* stmts)
{
    return new_node<InteractInput>(alloc_arg(stmts));
}

const EvalInput* AST::new_evalinput(const Expr* expr)
{
    return new_node<EvalInput>(alloc_arg(expr));
}

const InputInput* AST::new_inputinput(const Expr* expr)
{
    return new_node<InputInput>(alloc_arg(expr));
}
// < AST_NEW_NODE_IMPL

// > AST_NEW_NODE_LIST_ARGS_IMPL $ grammar -NNLI
const KeyDatumList* AST::new_keydatumlist(const key_datum_list_t list)
{
    return new_node<KeyDatumList>(alloc_arg(list));
}

const CallArgList* AST::new_callarglist(const call_arg_list_t list)
{
    return new_node<CallArgList>(alloc_arg(list));
}

const CompExpr* AST::new_compexpr(const PrimaryExpr* prim, const comp_list_t comps)
{
    return new_node<CompExpr>(alloc_arg(prim), alloc_arg(comps));
}

const TargetList* AST::new_targetlist(const target_list_t list)
{
    return new_node<TargetList>(alloc_arg(list));
}

const ExprList* AST::new_exprlist(const expr_list_t list)
{
    return new_node<ExprList>(alloc_arg(list));
}

const SimpleStmtList* AST::new_simplestmtlist(const simple_stmt_list_t list)
{
    return new_node<SimpleStmtList>(alloc_arg(list));
}

const StmtList* AST::new_stmtlist(const stmt_list_t list)
{
    return new_node<StmtList>(alloc_arg(list));
}
// < AST_NEW_NODE_LIST_ARGS_IMPL

Visitor0::~Visitor0()
{}

// > VISITOR0_IMPL $ grammar -VI=0 -v0result_t
result_t Visitor0::visit0(const Node* arg UNUSED)
{
    error("cannot visit Node");
    return 0;
}
// < VISITOR0_IMPL

Visitor::~Visitor()
{}

// > VISITOR_IMPL $ grammar -VI=0
void Visitor::visit(const Node* arg UNUSED)
{
    error("cannot visit Node");
}
// < VISITOR_IMPL

// > NODE_CLASSES_IMPL $ grammar -NI
Node::~Node()
{}

result_t Node::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Node::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Ident::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Ident::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Str::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Str::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Expr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Expr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t PrimaryExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void PrimaryExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t IdentExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void IdentExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t StrExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void StrExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ParenExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ParenExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t DictExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void DictExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t EmptyDictExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void EmptyDictExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t KeyDatumDictExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void KeyDatumDictExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t KeyDatum::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void KeyDatum::accept(Visitor* v) const
{
    v->visit(this);
}

result_t KeyDatumList::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void KeyDatumList::accept(Visitor* v) const
{
    v->visit(this);
}

result_t AttrRef::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void AttrRef::accept(Visitor* v) const
{
    v->visit(this);
}

result_t CallArg::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void CallArg::accept(Visitor* v) const
{
    v->visit(this);
}

result_t CallArgList::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void CallArgList::accept(Visitor* v) const
{
    v->visit(this);
}

result_t CallExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void CallExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t EmptyCallExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void EmptyCallExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ArgsCallExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ArgsCallExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t OrTest::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void OrTest::accept(Visitor* v) const
{
    v->visit(this);
}

result_t AndTest::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void AndTest::accept(Visitor* v) const
{
    v->visit(this);
}

result_t NotTest::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void NotTest::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Comp::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Comp::accept(Visitor* v) const
{
    v->visit(this);
}

result_t CompExpr::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void CompExpr::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Stmt::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Stmt::accept(Visitor* v) const
{
    v->visit(this);
}

result_t SimpleStmt::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void SimpleStmt::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ExprStmt::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ExprStmt::accept(Visitor* v) const
{
    v->visit(this);
}

result_t AssertStmt::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void AssertStmt::accept(Visitor* v) const
{
    v->visit(this);
}

result_t AssertStmt1::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void AssertStmt1::accept(Visitor* v) const
{
    v->visit(this);
}

result_t AssertStmt2::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void AssertStmt2::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Target::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Target::accept(Visitor* v) const
{
    v->visit(this);
}

result_t IdentTarget::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void IdentTarget::accept(Visitor* v) const
{
    v->visit(this);
}

result_t AttrRefTarget::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void AttrRefTarget::accept(Visitor* v) const
{
    v->visit(this);
}

result_t TargetList::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void TargetList::accept(Visitor* v) const
{
    v->visit(this);
}

result_t AssignStmt::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void AssignStmt::accept(Visitor* v) const
{
    v->visit(this);
}

result_t DelStmt::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void DelStmt::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ExprList::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ExprList::accept(Visitor* v) const
{
    v->visit(this);
}

result_t PrintStmt::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void PrintStmt::accept(Visitor* v) const
{
    v->visit(this);
}

result_t SimpleStmtList::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void SimpleStmtList::accept(Visitor* v) const
{
    v->visit(this);
}

result_t StmtList::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void StmtList::accept(Visitor* v) const
{
    v->visit(this);
}

result_t FileInput::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void FileInput::accept(Visitor* v) const
{
    v->visit(this);
}

result_t InteractInput::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void InteractInput::accept(Visitor* v) const
{
    v->visit(this);
}

result_t EvalInput::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void EvalInput::accept(Visitor* v) const
{
    v->visit(this);
}

result_t InputInput::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void InputInput::accept(Visitor* v) const
{
    v->visit(this);
}
// < NODE_CLASSES_IMPL

} // namespace AST

