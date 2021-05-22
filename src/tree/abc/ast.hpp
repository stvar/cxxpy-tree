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

#ifndef __AST_HPP
#define __AST_HPP

#include "config.h"

#include <cstddef>

#include <iterator>
#include <algorithm>
#include <list>
#include <stdexcept>
#include <utility>

#include "sys.hpp"
#include "std-ext.hpp"
#include "ast-base.hpp"

namespace AST {

struct AstError : public std::runtime_error
{
    AstError(const char* msg);
    AstError(const std::string& msg);
};

// > NODE_CLASSES_FORWARD $ grammar -NF
class Node;
class A;
class B;
class C;
class D;
class N;
class M;
class P;
class Q;
class R;
class L;
class S;
// < NODE_CLASSES_FORWARD

// > NODE_LIST_ARGS_DECL $ grammar -NLD
typedef std::pair<size_t, B const* const*> b_list_t;
typedef std::pair<size_t, Q const* const*> q_list_t;
// < NODE_LIST_ARGS_DECL

using namespace AST;

class AST : private BaseAST<Node>
{
public:
    typedef BaseAST<Node> base_t;

    struct node_t :
        public base_enum_t<node_t>
    {
        enum type_t {
// > AST_NODE_ENUM $ grammar -NE
            Node,
            A,
            B,
            C,
            D,
            N,
            M,
            P,
            Q,
            R,
            L,
            S,
// < AST_NODE_ENUM
        };

    private:
        friend class base_enum_t<node_t>;
        static const char* names[12];
    };

    AST(const AST&) = delete;
    AST& operator=(const AST&) = delete;

    AST(AST&&) = delete;
    AST& operator=(AST&&) = delete;

    AST(
#ifdef DEBUG
        bool _debug = false
#endif
    );
    ~AST();

    const Node* root_node() const { return root; }
    void root_node(const Node*);

    size_t num_node() const { return base_t::num_node(); }

// > AST_NEW_NODE_DECL $ grammar -NND
    const Node* new_node();
    const A* new_a();
    const B* new_b(const A* a);
    const C* new_c(const std::list<const B*>* b);
    const D* new_d(const char* c);
    const N* new_n(const B* x, const C* y, const D* z);
    const M* new_m(const A* a, const std::list<const B*>* b, const char* c);
    const P* new_p(const A* x, const A* y);
    const Q* new_q();
    const R* new_r();
    const L* new_l(const std::list<const Q*>* q);
    const S* new_s(const bool b);
// < AST_NEW_NODE_DECL

// > AST_NEW_NODE_LIST_ARGS_DECL $ grammar -NNLD
    const C* new_c(const b_list_t b);
    const M* new_m(const A* a, const b_list_t b, const char* c);
    const L* new_l(const q_list_t q);
// < AST_NEW_NODE_LIST_ARGS_DECL

    template<typename T>
    T* new_list(size_t sz)
    {
        return sz > 0
            ? static_cast<T*>(raw_alloc.allocate<T>(sz))
            : nullptr;
    }

    typedef base_t::node_iterator node_iterator;

    node_iterator node_begin() const { return base_t::node_begin(); }
    node_iterator node_end() const   { return base_t::node_end(); }

private:
    using base_t::new_node;

    template<typename T>
    typename Arg<root_t, T>::result_t alloc_arg(T arg)
    {
        return Arg<root_t, T>::alloc(this, arg);
    }

// > NODE_LIST_ARGS_FRIENDS $ grammar -NLF
    friend class Arg<root_t, const std::list<const B*>*>;
    friend class Arg<root_t, const std::list<const Q*>*>;
// < NODE_LIST_ARGS_FRIENDS

// > NODE_LIST_TYPED_ARGS_FRIENDS $ grammar -NLT
    friend class Arg<root_t, b_list_t>;
    friend class Arg<root_t, q_list_t>;
// < NODE_LIST_TYPED_ARGS_FRIENDS
};

class Visitor0;
class Visitor;

typedef Ext::widest_uint_t result_t;

// > NODE_CLASSES_DEF $ grammar -NDP
class Node
{
public:
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) = delete;
    Node& operator=(Node&&) = delete;

    Node() : t(0) {} // stev: n is initialized elsewere

    virtual ~Node();

    virtual AST::node_t::type_t type() const { return AST::node_t::Node; }
    const char* name() const { return AST::node_t::name(type()); }

    virtual result_t accept(Visitor0*) const;
    virtual void accept(Visitor*) const;

    unsigned short tag() const { return t; };
    void tag(unsigned short _t) const { t = _t; };

private:
    friend class BaseAST<Node>;

    mutable unsigned short t;
};

class A : public Node
{
public:
    A() {}

    AST::node_t::type_t type() const { return AST::node_t::A; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class B : public Node
{
public:
    B(const A* _a) :
        a(_a)
    {}

    AST::node_t::type_t type() const { return AST::node_t::B; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const A* a;
};

class C : public Node
{
public:
    C(const b_list_t _b) :
        b(_b)
    {}

    AST::node_t::type_t type() const { return AST::node_t::C; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const b_list_t b;
};

class D : public Node
{
public:
    D(const char* _c) :
        c(_c)
    {}

    AST::node_t::type_t type() const { return AST::node_t::D; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const char* c;
};

class N : public Node
{
public:
    N(const B* _x, const C* _y, const D* _z) :
        x(_x),
        y(_y),
        z(_z)
    {}

    AST::node_t::type_t type() const { return AST::node_t::N; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const B* x;
    const C* y;
    const D* z;
};

class M : public Node
{
public:
    M(const A* _a, const b_list_t _b, const char* _c) :
        a(_a),
        b(_b),
        c(_c)
    {}

    AST::node_t::type_t type() const { return AST::node_t::M; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const A* a;
    const b_list_t b;
    const char* c;
};

class P : public Node
{
public:
    P(const A* _x, const A* _y) :
        x(_x),
        y(_y)
    {}

    AST::node_t::type_t type() const { return AST::node_t::P; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const A* x;
    const A* y;
};

class Q : public A
{
public:
    Q() {}

    AST::node_t::type_t type() const { return AST::node_t::Q; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class R : public Q
{
public:
    R() {}

    AST::node_t::type_t type() const { return AST::node_t::R; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class L : public Node
{
public:
    L(const q_list_t _q) :
        q(_q)
    {}

    AST::node_t::type_t type() const { return AST::node_t::L; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const q_list_t q;
};

class S : public Q
{
public:
    S(const bool _b) :
        b(_b)
    {}

    AST::node_t::type_t type() const { return AST::node_t::S; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const bool b;
};
// < NODE_CLASSES_DEF

template<typename T>
struct NodeCast;

template<typename T>
const T* node_cast(const Node* node)
{
    return node && node->type() == NodeCast<T>::type
        ? static_cast<const T*>(node)
        : nullptr;
}

template<typename T>
inline const char* node_name()
{
    return AST::node_t::name(NodeCast<T>::type);
}

// > NODE_CAST_DECL $ grammar -NC
template<>
struct NodeCast<Node>
{
    static const AST::node_t::type_t type = AST::node_t::Node;
};

template<>
struct NodeCast<A>
{
    static const AST::node_t::type_t type = AST::node_t::A;
};

template<>
struct NodeCast<B>
{
    static const AST::node_t::type_t type = AST::node_t::B;
};

template<>
struct NodeCast<C>
{
    static const AST::node_t::type_t type = AST::node_t::C;
};

template<>
struct NodeCast<D>
{
    static const AST::node_t::type_t type = AST::node_t::D;
};

template<>
struct NodeCast<N>
{
    static const AST::node_t::type_t type = AST::node_t::N;
};

template<>
struct NodeCast<M>
{
    static const AST::node_t::type_t type = AST::node_t::M;
};

template<>
struct NodeCast<P>
{
    static const AST::node_t::type_t type = AST::node_t::P;
};

template<>
struct NodeCast<Q>
{
    static const AST::node_t::type_t type = AST::node_t::Q;
};

template<>
struct NodeCast<R>
{
    static const AST::node_t::type_t type = AST::node_t::R;
};

template<>
struct NodeCast<L>
{
    static const AST::node_t::type_t type = AST::node_t::L;
};

template<>
struct NodeCast<S>
{
    static const AST::node_t::type_t type = AST::node_t::S;
};
// < NODE_CAST_DECL

class Visitor0
{
public:
    virtual ~Visitor0();

// > VISITOR0_DECL $ grammar -VD=0 -v0result_t
    virtual result_t visit0(const Node*);
    virtual result_t visit0(const A*) = 0;
    virtual result_t visit0(const B*) = 0;
    virtual result_t visit0(const C*) = 0;
    virtual result_t visit0(const D*) = 0;
    virtual result_t visit0(const N*) = 0;
    virtual result_t visit0(const M*) = 0;
    virtual result_t visit0(const P*) = 0;
    virtual result_t visit0(const Q*) = 0;
    virtual result_t visit0(const R*) = 0;
    virtual result_t visit0(const L*) = 0;
    virtual result_t visit0(const S*) = 0;
// < VISITOR0_DECL
};

template <typename T>
class EvalVisitor : private Visitor0
{
public:
    typedef ResultCast<T> cast_t;

    T eval(const Node* node);

protected:
// > EVAL_VISITOR_VISITOR_DECL $ grammar -VED
    virtual T visit(const Node*) = 0;
    virtual T visit(const A*) = 0;
    virtual T visit(const B*) = 0;
    virtual T visit(const C*) = 0;
    virtual T visit(const D*) = 0;
    virtual T visit(const N*) = 0;
    virtual T visit(const M*) = 0;
    virtual T visit(const P*) = 0;
    virtual T visit(const Q*) = 0;
    virtual T visit(const R*) = 0;
    virtual T visit(const L*) = 0;
    virtual T visit(const S*) = 0;
// < EVAL_VISITOR_VISITOR_DECL

private:
// > EVAL_VISITOR_VISITOR0_DECL $ grammar -VD -v0result_t
    result_t visit0(const A*);
    result_t visit0(const B*);
    result_t visit0(const C*);
    result_t visit0(const D*);
    result_t visit0(const N*);
    result_t visit0(const M*);
    result_t visit0(const P*);
    result_t visit0(const Q*);
    result_t visit0(const R*);
    result_t visit0(const L*);
    result_t visit0(const S*);
// < EVAL_VISITOR_VISITOR0_DECL
};

// > EVAL_VISITOR_VISITOR0_IMPL $ grammar -VEI -v0result_t
template <typename T>
result_t EvalVisitor<T>::visit0(const A* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const B* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const C* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const D* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const N* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const M* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const P* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const Q* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const R* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const L* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const S* arg)
{
    return cast_t::cast(visit(arg));
};
// < EVAL_VISITOR_VISITOR0_IMPL

template <typename T>
inline T EvalVisitor<T>::eval(const Node* node)
{
    return cast_t::cast(node->accept(this));
}

class Visitor
{
public:
    virtual ~Visitor();

// > VISITOR_DECL $ grammar -VD=0
    virtual void visit(const Node*);
    virtual void visit(const A*) = 0;
    virtual void visit(const B*) = 0;
    virtual void visit(const C*) = 0;
    virtual void visit(const D*) = 0;
    virtual void visit(const N*) = 0;
    virtual void visit(const M*) = 0;
    virtual void visit(const P*) = 0;
    virtual void visit(const Q*) = 0;
    virtual void visit(const R*) = 0;
    virtual void visit(const L*) = 0;
    virtual void visit(const S*) = 0;
// < VISITOR_DECL
};

} // namespace AST

#endif/*__AST_HPP*/

