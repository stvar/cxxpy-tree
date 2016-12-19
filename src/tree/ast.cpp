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

#include <functional>
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "sys.hpp"
#include "std-ext.hpp"
#include "ast.hpp"

namespace AST {

using TreeAST::error;
using TreeAST::AST;

} // namespace AST

#include "ast-base-impl.hpp"

#define POOL_SIZE_SMALL \
256, 16384, \
256, 16384

#define POOL_SIZE_MEDIUM \
2048, 32768, \
1024, 16384

#define POOL_BIG \
8192, 131072, \
4096, 65536

#define POOL_SIZE POOL_BIG

namespace TreeAST {

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
        POOL_SIZE,
        base_t::node_constr,
        base_t::node_destr),
    n_treenodes(0)
{}

AST::~AST()
{}

const char* AST::node_t::names[16] = {
// > AST_NODE_ENUM_NAMES $ grammar -NS
    "Node",             // node_t::type_t::Node
    "Literal",          // node_t::type_t::Literal
    "ListElem",         // node_t::type_t::ListElem
    "NodeName",         // node_t::type_t::NodeName
    "NodeList",         // node_t::type_t::NodeList
    "ArgName",          // node_t::type_t::ArgName
    "ArgValue",         // node_t::type_t::ArgValue
    "NodeValue",        // node_t::type_t::NodeValue
    "ListValue",        // node_t::type_t::ListValue
    "LiteralValue",     // node_t::type_t::LiteralValue
    "NodeArg",          // node_t::type_t::NodeArg
    "NodeArgs",         // node_t::type_t::NodeArgs
    "ListArgs",         // node_t::type_t::ListArgs
    "LiteralArgs",      // node_t::type_t::LiteralArgs
    "TreeNode",         // node_t::type_t::TreeNode
    "Tree",             // node_t::type_t::Tree
// < AST_NODE_ENUM_NAMES
};

} // namespace TreeAST

namespace AST {

// > NODE_LIST_ARGS_USING_NODES $ grammar -NLUN=TreeAST
using TreeAST::ListElem;
using TreeAST::NodeArg;
// < NODE_LIST_ARGS_USING_NODES

// > NODE_LIST_ARGS_USING_TYPES $ grammar -NLUT=TreeAST
using TreeAST::list_elem_list_t;
using TreeAST::node_arg_list_t;
// < NODE_LIST_ARGS_USING_TYPES

// > NODE_LIST_ARGS_IMPL $ grammar -NLI
template<>
struct ArgList<ListElem>
{
    typedef list_elem_list_t list_t;
};

template<>
struct ArgList<NodeArg>
{
    typedef node_arg_list_t list_t;
};
// < NODE_LIST_ARGS_IMPL

using TreeAST::Node;
using TreeAST::list_elem_idx_t;

template<>
struct Arg<Node, list_elem_idx_t> : public ArgResult<list_elem_idx_t>
{
    static list_elem_idx_t alloc(BaseAST<Node>* self UNUSED, list_elem_idx_t arg)
    { return arg; }
};

} // namespace AST

namespace TreeAST {

void AST::root_node(const Node* node)
{
    Arg<Node>::check(this, node);
    root = node;
    n_treenodes = std::count_if(node_begin(), node_end(), IsTreeNode());
}

// > AST_NEW_NODE_IMPL $ grammar -NNI
const Node* AST::new_node()
{
    return new_node<Node>();
}

const Literal* AST::new_literal(const char* str)
{
    return new_node<Literal>(alloc_arg(str));
}

const ListElem* AST::new_listelem(const list_elem_idx_t index, const TreeNode* node)
{
    return new_node<ListElem>(alloc_arg(index), alloc_arg(node));
}

const NodeName* AST::new_nodename(const char* ident)
{
    return new_node<NodeName>(alloc_arg(ident));
}

const NodeList* AST::new_nodelist(const NodeName* base, const std::list<const ListElem*>* elems)
{
    return new_node<NodeList>(alloc_arg(base), alloc_arg(elems));
}

const ArgName* AST::new_argname(const char* str)
{
    return new_node<ArgName>(alloc_arg(str));
}

const ArgValue* AST::new_argvalue()
{
    return new_node<ArgValue>();
}

const NodeValue* AST::new_nodevalue(const TreeNode* node)
{
    return new_node<NodeValue>(alloc_arg(node));
}

const ListValue* AST::new_listvalue(const NodeList* list)
{
    return new_node<ListValue>(alloc_arg(list));
}

const LiteralValue* AST::new_literalvalue(const Literal* literal)
{
    return new_node<LiteralValue>(alloc_arg(literal));
}

const NodeArg* AST::new_nodearg(const ArgName* name, const ArgValue* value)
{
    return new_node<NodeArg>(alloc_arg(name), alloc_arg(value));
}

const NodeArgs* AST::new_nodeargs()
{
    return new_node<NodeArgs>();
}

const ListArgs* AST::new_listargs(const std::list<const NodeArg*>* list)
{
    return new_node<ListArgs>(alloc_arg(list));
}

const LiteralArgs* AST::new_literalargs(const Literal* literal)
{
    return new_node<LiteralArgs>(alloc_arg(literal));
}

const TreeNode* AST::new_treenode(const NodeName* name, const NodeArgs* args)
{
    return new_node<TreeNode>(alloc_arg(name), alloc_arg(args));
}

const Tree* AST::new_tree(const TreeNode* root)
{
    return new_node<Tree>(alloc_arg(root));
}
// < AST_NEW_NODE_IMPL

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

result_t Literal::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Literal::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ListElem::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ListElem::accept(Visitor* v) const
{
    v->visit(this);
}

result_t NodeName::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void NodeName::accept(Visitor* v) const
{
    v->visit(this);
}

result_t NodeList::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void NodeList::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ArgName::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ArgName::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ArgValue::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ArgValue::accept(Visitor* v) const
{
    v->visit(this);
}

result_t NodeValue::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void NodeValue::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ListValue::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ListValue::accept(Visitor* v) const
{
    v->visit(this);
}

result_t LiteralValue::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void LiteralValue::accept(Visitor* v) const
{
    v->visit(this);
}

result_t NodeArg::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void NodeArg::accept(Visitor* v) const
{
    v->visit(this);
}

result_t NodeArgs::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void NodeArgs::accept(Visitor* v) const
{
    v->visit(this);
}

result_t ListArgs::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void ListArgs::accept(Visitor* v) const
{
    v->visit(this);
}

result_t LiteralArgs::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void LiteralArgs::accept(Visitor* v) const
{
    v->visit(this);
}

result_t TreeNode::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void TreeNode::accept(Visitor* v) const
{
    v->visit(this);
}

result_t Tree::accept(Visitor0* v) const
{
    return v->visit0(this);
}

void Tree::accept(Visitor* v) const
{
    v->visit(this);
}
// < NODE_CLASSES_IMPL

} // namespace TreeAST

