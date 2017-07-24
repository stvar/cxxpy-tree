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

#ifndef __TREE_AST_HPP
#define __TREE_AST_HPP

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

namespace TreeAST {

struct AstError : public std::runtime_error
{
    AstError(const char* msg);
    AstError(const std::string& msg);
};

void error(const char* msg, ...) PRINTF_FMT(1);

// > NODE_CLASSES_FORWARD $ grammar -NF
class Node;
class Literal;
class ListElem;
class NodeName;
class NodeList;
class ArgName;
class ArgValue;
class NodeValue;
class ListValue;
class LiteralValue;
class NodeArg;
class NodeArgs;
class ListArgs;
class LiteralArgs;
class TreeNode;
class Tree;
// < NODE_CLASSES_FORWARD

// > NODE_LIST_ARGS_DECL $ grammar -NLD
typedef std::pair<size_t, ListElem const* const*> list_elem_list_t;
typedef std::pair<size_t, NodeArg const* const*> node_arg_list_t;
// < NODE_LIST_ARGS_DECL

typedef unsigned char list_elem_idx_t;

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
            Literal,
            ListElem,
            NodeName,
            NodeList,
            ArgName,
            ArgValue,
            NodeValue,
            ListValue,
            LiteralValue,
            NodeArg,
            NodeArgs,
            ListArgs,
            LiteralArgs,
            TreeNode,
            Tree,
// < AST_NODE_ENUM
        };

    private:
        friend class base_enum_t<node_t>;
        static const char* names[16];
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
    size_t num_treenode() const { return n_treenodes; }

// > AST_NEW_NODE_DECL $ grammar -NND
    const Node* new_node();
    const Literal* new_literal(const char* str);
    const ListElem* new_listelem(const list_elem_idx_t index, const TreeNode* node);
    const NodeName* new_nodename(const char* ident);
    const NodeList* new_nodelist(const NodeName* base, const std::list<const ListElem*>* elems);
    const ArgName* new_argname(const char* str);
    const ArgValue* new_argvalue();
    const NodeValue* new_nodevalue(const TreeNode* node);
    const ListValue* new_listvalue(const NodeList* list);
    const LiteralValue* new_literalvalue(const Literal* literal);
    const NodeArg* new_nodearg(const ArgName* name, const ArgValue* value);
    const NodeArgs* new_nodeargs();
    const ListArgs* new_listargs(const std::list<const NodeArg*>* list);
    const LiteralArgs* new_literalargs(const Literal* literal);
    const TreeNode* new_treenode(const NodeName* name, const NodeArgs* args);
    const Tree* new_tree(const TreeNode* root);
// < AST_NEW_NODE_DECL

    typedef base_t::node_iterator node_iterator;

    node_iterator node_begin() const { return base_t::node_begin(); }
    node_iterator node_end() const   { return base_t::node_end(); }

    class treenode_iterator;

    treenode_iterator treenode_begin() const;
    treenode_iterator treenode_end() const;

private:
    using base_t::new_node;

    template<typename T>
    typename Arg<root_t, T>::result_t alloc_arg(T arg)
    {
        return Arg<root_t, T>::alloc(this, arg);
    }

// > NODE_LIST_ARGS_FRIENDS $ grammar -NLF
    friend class Arg<root_t, const std::list<const ListElem*>*>;
    friend class Arg<root_t, const std::list<const NodeArg*>*>;
// < NODE_LIST_ARGS_FRIENDS

    struct IsTreeNode;

    size_t n_treenodes;
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

    unsigned short id() const { return n; };

    unsigned short tag() const { return t; };
    void tag(unsigned short _t) const { t = _t; };

private:
    friend class BaseAST<Node>;

    unsigned short n;
    mutable
    unsigned short t;
};

class Literal : public Node
{
public:
    Literal(const char* _str) :
        str(_str)
    {}

    AST::node_t::type_t type() const { return AST::node_t::Literal; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const char* str;
};

class ListElem : public Node
{
public:
    ListElem(const list_elem_idx_t _index, const TreeNode* _node) :
        index(_index),
        node(_node)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ListElem; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const list_elem_idx_t index;
    const TreeNode* node;
};

class NodeName : public Node
{
public:
    NodeName(const char* _ident) :
        ident(_ident)
    {}

    AST::node_t::type_t type() const { return AST::node_t::NodeName; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const char* ident;
};

class NodeList : public Node
{
public:
    NodeList(const NodeName* _base, const list_elem_list_t _elems) :
        base(_base),
        elems(_elems)
    {}

    AST::node_t::type_t type() const { return AST::node_t::NodeList; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const NodeName* base;
    const list_elem_list_t elems;
};

class ArgName : public Node
{
public:
    ArgName(const char* _str) :
        str(_str)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ArgName; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const char* str;
};

class ArgValue : public Node
{
public:
    ArgValue() {}

    AST::node_t::type_t type() const { return AST::node_t::ArgValue; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class NodeValue : public ArgValue
{
public:
    NodeValue(const TreeNode* _node) :
        node(_node)
    {}

    AST::node_t::type_t type() const { return AST::node_t::NodeValue; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const TreeNode* node;
};

class ListValue : public ArgValue
{
public:
    ListValue(const NodeList* _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ListValue; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const NodeList* list;
};

class LiteralValue : public ArgValue
{
public:
    LiteralValue(const Literal* _literal) :
        literal(_literal)
    {}

    AST::node_t::type_t type() const { return AST::node_t::LiteralValue; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Literal* literal;
};

class NodeArg : public Node
{
public:
    NodeArg(const ArgName* _name, const ArgValue* _value) :
        name(_name),
        value(_value)
    {}

    AST::node_t::type_t type() const { return AST::node_t::NodeArg; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const ArgName* name;
    const ArgValue* value;
};

class NodeArgs : public Node
{
public:
    NodeArgs() {}

    AST::node_t::type_t type() const { return AST::node_t::NodeArgs; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class ListArgs : public NodeArgs
{
public:
    ListArgs(const node_arg_list_t _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ListArgs; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const node_arg_list_t list;
};

class LiteralArgs : public NodeArgs
{
public:
    LiteralArgs(const Literal* _literal) :
        literal(_literal)
    {}

    AST::node_t::type_t type() const { return AST::node_t::LiteralArgs; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Literal* literal;
};

class TreeNode : public Node
{
public:
    TreeNode(const NodeName* _name, const NodeArgs* _args) :
        name(_name),
        args(_args)
    {}

    AST::node_t::type_t type() const { return AST::node_t::TreeNode; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const NodeName* name;
    const NodeArgs* args;
};

class Tree : public Node
{
public:
    Tree(const TreeNode* _root) :
        root(_root)
    {}

    AST::node_t::type_t type() const { return AST::node_t::Tree; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const TreeNode* root;
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
struct NodeCast<Literal>
{
    static const AST::node_t::type_t type = AST::node_t::Literal;
};

template<>
struct NodeCast<ListElem>
{
    static const AST::node_t::type_t type = AST::node_t::ListElem;
};

template<>
struct NodeCast<NodeName>
{
    static const AST::node_t::type_t type = AST::node_t::NodeName;
};

template<>
struct NodeCast<NodeList>
{
    static const AST::node_t::type_t type = AST::node_t::NodeList;
};

template<>
struct NodeCast<ArgName>
{
    static const AST::node_t::type_t type = AST::node_t::ArgName;
};

template<>
struct NodeCast<ArgValue>
{
    static const AST::node_t::type_t type = AST::node_t::ArgValue;
};

template<>
struct NodeCast<NodeValue>
{
    static const AST::node_t::type_t type = AST::node_t::NodeValue;
};

template<>
struct NodeCast<ListValue>
{
    static const AST::node_t::type_t type = AST::node_t::ListValue;
};

template<>
struct NodeCast<LiteralValue>
{
    static const AST::node_t::type_t type = AST::node_t::LiteralValue;
};

template<>
struct NodeCast<NodeArg>
{
    static const AST::node_t::type_t type = AST::node_t::NodeArg;
};

template<>
struct NodeCast<NodeArgs>
{
    static const AST::node_t::type_t type = AST::node_t::NodeArgs;
};

template<>
struct NodeCast<ListArgs>
{
    static const AST::node_t::type_t type = AST::node_t::ListArgs;
};

template<>
struct NodeCast<LiteralArgs>
{
    static const AST::node_t::type_t type = AST::node_t::LiteralArgs;
};

template<>
struct NodeCast<TreeNode>
{
    static const AST::node_t::type_t type = AST::node_t::TreeNode;
};

template<>
struct NodeCast<Tree>
{
    static const AST::node_t::type_t type = AST::node_t::Tree;
};
// < NODE_CAST_DECL

class Visitor0
{
public:
    virtual ~Visitor0();

// > VISITOR0_DECL $ grammar -VD=0 -v0result_t
    virtual result_t visit0(const Node*);
    virtual result_t visit0(const Literal*) = 0;
    virtual result_t visit0(const ListElem*) = 0;
    virtual result_t visit0(const NodeName*) = 0;
    virtual result_t visit0(const NodeList*) = 0;
    virtual result_t visit0(const ArgName*) = 0;
    virtual result_t visit0(const ArgValue*) = 0;
    virtual result_t visit0(const NodeValue*) = 0;
    virtual result_t visit0(const ListValue*) = 0;
    virtual result_t visit0(const LiteralValue*) = 0;
    virtual result_t visit0(const NodeArg*) = 0;
    virtual result_t visit0(const NodeArgs*) = 0;
    virtual result_t visit0(const ListArgs*) = 0;
    virtual result_t visit0(const LiteralArgs*) = 0;
    virtual result_t visit0(const TreeNode*) = 0;
    virtual result_t visit0(const Tree*) = 0;
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
    virtual T visit(const Literal*) = 0;
    virtual T visit(const ListElem*) = 0;
    virtual T visit(const NodeName*) = 0;
    virtual T visit(const NodeList*) = 0;
    virtual T visit(const ArgName*) = 0;
    virtual T visit(const ArgValue*) = 0;
    virtual T visit(const NodeValue*) = 0;
    virtual T visit(const ListValue*) = 0;
    virtual T visit(const LiteralValue*) = 0;
    virtual T visit(const NodeArg*) = 0;
    virtual T visit(const NodeArgs*) = 0;
    virtual T visit(const ListArgs*) = 0;
    virtual T visit(const LiteralArgs*) = 0;
    virtual T visit(const TreeNode*) = 0;
    virtual T visit(const Tree*) = 0;
// < EVAL_VISITOR_VISITOR_DECL

private:
// > EVAL_VISITOR_VISITOR0_DECL $ grammar -VD -v0result_t
    result_t visit0(const Literal*);
    result_t visit0(const ListElem*);
    result_t visit0(const NodeName*);
    result_t visit0(const NodeList*);
    result_t visit0(const ArgName*);
    result_t visit0(const ArgValue*);
    result_t visit0(const NodeValue*);
    result_t visit0(const ListValue*);
    result_t visit0(const LiteralValue*);
    result_t visit0(const NodeArg*);
    result_t visit0(const NodeArgs*);
    result_t visit0(const ListArgs*);
    result_t visit0(const LiteralArgs*);
    result_t visit0(const TreeNode*);
    result_t visit0(const Tree*);
// < EVAL_VISITOR_VISITOR0_DECL
};

// > EVAL_VISITOR_VISITOR0_IMPL $ grammar -VEI -v0result_t
template <typename T>
result_t EvalVisitor<T>::visit0(const Literal* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ListElem* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const NodeName* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const NodeList* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ArgName* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ArgValue* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const NodeValue* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ListValue* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const LiteralValue* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const NodeArg* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const NodeArgs* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ListArgs* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const LiteralArgs* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const TreeNode* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const Tree* arg)
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
    virtual void visit(const Literal*) = 0;
    virtual void visit(const ListElem*) = 0;
    virtual void visit(const NodeName*) = 0;
    virtual void visit(const NodeList*) = 0;
    virtual void visit(const ArgName*) = 0;
    virtual void visit(const ArgValue*) = 0;
    virtual void visit(const NodeValue*) = 0;
    virtual void visit(const ListValue*) = 0;
    virtual void visit(const LiteralValue*) = 0;
    virtual void visit(const NodeArg*) = 0;
    virtual void visit(const NodeArgs*) = 0;
    virtual void visit(const ListArgs*) = 0;
    virtual void visit(const LiteralArgs*) = 0;
    virtual void visit(const TreeNode*) = 0;
    virtual void visit(const Tree*) = 0;
// < VISITOR_DECL
};

struct AST::IsTreeNode : Ext::unary_func_t<bool, const Node*>
{
    bool operator()(const Node* node) const
    {
        return node->type() == node_t::TreeNode;
    }
};

class AST::treenode_iterator :
    public std::iterator<std::input_iterator_tag, const TreeNode>,
    private AST::op_node_iterator<AST::IsTreeNode>
{
public:
    typedef AST::op_node_iterator<AST::IsTreeNode> base_t;

    using base_t::operator++;

    bool operator==(const treenode_iterator& v) const
    { return base_t::operator==(v); }
    bool operator!=(const treenode_iterator& v) const
    { return base_t::operator!=(v); }

    const TreeNode* operator*() const
    { return static_cast<const TreeNode*>(*ptr); }
    const TreeNode* operator->() const
    { return static_cast<const TreeNode*>(*ptr); }

private:
    treenode_iterator(const node_iterator& _ptr, const node_iterator& _end) :
        base_t(_ptr, _end)
    {}
    treenode_iterator(const node_iterator& _ptr) :
        base_t(_ptr)
    {}
    friend class AST;
};

inline AST::treenode_iterator AST::treenode_begin() const
{
    return treenode_iterator(node_begin(), node_end());
}

inline AST::treenode_iterator AST::treenode_end() const
{
    return treenode_iterator(node_end());
}

} // namespace TreeAST

namespace Ext {

template<>
class ReprPrint<TreeAST::list_elem_idx_t>
{
public:
    explicit ReprPrint(std::ostream& _ost) :
        ost(_ost)
    {}

    void operator()(TreeAST::list_elem_idx_t idx)
    { ost << '\'' << integer_cast<short>(idx) << '\''; }

private:
    std::ostream& ost;
};

template<>
class Repr<TreeAST::list_elem_idx_t>
{
public:
    Repr(TreeAST::list_elem_idx_t _idx) :
        idx(_idx)
    {}

    void print(std::ostream& ost) const
    { ReprPrint<TreeAST::list_elem_idx_t>{ost}(idx); }

private:
    TreeAST::list_elem_idx_t idx;
};

inline Repr<TreeAST::list_elem_idx_t> repr(TreeAST::list_elem_idx_t idx)
{ return Repr<TreeAST::list_elem_idx_t>(idx); }

} // namespace Ext

#endif/*__TREE_AST_HPP*/

