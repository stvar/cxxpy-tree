// Copyright (C) 2016  Stefan Vargyas
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

static const char extension[] = "abc-printer";

namespace AST {

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
    void visit(const Node*); //!!! ???
// > PRINTER_VISITOR_DECL $ grammar -VD
    void visit(const A*);
    void visit(const B*);
    void visit(const C*);
    void visit(const D*);
    void visit(const N*);
    void visit(const M*);
    void visit(const P*);
    void visit(const Q*);
    void visit(const R*);
    void visit(const L*);
    void visit(const S*);
// < PRINTER_VISITOR_DECL
};

// > PRINTER_VISITOR_IMPL $ grammar -NP=Printer
void Printer::visit(const Node* arg)
{
    ost << empty(arg);
}

void Printer::visit(const A* arg)
{
    ost << empty(arg);
}

void Printer::visit(const B* arg)
{
    ost << name(arg) << '('
        << attr("a")
        << node(arg->a)
        << endl() << ')';
}

void Printer::visit(const C* arg)
{
    ost << name(arg) << '('
        << attr("b")
        << list(arg->b)
        << endl() << ')';
}

void Printer::visit(const D* arg)
{
    ost << name(arg) << '('
        << repr(arg->c) << ')';
}

void Printer::visit(const N* arg)
{
    ost << name(arg) << '('
        << attr("x")
        << node(arg->x)
        << attr("y")
        << node(arg->y)
        << attr("z")
        << node(arg->z)
        << endl() << ')';
}

void Printer::visit(const M* arg)
{
    ost << name(arg) << '('
        << attr("a")
        << node(arg->a)
        << attr("b")
        << list(arg->b)
        << repr(arg->c)
        << endl() << ')';
}

void Printer::visit(const P* arg)
{
    ost << name(arg) << '('
        << attr("x")
        << node(arg->x)
        << attr("y")
        << node(arg->y)
        << endl() << ')';
}

void Printer::visit(const Q* arg)
{
    ost << empty(arg);
}

void Printer::visit(const R* arg)
{
    ost << empty(arg);
}

void Printer::visit(const L* arg)
{
    ost << name(arg) << '('
        << attr("q")
        << list(arg->q)
        << endl() << ')';
}

void Printer::visit(const S* arg)
{
    ost << name(arg) << '('
        << attr("b")
        << type(arg->b)
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


