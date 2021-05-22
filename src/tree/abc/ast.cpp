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
        256, 16384,
        256, 16384,
        base_t::node_constr,
        base_t::node_destr)
{}

AST::~AST()
{}

const char* AST::node_t::names[12] = {
// > AST_NODE_ENUM_NAMES $ grammar -NS
    "Node",             // node_t::type_t::Node
    "A",                // node_t::type_t::A
    "B",                // node_t::type_t::B
    "C",                // node_t::type_t::C
    "D",                // node_t::type_t::D
    "N",                // node_t::type_t::N
    "M",                // node_t::type_t::M
    "P",                // node_t::type_t::P
    "Q",                // node_t::type_t::Q
    "R",                // node_t::type_t::R
    "L",                // node_t::type_t::L
    "S",                // node_t::type_t::S
// < AST_NODE_ENUM_NAMES
};

// > NODE_LIST_ARGS_IMPL $ grammar -NLI
template<>
struct ArgList<B>
{
    typedef b_list_t list_t;
};

template<>
struct ArgList<Q>
{
    typedef q_list_t list_t;
};
// < NODE_LIST_ARGS_IMPL

template<>
struct Arg<Node, bool> : public ArgResult<bool>
{
    static bool alloc(BaseAST<Node>* self UNUSED, bool arg)
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

const A* AST::new_a()
{
    return new_node<A>();
}

const B* AST::new_b(const A* a)
{
    return new_node<B>(alloc_arg(a));
}

const C* AST::new_c(const std::list<const B*>* b)
{
    return new_node<C>(alloc_arg(b));
}

const D* AST::new_d(const char* c)
{
    return new_node<D>(alloc_arg(c));
}

const N* AST::new_n(const B* x, const C* y, const D* z)
{
    return new_node<N>(alloc_arg(x), alloc_arg(y), alloc_arg(z));
}

const M* AST::new_m(const A* a, const std::list<const B*>* b, const char* c)
{
    return new_node<M>(alloc_arg(a), alloc_arg(b), alloc_arg(c));
}

const P* AST::new_p(const A* x, const A* y)
{
    return new_node<P>(alloc_arg(x), alloc_arg(y));
}

const Q* AST::new_q()
{
    return new_node<Q>();
}

const R* AST::new_r()
{
    return new_node<R>();
}

const L* AST::new_l(const std::list<const Q*>* q)
{
    return new_node<L>(alloc_arg(q));
}

const S* AST::new_s(const bool b)
{
    return new_node<S>(alloc_arg(b));
}
// < AST_NEW_NODE_IMPL

// > AST_NEW_NODE_LIST_ARGS_IMPL $ grammar -NNLI
const C* AST::new_c(const b_list_t b)
{
    return new_node<C>(alloc_arg(b));
}

const M* AST::new_m(const A* a, const b_list_t b, const char* c)
{
    return new_node<M>(alloc_arg(a), alloc_arg(b), alloc_arg(c));
}

const L* AST::new_l(const q_list_t q)
{
    return new_node<L>(alloc_arg(q));
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

result_t A::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void A::accept(Visitor* v) const
{
    v->visit(this);
}

result_t B::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void B::accept(Visitor* v) const
{
    v->visit(this);
}

result_t C::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void C::accept(Visitor* v) const
{
    v->visit(this);
}

result_t D::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void D::accept(Visitor* v) const
{
    v->visit(this);
}

result_t N::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void N::accept(Visitor* v) const
{
    v->visit(this);
}

result_t M::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void M::accept(Visitor* v) const
{
    v->visit(this);
}

result_t P::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void P::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Q::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Q::accept(Visitor* v) const
{
    v->visit(this);
}

result_t R::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void R::accept(Visitor* v) const
{
    v->visit(this);
}

result_t L::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void L::accept(Visitor* v) const
{
    v->visit(this);
}

result_t S::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void S::accept(Visitor* v) const
{
    v->visit(this);
}
// < NODE_CLASSES_IMPL

} // namespace AST

