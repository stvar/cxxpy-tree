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

#include <iostream>

#include "ast.hpp"
#include "ast-base-printer.hpp"
#include "tree/ext-printer.hpp"

static const char extension[] = "cxxpy-printer";

namespace AST {

struct CompOpPrint
{
    CompOpPrint(AST::comp_op_t::type_t  _val) : val(_val) {}

    const char* name() const
    { return AST::comp_op_t::name(val); }

    void print(std::ostream& ost) const
    { ost << '\'' << name() << '\''; }

    AST::comp_op_t::type_t val;
};

inline std::ostream& operator<<(std::ostream& ost, const CompOpPrint& obj)
{ obj.print(ost); return ost; }

class Printer :
    public ExtBasePrinter<Node, Visitor>,
    private Visitor
{
public:
    typedef ExtBasePrinter<Node, Visitor> base_t;

    Printer(std::ostream& _ost) :
        base_t(_ost, this)
    {}

    using base_t::print;

private:
    static CompOpPrint type(AST::comp_op_t::type_t val)
    { return CompOpPrint(val); }

    void visit(const Node*); //!!! ???
// > PRINTER_VISITOR_DECL $ grammar -VD
    void visit(const Ident*);
    void visit(const Str*);
    void visit(const Expr*);
    void visit(const PrimaryExpr*);
    void visit(const IdentExpr*);
    void visit(const StrExpr*);
    void visit(const ParenExpr*);
    void visit(const DictExpr*);
    void visit(const EmptyDictExpr*);
    void visit(const KeyDatumDictExpr*);
    void visit(const KeyDatum*);
    void visit(const KeyDatumList*);
    void visit(const AttrRef*);
    void visit(const CallArg*);
    void visit(const CallArgList*);
    void visit(const CallExpr*);
    void visit(const EmptyCallExpr*);
    void visit(const ArgsCallExpr*);
    void visit(const OrTest*);
    void visit(const AndTest*);
    void visit(const NotTest*);
    void visit(const Comp*);
    void visit(const CompExpr*);
    void visit(const Stmt*);
    void visit(const SimpleStmt*);
    void visit(const ExprStmt*);
    void visit(const AssertStmt*);
    void visit(const AssertStmt1*);
    void visit(const AssertStmt2*);
    void visit(const Target*);
    void visit(const IdentTarget*);
    void visit(const AttrRefTarget*);
    void visit(const TargetList*);
    void visit(const AssignStmt*);
    void visit(const DelStmt*);
    void visit(const ExprList*);
    void visit(const PrintStmt*);
    void visit(const SimpleStmtList*);
    void visit(const StmtList*);
    void visit(const FileInput*);
    void visit(const InteractInput*);
    void visit(const EvalInput*);
    void visit(const InputInput*);
// < PRINTER_VISITOR_DECL

    using base_t::type;
};

// > PRINTER_VISITOR_IMPL $ grammar -NP=Printer
void Printer::visit(const Node* arg)
{
    ost << empty(arg);
}

void Printer::visit(const Ident* arg)
{
    ost << name(arg) << '('
        << repr(arg->ident) << ')';
}

void Printer::visit(const Str* arg)
{
    ost << name(arg) << '('
        << repr(arg->str) << ')';
}

void Printer::visit(const Expr* arg)
{
    ost << empty(arg);
}

void Printer::visit(const PrimaryExpr* arg)
{
    ost << empty(arg);
}

void Printer::visit(const IdentExpr* arg)
{
    ost << name(arg) << '('
        << attr("ident")
        << node(arg->ident)
        << endl() << ')';
}

void Printer::visit(const StrExpr* arg)
{
    ost << name(arg) << '('
        << attr("str")
        << node(arg->str)
        << endl() << ')';
}

void Printer::visit(const ParenExpr* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}

void Printer::visit(const DictExpr* arg)
{
    ost << empty(arg);
}

void Printer::visit(const EmptyDictExpr* arg)
{
    ost << empty(arg);
}

void Printer::visit(const KeyDatumDictExpr* arg)
{
    ost << name(arg) << '('
        << attr("key_datums")
        << node(arg->key_datums)
        << endl() << ')';
}

void Printer::visit(const KeyDatum* arg)
{
    ost << name(arg) << '('
        << attr("expr1")
        << node(arg->expr1)
        << attr("expr2")
        << node(arg->expr2)
        << endl() << ')';
}

void Printer::visit(const KeyDatumList* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << list(arg->list)
        << endl() << ')';
}

void Printer::visit(const AttrRef* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << attr("ident")
        << node(arg->ident)
        << endl() << ')';
}

void Printer::visit(const CallArg* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}

void Printer::visit(const CallArgList* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << list(arg->list)
        << endl() << ')';
}

void Printer::visit(const CallExpr* arg)
{
    ost << empty(arg);
}

void Printer::visit(const EmptyCallExpr* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}

void Printer::visit(const ArgsCallExpr* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << attr("args")
        << node(arg->args)
        << endl() << ')';
}

void Printer::visit(const OrTest* arg)
{
    ost << name(arg) << '('
        << attr("expr1")
        << node(arg->expr1)
        << attr("expr2")
        << node(arg->expr2)
        << endl() << ')';
}

void Printer::visit(const AndTest* arg)
{
    ost << name(arg) << '('
        << attr("expr1")
        << node(arg->expr1)
        << attr("expr2")
        << node(arg->expr2)
        << endl() << ')';
}

void Printer::visit(const NotTest* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}

void Printer::visit(const Comp* arg)
{
    ost << name(arg) << '('
        << attr("op")
        << type(arg->op)
        << attr("prim")
        << node(arg->prim)
        << endl() << ')';
}

void Printer::visit(const CompExpr* arg)
{
    ost << name(arg) << '('
        << attr("prim")
        << node(arg->prim)
        << attr("comps")
        << list(arg->comps)
        << endl() << ')';
}

void Printer::visit(const Stmt* arg)
{
    ost << empty(arg);
}

void Printer::visit(const SimpleStmt* arg)
{
    ost << empty(arg);
}

void Printer::visit(const ExprStmt* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}

void Printer::visit(const AssertStmt* arg)
{
    ost << empty(arg);
}

void Printer::visit(const AssertStmt1* arg)
{
    ost << name(arg) << '('
        << attr("expr1")
        << node(arg->expr1)
        << endl() << ')';
}

void Printer::visit(const AssertStmt2* arg)
{
    ost << name(arg) << '('
        << attr("expr1")
        << node(arg->expr1)
        << attr("expr2")
        << node(arg->expr2)
        << endl() << ')';
}

void Printer::visit(const Target* arg)
{
    ost << empty(arg);
}

void Printer::visit(const IdentTarget* arg)
{
    ost << name(arg) << '('
        << attr("ident")
        << node(arg->ident)
        << endl() << ')';
}

void Printer::visit(const AttrRefTarget* arg)
{
    ost << name(arg) << '('
        << attr("attr")
        << node(arg->attr)
        << endl() << ')';
}

void Printer::visit(const TargetList* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << list(arg->list)
        << endl() << ')';
}

void Printer::visit(const AssignStmt* arg)
{
    ost << name(arg) << '('
        << attr("targets")
        << node(arg->targets)
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}

void Printer::visit(const DelStmt* arg)
{
    ost << name(arg) << '('
        << attr("targets")
        << node(arg->targets)
        << endl() << ')';
}

void Printer::visit(const ExprList* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << list(arg->list)
        << endl() << ')';
}

void Printer::visit(const PrintStmt* arg)
{
    ost << name(arg) << '('
        << attr("exprs")
        << node(arg->exprs)
        << attr("trail_comma")
        << type(arg->trail_comma)
        << endl() << ')';
}

void Printer::visit(const SimpleStmtList* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << list(arg->list)
        << endl() << ')';
}

void Printer::visit(const StmtList* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << list(arg->list)
        << endl() << ')';
}

void Printer::visit(const FileInput* arg)
{
    ost << name(arg) << '('
        << attr("stmts")
        << node(arg->stmts)
        << endl() << ')';
}

void Printer::visit(const InteractInput* arg)
{
    ost << name(arg) << '('
        << attr("stmts")
        << node(arg->stmts)
        << endl() << ')';
}

void Printer::visit(const EvalInput* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}

void Printer::visit(const InputInput* arg)
{
    ost << name(arg) << '('
        << attr("expr")
        << node(arg->expr)
        << endl() << ')';
}
// < PRINTER_VISITOR_IMPL

} // namespace AST

#include "tree/ext-printer-impl.hpp"

EXT_FUNC_ENTRY
Sys::ext_func_result_t print(const AST::Node* obj, const Obj::options_t* opt)
{
    return ext_print_func_entry(obj, opt);
}


