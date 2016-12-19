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

#ifndef __AST_HPP
#define __AST_HPP

#include "config.h"

#include <cstddef>

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
class Ident;
class Str;
class Expr;
class PrimaryExpr;
class IdentExpr;
class StrExpr;
class ParenExpr;
class DictExpr;
class EmptyDictExpr;
class KeyDatumDictExpr;
class KeyDatum;
class KeyDatumList;
class AttrRef;
class CallArg;
class CallArgList;
class CallExpr;
class EmptyCallExpr;
class ArgsCallExpr;
class OrTest;
class AndTest;
class NotTest;
class Comp;
class CompExpr;
class Stmt;
class SimpleStmt;
class ExprStmt;
class AssertStmt;
class AssertStmt1;
class AssertStmt2;
class Target;
class IdentTarget;
class AttrRefTarget;
class TargetList;
class AssignStmt;
class DelStmt;
class ExprList;
class PrintStmt;
class SimpleStmtList;
class StmtList;
class FileInput;
class InteractInput;
class EvalInput;
class InputInput;
// < NODE_CLASSES_FORWARD

// > NODE_LIST_ARGS $ grammar -NLD
typedef std::pair<size_t, CallArg const* const*> call_arg_list_t;
typedef std::pair<size_t, Comp const* const*> comp_list_t;
typedef std::pair<size_t, Expr const* const*> expr_list_t;
typedef std::pair<size_t, KeyDatum const* const*> key_datum_list_t;
typedef std::pair<size_t, SimpleStmt const* const*> simple_stmt_list_t;
typedef std::pair<size_t, Stmt const* const*> stmt_list_t;
typedef std::pair<size_t, Target const* const*> target_list_t;
// < NODE_LIST_ARGS

class AST : private BaseAST<Node>
{
public:
    typedef BaseAST<Node> base_t;

    enum tree_type_t {
        stmts_tree,
        expr_tree
    };

    struct node_t :
        public base_enum_t<node_t>
    {
        enum type_t {
// > AST_NODE_ENUM $ grammar -NE
            Node,
            Ident,
            Str,
            Expr,
            PrimaryExpr,
            IdentExpr,
            StrExpr,
            ParenExpr,
            DictExpr,
            EmptyDictExpr,
            KeyDatumDictExpr,
            KeyDatum,
            KeyDatumList,
            AttrRef,
            CallArg,
            CallArgList,
            CallExpr,
            EmptyCallExpr,
            ArgsCallExpr,
            OrTest,
            AndTest,
            NotTest,
            Comp,
            CompExpr,
            Stmt,
            SimpleStmt,
            ExprStmt,
            AssertStmt,
            AssertStmt1,
            AssertStmt2,
            Target,
            IdentTarget,
            AttrRefTarget,
            TargetList,
            AssignStmt,
            DelStmt,
            ExprList,
            PrintStmt,
            SimpleStmtList,
            StmtList,
            FileInput,
            InteractInput,
            EvalInput,
            InputInput,
// < AST_NODE_ENUM
        };

    private:
        friend class base_enum_t<node_t>;
        static const char* names[44];
    };

    struct comp_op_t :
        public base_enum_t<comp_op_t>
    {
        enum type_t {
            equal,     // "=="
            not_equal, // "!="
            is,        // "is"
            is_not     // "is" "not"
        };

    private:
        friend class base_enum_t<comp_op_t>;
        static const char* names[4];
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
    const Ident* new_ident(const char* ident);
    const Str* new_str(const char* str);
    const Expr* new_expr();
    const PrimaryExpr* new_primaryexpr();
    const IdentExpr* new_identexpr(const Ident* ident);
    const StrExpr* new_strexpr(const Str* str);
    const ParenExpr* new_parenexpr(const Expr* expr);
    const DictExpr* new_dictexpr();
    const EmptyDictExpr* new_emptydictexpr();
    const KeyDatumDictExpr* new_keydatumdictexpr(const KeyDatumList* key_datums);
    const KeyDatum* new_keydatum(const Expr* expr1, const Expr* expr2);
    const KeyDatumList* new_keydatumlist(const std::list<const KeyDatum*>* list);
    const AttrRef* new_attrref(const PrimaryExpr* expr, const IdentExpr* ident);
    const CallArg* new_callarg(const Expr* expr);
    const CallArgList* new_callarglist(const std::list<const CallArg*>* list);
    const CallExpr* new_callexpr();
    const EmptyCallExpr* new_emptycallexpr(const PrimaryExpr* expr);
    const ArgsCallExpr* new_argscallexpr(const PrimaryExpr* expr, const CallArgList* args);
    const OrTest* new_ortest(const Expr* expr1, const Expr* expr2);
    const AndTest* new_andtest(const Expr* expr1, const Expr* expr2);
    const NotTest* new_nottest(const Expr* expr);
    const Comp* new_comp(const AST::comp_op_t::type_t op, const PrimaryExpr* prim);
    const CompExpr* new_compexpr(const PrimaryExpr* prim, const std::list<const Comp*>* comps);
    const Stmt* new_stmt();
    const SimpleStmt* new_simplestmt();
    const ExprStmt* new_exprstmt(const Expr* expr);
    const AssertStmt* new_assertstmt();
    const AssertStmt1* new_assertstmt1(const Expr* expr1);
    const AssertStmt2* new_assertstmt2(const Expr* expr1, const Expr* expr2);
    const Target* new_target();
    const IdentTarget* new_identtarget(const IdentExpr* ident);
    const AttrRefTarget* new_attrreftarget(const AttrRef* attr);
    const TargetList* new_targetlist(const std::list<const Target*>* list);
    const AssignStmt* new_assignstmt(const TargetList* targets, const Expr* expr);
    const DelStmt* new_delstmt(const TargetList* targets);
    const ExprList* new_exprlist(const std::list<const Expr*>* list);
    const PrintStmt* new_printstmt(const ExprList* exprs, const bool trail_comma);
    const SimpleStmtList* new_simplestmtlist(const std::list<const SimpleStmt*>* list);
    const StmtList* new_stmtlist(const std::list<const Stmt*>* list);
    const FileInput* new_fileinput(const StmtList* stmts);
    const InteractInput* new_interactinput(const SimpleStmtList* stmts);
    const EvalInput* new_evalinput(const Expr* expr);
    const InputInput* new_inputinput(const Expr* expr);
// < AST_NEW_NODE_DECL

// > AST_NEW_NODE_LIST_ARGS_DECL $ grammar -NNLD
    const KeyDatumList* new_keydatumlist(const key_datum_list_t list);
    const CallArgList* new_callarglist(const call_arg_list_t list);
    const CompExpr* new_compexpr(const PrimaryExpr* prim, const comp_list_t comps);
    const TargetList* new_targetlist(const target_list_t list);
    const ExprList* new_exprlist(const expr_list_t list);
    const SimpleStmtList* new_simplestmtlist(const simple_stmt_list_t list);
    const StmtList* new_stmtlist(const stmt_list_t list);
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
    friend class Arg<root_t, const std::list<const CallArg*>*>;
    friend class Arg<root_t, const std::list<const Comp*>*>;
    friend class Arg<root_t, const std::list<const Expr*>*>;
    friend class Arg<root_t, const std::list<const KeyDatum*>*>;
    friend class Arg<root_t, const std::list<const SimpleStmt*>*>;
    friend class Arg<root_t, const std::list<const Stmt*>*>;
    friend class Arg<root_t, const std::list<const Target*>*>;
// < NODE_LIST_ARGS_FRIENDS

// > NODE_LIST_TYPED_ARGS_FRIENDS $ grammar -NLT
    friend class Arg<root_t, call_arg_list_t>;
    friend class Arg<root_t, comp_list_t>;
    friend class Arg<root_t, expr_list_t>;
    friend class Arg<root_t, key_datum_list_t>;
    friend class Arg<root_t, simple_stmt_list_t>;
    friend class Arg<root_t, stmt_list_t>;
    friend class Arg<root_t, target_list_t>;
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

    unsigned short id() const { return n; };

    unsigned short tag() const { return t; };
    void tag(unsigned short _t) const { t = _t; };

private:
    friend class BaseAST<Node>;

    unsigned short n;
    mutable
    unsigned short t;
};

class Ident : public Node
{
public:
    Ident(const char* _ident) :
        ident(_ident)
    {}

    AST::node_t::type_t type() const { return AST::node_t::Ident; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const char* ident;
};

class Str : public Node
{
public:
    Str(const char* _str) :
        str(_str)
    {}

    AST::node_t::type_t type() const { return AST::node_t::Str; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const char* str;
};

class Expr : public Node
{
public:
    Expr() {}

    AST::node_t::type_t type() const { return AST::node_t::Expr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class PrimaryExpr : public Expr
{
public:
    PrimaryExpr() {}

    AST::node_t::type_t type() const { return AST::node_t::PrimaryExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class IdentExpr : public PrimaryExpr
{
public:
    IdentExpr(const Ident* _ident) :
        ident(_ident)
    {}

    AST::node_t::type_t type() const { return AST::node_t::IdentExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Ident* ident;
};

class StrExpr : public PrimaryExpr
{
public:
    StrExpr(const Str* _str) :
        str(_str)
    {}

    AST::node_t::type_t type() const { return AST::node_t::StrExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Str* str;
};

class ParenExpr : public PrimaryExpr
{
public:
    ParenExpr(const Expr* _expr) :
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ParenExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr;
};

class DictExpr : public PrimaryExpr
{
public:
    DictExpr() {}

    AST::node_t::type_t type() const { return AST::node_t::DictExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class EmptyDictExpr : public DictExpr
{
public:
    EmptyDictExpr() {}

    AST::node_t::type_t type() const { return AST::node_t::EmptyDictExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class KeyDatumDictExpr : public DictExpr
{
public:
    KeyDatumDictExpr(const KeyDatumList* _key_datums) :
        key_datums(_key_datums)
    {}

    AST::node_t::type_t type() const { return AST::node_t::KeyDatumDictExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const KeyDatumList* key_datums;
};

class KeyDatum : public Node
{
public:
    KeyDatum(const Expr* _expr1, const Expr* _expr2) :
        expr1(_expr1),
        expr2(_expr2)
    {}

    AST::node_t::type_t type() const { return AST::node_t::KeyDatum; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr1;
    const Expr* expr2;
};

class KeyDatumList : public Node
{
public:
    KeyDatumList(const key_datum_list_t _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::KeyDatumList; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const key_datum_list_t list;
};

class AttrRef : public PrimaryExpr
{
public:
    AttrRef(const PrimaryExpr* _expr, const IdentExpr* _ident) :
        expr(_expr),
        ident(_ident)
    {}

    AST::node_t::type_t type() const { return AST::node_t::AttrRef; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const PrimaryExpr* expr;
    const IdentExpr* ident;
};

class CallArg : public Node
{
public:
    CallArg(const Expr* _expr) :
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::CallArg; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr;
};

class CallArgList : public Node
{
public:
    CallArgList(const call_arg_list_t _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::CallArgList; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const call_arg_list_t list;
};

class CallExpr : public PrimaryExpr
{
public:
    CallExpr() {}

    AST::node_t::type_t type() const { return AST::node_t::CallExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class EmptyCallExpr : public CallExpr
{
public:
    EmptyCallExpr(const PrimaryExpr* _expr) :
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::EmptyCallExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const PrimaryExpr* expr;
};

class ArgsCallExpr : public CallExpr
{
public:
    ArgsCallExpr(const PrimaryExpr* _expr, const CallArgList* _args) :
        expr(_expr),
        args(_args)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ArgsCallExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const PrimaryExpr* expr;
    const CallArgList* args;
};

class OrTest : public Expr
{
public:
    OrTest(const Expr* _expr1, const Expr* _expr2) :
        expr1(_expr1),
        expr2(_expr2)
    {}

    AST::node_t::type_t type() const { return AST::node_t::OrTest; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr1;
    const Expr* expr2;
};

class AndTest : public Expr
{
public:
    AndTest(const Expr* _expr1, const Expr* _expr2) :
        expr1(_expr1),
        expr2(_expr2)
    {}

    AST::node_t::type_t type() const { return AST::node_t::AndTest; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr1;
    const Expr* expr2;
};

class NotTest : public Expr
{
public:
    NotTest(const Expr* _expr) :
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::NotTest; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr;
};

class Comp : public Node
{
public:
    Comp(const AST::comp_op_t::type_t _op, const PrimaryExpr* _prim) :
        op(_op),
        prim(_prim)
    {}

    AST::node_t::type_t type() const { return AST::node_t::Comp; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const AST::comp_op_t::type_t op;
    const PrimaryExpr* prim;
};

class CompExpr : public Expr
{
public:
    CompExpr(const PrimaryExpr* _prim, const comp_list_t _comps) :
        prim(_prim),
        comps(_comps)
    {}

    AST::node_t::type_t type() const { return AST::node_t::CompExpr; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const PrimaryExpr* prim;
    const comp_list_t comps;
};

class Stmt : public Node
{
public:
    Stmt() {}

    AST::node_t::type_t type() const { return AST::node_t::Stmt; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class SimpleStmt : public Stmt
{
public:
    SimpleStmt() {}

    AST::node_t::type_t type() const { return AST::node_t::SimpleStmt; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class ExprStmt : public SimpleStmt
{
public:
    ExprStmt(const Expr* _expr) :
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ExprStmt; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr;
};

class AssertStmt : public SimpleStmt
{
public:
    AssertStmt() {}

    AST::node_t::type_t type() const { return AST::node_t::AssertStmt; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class AssertStmt1 : public AssertStmt
{
public:
    AssertStmt1(const Expr* _expr1) :
        expr1(_expr1)
    {}

    AST::node_t::type_t type() const { return AST::node_t::AssertStmt1; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr1;
};

class AssertStmt2 : public AssertStmt
{
public:
    AssertStmt2(const Expr* _expr1, const Expr* _expr2) :
        expr1(_expr1),
        expr2(_expr2)
    {}

    AST::node_t::type_t type() const { return AST::node_t::AssertStmt2; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr1;
    const Expr* expr2;
};

class Target : public Node
{
public:
    Target() {}

    AST::node_t::type_t type() const { return AST::node_t::Target; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;
};

class IdentTarget : public Target
{
public:
    IdentTarget(const IdentExpr* _ident) :
        ident(_ident)
    {}

    AST::node_t::type_t type() const { return AST::node_t::IdentTarget; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const IdentExpr* ident;
};

class AttrRefTarget : public Target
{
public:
    AttrRefTarget(const AttrRef* _attr) :
        attr(_attr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::AttrRefTarget; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const AttrRef* attr;
};

class TargetList : public Node
{
public:
    TargetList(const target_list_t _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::TargetList; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const target_list_t list;
};

class AssignStmt : public SimpleStmt
{
public:
    AssignStmt(const TargetList* _targets, const Expr* _expr) :
        targets(_targets),
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::AssignStmt; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const TargetList* targets;
    const Expr* expr;
};

class DelStmt : public SimpleStmt
{
public:
    DelStmt(const TargetList* _targets) :
        targets(_targets)
    {}

    AST::node_t::type_t type() const { return AST::node_t::DelStmt; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const TargetList* targets;
};

class ExprList : public Node
{
public:
    ExprList(const expr_list_t _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::ExprList; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const expr_list_t list;
};

class PrintStmt : public SimpleStmt
{
public:
    PrintStmt(const ExprList* _exprs, const bool _trail_comma) :
        exprs(_exprs),
        trail_comma(_trail_comma)
    {}

    AST::node_t::type_t type() const { return AST::node_t::PrintStmt; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const ExprList* exprs;
    const bool trail_comma;
};

class SimpleStmtList : public Stmt
{
public:
    SimpleStmtList(const simple_stmt_list_t _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::SimpleStmtList; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const simple_stmt_list_t list;
};

class StmtList : public Stmt
{
public:
    StmtList(const stmt_list_t _list) :
        list(_list)
    {}

    AST::node_t::type_t type() const { return AST::node_t::StmtList; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const stmt_list_t list;
};

class FileInput : public Node
{
public:
    FileInput(const StmtList* _stmts) :
        stmts(_stmts)
    {}

    AST::node_t::type_t type() const { return AST::node_t::FileInput; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const StmtList* stmts;
};

class InteractInput : public Node
{
public:
    InteractInput(const SimpleStmtList* _stmts) :
        stmts(_stmts)
    {}

    AST::node_t::type_t type() const { return AST::node_t::InteractInput; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const SimpleStmtList* stmts;
};

class EvalInput : public Node
{
public:
    EvalInput(const Expr* _expr) :
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::EvalInput; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr;
};

class InputInput : public Node
{
public:
    InputInput(const Expr* _expr) :
        expr(_expr)
    {}

    AST::node_t::type_t type() const { return AST::node_t::InputInput; }

    result_t accept(Visitor0*) const;
    void accept(Visitor*) const;

    const Expr* expr;
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
struct NodeCast<Ident>
{
    static const AST::node_t::type_t type = AST::node_t::Ident;
};

template<>
struct NodeCast<Str>
{
    static const AST::node_t::type_t type = AST::node_t::Str;
};

template<>
struct NodeCast<Expr>
{
    static const AST::node_t::type_t type = AST::node_t::Expr;
};

template<>
struct NodeCast<PrimaryExpr>
{
    static const AST::node_t::type_t type = AST::node_t::PrimaryExpr;
};

template<>
struct NodeCast<IdentExpr>
{
    static const AST::node_t::type_t type = AST::node_t::IdentExpr;
};

template<>
struct NodeCast<StrExpr>
{
    static const AST::node_t::type_t type = AST::node_t::StrExpr;
};

template<>
struct NodeCast<ParenExpr>
{
    static const AST::node_t::type_t type = AST::node_t::ParenExpr;
};

template<>
struct NodeCast<DictExpr>
{
    static const AST::node_t::type_t type = AST::node_t::DictExpr;
};

template<>
struct NodeCast<EmptyDictExpr>
{
    static const AST::node_t::type_t type = AST::node_t::EmptyDictExpr;
};

template<>
struct NodeCast<KeyDatumDictExpr>
{
    static const AST::node_t::type_t type = AST::node_t::KeyDatumDictExpr;
};

template<>
struct NodeCast<KeyDatum>
{
    static const AST::node_t::type_t type = AST::node_t::KeyDatum;
};

template<>
struct NodeCast<KeyDatumList>
{
    static const AST::node_t::type_t type = AST::node_t::KeyDatumList;
};

template<>
struct NodeCast<AttrRef>
{
    static const AST::node_t::type_t type = AST::node_t::AttrRef;
};

template<>
struct NodeCast<CallArg>
{
    static const AST::node_t::type_t type = AST::node_t::CallArg;
};

template<>
struct NodeCast<CallArgList>
{
    static const AST::node_t::type_t type = AST::node_t::CallArgList;
};

template<>
struct NodeCast<CallExpr>
{
    static const AST::node_t::type_t type = AST::node_t::CallExpr;
};

template<>
struct NodeCast<EmptyCallExpr>
{
    static const AST::node_t::type_t type = AST::node_t::EmptyCallExpr;
};

template<>
struct NodeCast<ArgsCallExpr>
{
    static const AST::node_t::type_t type = AST::node_t::ArgsCallExpr;
};

template<>
struct NodeCast<OrTest>
{
    static const AST::node_t::type_t type = AST::node_t::OrTest;
};

template<>
struct NodeCast<AndTest>
{
    static const AST::node_t::type_t type = AST::node_t::AndTest;
};

template<>
struct NodeCast<NotTest>
{
    static const AST::node_t::type_t type = AST::node_t::NotTest;
};

template<>
struct NodeCast<Comp>
{
    static const AST::node_t::type_t type = AST::node_t::Comp;
};

template<>
struct NodeCast<CompExpr>
{
    static const AST::node_t::type_t type = AST::node_t::CompExpr;
};

template<>
struct NodeCast<Stmt>
{
    static const AST::node_t::type_t type = AST::node_t::Stmt;
};

template<>
struct NodeCast<SimpleStmt>
{
    static const AST::node_t::type_t type = AST::node_t::SimpleStmt;
};

template<>
struct NodeCast<ExprStmt>
{
    static const AST::node_t::type_t type = AST::node_t::ExprStmt;
};

template<>
struct NodeCast<AssertStmt>
{
    static const AST::node_t::type_t type = AST::node_t::AssertStmt;
};

template<>
struct NodeCast<AssertStmt1>
{
    static const AST::node_t::type_t type = AST::node_t::AssertStmt1;
};

template<>
struct NodeCast<AssertStmt2>
{
    static const AST::node_t::type_t type = AST::node_t::AssertStmt2;
};

template<>
struct NodeCast<Target>
{
    static const AST::node_t::type_t type = AST::node_t::Target;
};

template<>
struct NodeCast<IdentTarget>
{
    static const AST::node_t::type_t type = AST::node_t::IdentTarget;
};

template<>
struct NodeCast<AttrRefTarget>
{
    static const AST::node_t::type_t type = AST::node_t::AttrRefTarget;
};

template<>
struct NodeCast<TargetList>
{
    static const AST::node_t::type_t type = AST::node_t::TargetList;
};

template<>
struct NodeCast<AssignStmt>
{
    static const AST::node_t::type_t type = AST::node_t::AssignStmt;
};

template<>
struct NodeCast<DelStmt>
{
    static const AST::node_t::type_t type = AST::node_t::DelStmt;
};

template<>
struct NodeCast<ExprList>
{
    static const AST::node_t::type_t type = AST::node_t::ExprList;
};

template<>
struct NodeCast<PrintStmt>
{
    static const AST::node_t::type_t type = AST::node_t::PrintStmt;
};

template<>
struct NodeCast<SimpleStmtList>
{
    static const AST::node_t::type_t type = AST::node_t::SimpleStmtList;
};

template<>
struct NodeCast<StmtList>
{
    static const AST::node_t::type_t type = AST::node_t::StmtList;
};

template<>
struct NodeCast<FileInput>
{
    static const AST::node_t::type_t type = AST::node_t::FileInput;
};

template<>
struct NodeCast<InteractInput>
{
    static const AST::node_t::type_t type = AST::node_t::InteractInput;
};

template<>
struct NodeCast<EvalInput>
{
    static const AST::node_t::type_t type = AST::node_t::EvalInput;
};

template<>
struct NodeCast<InputInput>
{
    static const AST::node_t::type_t type = AST::node_t::InputInput;
};
// < NODE_CAST_DECL

class Visitor0
{
public:
    virtual ~Visitor0();

// > VISITOR0_DECL $ grammar -VD=0 -v0result_t
    virtual result_t visit0(const Node*);
    virtual result_t visit0(const Ident*) = 0;
    virtual result_t visit0(const Str*) = 0;
    virtual result_t visit0(const Expr*) = 0;
    virtual result_t visit0(const PrimaryExpr*) = 0;
    virtual result_t visit0(const IdentExpr*) = 0;
    virtual result_t visit0(const StrExpr*) = 0;
    virtual result_t visit0(const ParenExpr*) = 0;
    virtual result_t visit0(const DictExpr*) = 0;
    virtual result_t visit0(const EmptyDictExpr*) = 0;
    virtual result_t visit0(const KeyDatumDictExpr*) = 0;
    virtual result_t visit0(const KeyDatum*) = 0;
    virtual result_t visit0(const KeyDatumList*) = 0;
    virtual result_t visit0(const AttrRef*) = 0;
    virtual result_t visit0(const CallArg*) = 0;
    virtual result_t visit0(const CallArgList*) = 0;
    virtual result_t visit0(const CallExpr*) = 0;
    virtual result_t visit0(const EmptyCallExpr*) = 0;
    virtual result_t visit0(const ArgsCallExpr*) = 0;
    virtual result_t visit0(const OrTest*) = 0;
    virtual result_t visit0(const AndTest*) = 0;
    virtual result_t visit0(const NotTest*) = 0;
    virtual result_t visit0(const Comp*) = 0;
    virtual result_t visit0(const CompExpr*) = 0;
    virtual result_t visit0(const Stmt*) = 0;
    virtual result_t visit0(const SimpleStmt*) = 0;
    virtual result_t visit0(const ExprStmt*) = 0;
    virtual result_t visit0(const AssertStmt*) = 0;
    virtual result_t visit0(const AssertStmt1*) = 0;
    virtual result_t visit0(const AssertStmt2*) = 0;
    virtual result_t visit0(const Target*) = 0;
    virtual result_t visit0(const IdentTarget*) = 0;
    virtual result_t visit0(const AttrRefTarget*) = 0;
    virtual result_t visit0(const TargetList*) = 0;
    virtual result_t visit0(const AssignStmt*) = 0;
    virtual result_t visit0(const DelStmt*) = 0;
    virtual result_t visit0(const ExprList*) = 0;
    virtual result_t visit0(const PrintStmt*) = 0;
    virtual result_t visit0(const SimpleStmtList*) = 0;
    virtual result_t visit0(const StmtList*) = 0;
    virtual result_t visit0(const FileInput*) = 0;
    virtual result_t visit0(const InteractInput*) = 0;
    virtual result_t visit0(const EvalInput*) = 0;
    virtual result_t visit0(const InputInput*) = 0;
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
    virtual T visit(const Ident*) = 0;
    virtual T visit(const Str*) = 0;
    virtual T visit(const Expr*) = 0;
    virtual T visit(const PrimaryExpr*) = 0;
    virtual T visit(const IdentExpr*) = 0;
    virtual T visit(const StrExpr*) = 0;
    virtual T visit(const ParenExpr*) = 0;
    virtual T visit(const DictExpr*) = 0;
    virtual T visit(const EmptyDictExpr*) = 0;
    virtual T visit(const KeyDatumDictExpr*) = 0;
    virtual T visit(const KeyDatum*) = 0;
    virtual T visit(const KeyDatumList*) = 0;
    virtual T visit(const AttrRef*) = 0;
    virtual T visit(const CallArg*) = 0;
    virtual T visit(const CallArgList*) = 0;
    virtual T visit(const CallExpr*) = 0;
    virtual T visit(const EmptyCallExpr*) = 0;
    virtual T visit(const ArgsCallExpr*) = 0;
    virtual T visit(const OrTest*) = 0;
    virtual T visit(const AndTest*) = 0;
    virtual T visit(const NotTest*) = 0;
    virtual T visit(const Comp*) = 0;
    virtual T visit(const CompExpr*) = 0;
    virtual T visit(const Stmt*) = 0;
    virtual T visit(const SimpleStmt*) = 0;
    virtual T visit(const ExprStmt*) = 0;
    virtual T visit(const AssertStmt*) = 0;
    virtual T visit(const AssertStmt1*) = 0;
    virtual T visit(const AssertStmt2*) = 0;
    virtual T visit(const Target*) = 0;
    virtual T visit(const IdentTarget*) = 0;
    virtual T visit(const AttrRefTarget*) = 0;
    virtual T visit(const TargetList*) = 0;
    virtual T visit(const AssignStmt*) = 0;
    virtual T visit(const DelStmt*) = 0;
    virtual T visit(const ExprList*) = 0;
    virtual T visit(const PrintStmt*) = 0;
    virtual T visit(const SimpleStmtList*) = 0;
    virtual T visit(const StmtList*) = 0;
    virtual T visit(const FileInput*) = 0;
    virtual T visit(const InteractInput*) = 0;
    virtual T visit(const EvalInput*) = 0;
    virtual T visit(const InputInput*) = 0;
// < EVAL_VISITOR_VISITOR_DECL

private:
// > EVAL_VISITOR_VISITOR0_DECL $ grammar -VD -v0result_t
    result_t visit0(const Ident*);
    result_t visit0(const Str*);
    result_t visit0(const Expr*);
    result_t visit0(const PrimaryExpr*);
    result_t visit0(const IdentExpr*);
    result_t visit0(const StrExpr*);
    result_t visit0(const ParenExpr*);
    result_t visit0(const DictExpr*);
    result_t visit0(const EmptyDictExpr*);
    result_t visit0(const KeyDatumDictExpr*);
    result_t visit0(const KeyDatum*);
    result_t visit0(const KeyDatumList*);
    result_t visit0(const AttrRef*);
    result_t visit0(const CallArg*);
    result_t visit0(const CallArgList*);
    result_t visit0(const CallExpr*);
    result_t visit0(const EmptyCallExpr*);
    result_t visit0(const ArgsCallExpr*);
    result_t visit0(const OrTest*);
    result_t visit0(const AndTest*);
    result_t visit0(const NotTest*);
    result_t visit0(const Comp*);
    result_t visit0(const CompExpr*);
    result_t visit0(const Stmt*);
    result_t visit0(const SimpleStmt*);
    result_t visit0(const ExprStmt*);
    result_t visit0(const AssertStmt*);
    result_t visit0(const AssertStmt1*);
    result_t visit0(const AssertStmt2*);
    result_t visit0(const Target*);
    result_t visit0(const IdentTarget*);
    result_t visit0(const AttrRefTarget*);
    result_t visit0(const TargetList*);
    result_t visit0(const AssignStmt*);
    result_t visit0(const DelStmt*);
    result_t visit0(const ExprList*);
    result_t visit0(const PrintStmt*);
    result_t visit0(const SimpleStmtList*);
    result_t visit0(const StmtList*);
    result_t visit0(const FileInput*);
    result_t visit0(const InteractInput*);
    result_t visit0(const EvalInput*);
    result_t visit0(const InputInput*);
// < EVAL_VISITOR_VISITOR0_DECL
};

// > EVAL_VISITOR_VISITOR0_IMPL $ grammar -VEI -v0result_t
template <typename T>
result_t EvalVisitor<T>::visit0(const Ident* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const Str* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const Expr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const PrimaryExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const IdentExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const StrExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ParenExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const DictExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const EmptyDictExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const KeyDatumDictExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const KeyDatum* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const KeyDatumList* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const AttrRef* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const CallArg* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const CallArgList* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const CallExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const EmptyCallExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ArgsCallExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const OrTest* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const AndTest* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const NotTest* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const Comp* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const CompExpr* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const Stmt* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const SimpleStmt* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ExprStmt* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const AssertStmt* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const AssertStmt1* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const AssertStmt2* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const Target* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const IdentTarget* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const AttrRefTarget* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const TargetList* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const AssignStmt* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const DelStmt* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const ExprList* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const PrintStmt* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const SimpleStmtList* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const StmtList* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const FileInput* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const InteractInput* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const EvalInput* arg)
{
    return cast_t::cast(visit(arg));
};

template <typename T>
result_t EvalVisitor<T>::visit0(const InputInput* arg)
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
    virtual void visit(const Ident*) = 0;
    virtual void visit(const Str*) = 0;
    virtual void visit(const Expr*) = 0;
    virtual void visit(const PrimaryExpr*) = 0;
    virtual void visit(const IdentExpr*) = 0;
    virtual void visit(const StrExpr*) = 0;
    virtual void visit(const ParenExpr*) = 0;
    virtual void visit(const DictExpr*) = 0;
    virtual void visit(const EmptyDictExpr*) = 0;
    virtual void visit(const KeyDatumDictExpr*) = 0;
    virtual void visit(const KeyDatum*) = 0;
    virtual void visit(const KeyDatumList*) = 0;
    virtual void visit(const AttrRef*) = 0;
    virtual void visit(const CallArg*) = 0;
    virtual void visit(const CallArgList*) = 0;
    virtual void visit(const CallExpr*) = 0;
    virtual void visit(const EmptyCallExpr*) = 0;
    virtual void visit(const ArgsCallExpr*) = 0;
    virtual void visit(const OrTest*) = 0;
    virtual void visit(const AndTest*) = 0;
    virtual void visit(const NotTest*) = 0;
    virtual void visit(const Comp*) = 0;
    virtual void visit(const CompExpr*) = 0;
    virtual void visit(const Stmt*) = 0;
    virtual void visit(const SimpleStmt*) = 0;
    virtual void visit(const ExprStmt*) = 0;
    virtual void visit(const AssertStmt*) = 0;
    virtual void visit(const AssertStmt1*) = 0;
    virtual void visit(const AssertStmt2*) = 0;
    virtual void visit(const Target*) = 0;
    virtual void visit(const IdentTarget*) = 0;
    virtual void visit(const AttrRefTarget*) = 0;
    virtual void visit(const TargetList*) = 0;
    virtual void visit(const AssignStmt*) = 0;
    virtual void visit(const DelStmt*) = 0;
    virtual void visit(const ExprList*) = 0;
    virtual void visit(const PrintStmt*) = 0;
    virtual void visit(const SimpleStmtList*) = 0;
    virtual void visit(const StmtList*) = 0;
    virtual void visit(const FileInput*) = 0;
    virtual void visit(const InteractInput*) = 0;
    virtual void visit(const EvalInput*) = 0;
    virtual void visit(const InputInput*) = 0;
// < VISITOR_DECL
};

} // namespace AST

#endif/*__AST_HPP*/

