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

#include <iostream>
#include <iomanip>

#include "sys.hpp"
#include "std-ext.hpp"

#include "ast.hpp"

namespace AST {

using namespace TreeAST;
using TreeAST::AST;

} // namespace AST

#include "ast-base-printer.hpp"

#include "options.hpp"
#include "tree.hpp"

static const char extension[] = "tree-printer";

#include "ext-err-impl.hpp"

class options_t : public Opts::options_t
{
public:
    static const options_t* options(const Tree::options_t*);

    typedef Opts::options_t base_t;
    enum {
        print_ast_action = base_t::ext_func_action + 1,
        dump_ast_action,
        pretty_ast_action,
        terse_ast_action,
    };

    bool indent_dots;

    const char* act_name() const;

protected:
    options_t();

    struct opt_type_t : base_t::opt_type_t
    {
        enum {
            print_ast      = 'p',
            dump_ast       = 'u',
            pretty_ast     = 'P',
            terse_ast      = 'T',
            indent_dots    = 'i',
            no_indent_dots = 'I',
        };
    };

    void collect_opts(
        std::vector<char>& short_opts,
        std::vector<struct option>& long_opts) const;
    void parse(const Tree::options_t* parent);
    bool parse_opt(opt_t opt, const char* opt_arg);

    using base_t::noyes;

    void usage_acts() const;
    void usage_opts() const;

    void dump_opts() const;
};

options_t::options_t() :
    indent_dots(false)
{
    action = print_ast_action;
}

void options_t::parse(const Tree::options_t* parent)
{
    base_t::parse(parent->ext_argc, parent->ext_argv);
#ifdef DEBUG
    debug |= parent->debug_ext_func;
#endif
    if (parent->help_ext_func) {
        usage();
        exit(0);
    }
}

const options_t* options_t::options(const Tree::options_t* parent)
{
    static options_t opts;
    opts.parse(parent);
    return &opts;
}

void options_t::collect_opts(
    std::vector<char>& res_short_opts,
    std::vector<struct option>& res_long_opts) const
{
    static const char short_opts[] = "iIpPuT";
    static const auto n_short_opts = Ext::array_size(short_opts) - 1;

    static struct option long_opts[] = {
        { "print-ast",      0, nullptr, opt_type_t::print_ast },
        { "dump-ast",       0, nullptr, opt_type_t::dump_ast },
        { "pretty-ast",     0, nullptr, opt_type_t::pretty_ast },
        { "terse-ast",      0, nullptr, opt_type_t::terse_ast },
        { "indent-dots",    0, nullptr, opt_type_t::indent_dots },
        { "no-indent-dots", 0, nullptr, opt_type_t::no_indent_dots },
    };
    static const auto n_long_opts = Ext::array_size(long_opts);

    base_t::collect_opts(res_short_opts, res_long_opts);
    std::copy(short_opts, short_opts + n_short_opts, back_inserter(res_short_opts));
    std::copy(long_opts, long_opts + n_long_opts, back_inserter(res_long_opts));
}

void options_t::usage_acts() const
{
    base_t::usage_acts();
    using namespace std;	
    cout
        << "  -p|--print-ast       print the given AST (default)" << endl
        << "  -u|--dump-ast        dump the given AST" << endl
        << "  -P|--pretty-ast      print prettily the given AST" << endl
        << "  -T|--terse-ast       print tersely the given AST" << endl;
}

void options_t::usage_opts() const
{
    base_t::usage_opts();
    using namespace std;
    cout
        << "  -i|--indent-dots     print or not indenting dots (default not)" << endl
        << "  -I|--no-indent-dots" << endl;
}

void options_t::dump_opts() const
{
    base_t::dump_opts();
    using namespace std;
    cout
        << "indent-dots:    " << Ext::array(noyes)[indent_dots] << endl;
}

const char* options_t::act_name() const
{
    static char const* actions[] = {
        "print-ast",  // print_ast_action
        "dump-ast",   // dump_ast_action
        "pretty-ast", // pretty_ast_action
        "terse-ast",  // terse_ast_action
    };
    return action >= print_ast_action
        ? Ext::array(actions)[action - print_ast_action]
        : base_t::act_name();
}

bool options_t::parse_opt(opt_t opt, const char* opt_arg)
{
    if (base_t::parse_opt(opt, opt_arg))
        return true;
    switch (opt) {
    case opt_type_t::print_ast:
        action = print_ast_action;
        break;
    case opt_type_t::dump_ast:
        action = dump_ast_action;
        break;
    case opt_type_t::pretty_ast:
        action = pretty_ast_action;
        break;
    case opt_type_t::terse_ast:
        action = terse_ast_action;
        break;
    case opt_type_t::indent_dots:
        indent_dots = true;
        break;
    case opt_type_t::no_indent_dots:
        indent_dots = false;
        break;
    default:
        return false;
    }
    return true;
}

namespace AST {

struct DotsEndlPrint
{
    DotsEndlPrint(short _level, bool _dots) :
        level(_level), dots(_dots)
    {}

    void print(std::ostream& ost) const
    {
        static const char* indent[] = {
            "    ", ".   "
        };
        ost << std::endl;
        if (!level) return;
        auto ind =
            Ext::array(indent)[dots];
        while (level --)
            ost << ind;
    }

    mutable short level;
    bool dots;
};

inline std::ostream& operator<<(std::ostream& ost, const DotsEndlPrint& obj)
{ obj.print(ost); return ost; }

class BasicPrinter :
    protected BasePrinter<Node, Visitor>,
    private Visitor
{
protected:
    typedef BasePrinter<Node, Visitor> base_t;

    BasicPrinter(std::ostream& _ost, const options_t* _opt, bool _verbose = true) :
        base_t(_ost, this),
        opt(_opt),
        verbose(_verbose)
    {}

    using base_t::print;

    void print(const AST* ast)
    {
        print(ast->root_node());
        ost << std::endl;
    }

    void print_attr(const char* attr)
    { ost << endl(1) << '.' << attr << " = "; }

    template<typename T>
    void print_list(const std::pair<size_t, T const* const*>& list);

    AttrPrint<BasicPrinter> attr(const char* attr)
    { return AttrPrint<BasicPrinter>(this, attr); }

    template<typename T>
    ListPrint<BasicPrinter, T> list(const std::pair<size_t, T const* const*>& list)
    { return ListPrint<BasicPrinter, T>(this, list); }

    template<typename T, typename R> friend struct ListPrint; //!!!
    friend struct AttrPrint<BasicPrinter>;

    static Ext::Repr<list_elem_idx_t> type(const list_elem_idx_t idx)
    { return Ext::repr(idx); }

    DotsEndlPrint endl() const
    { return DotsEndlPrint(indent_level, opt->indent_dots); }
    DotsEndlPrint endl(short inc) const
    { return DotsEndlPrint(indent_level + inc, opt->indent_dots); }

private:
    const options_t* opt;
    bool verbose;
};

class Printer : private BasicPrinter
{
public:
    Printer(std::ostream& _ost, const options_t* _opt) :
        BasicPrinter(_ost, _opt, true)
    {}

    void print(const AST*, bool newline = true);

private:
    typedef TreeAST::Tree Tree;

    void visit(const Node*); //!!! ???
// > PRINTER_VISITOR_DECL $ grammar -VD
    void visit(const Literal*);
    void visit(const ListElem*);
    void visit(const NodeName*);
    void visit(const NodeList*);
    void visit(const ArgName*);
    void visit(const ArgValue*);
    void visit(const NodeValue*);
    void visit(const ListValue*);
    void visit(const LiteralValue*);
    void visit(const NodeArg*);
    void visit(const NodeArgs*);
    void visit(const ListArgs*);
    void visit(const LiteralArgs*);
    void visit(const TreeNode*);
    void visit(const Tree*);
// < PRINTER_VISITOR_DECL

    using base_t::type;
    using BasicPrinter::type;
};

class Dumper : private BasicPrinter
{
public:
    Dumper(std::ostream& _ost, const options_t* _opt) :
        BasicPrinter(_ost, _opt, false)
    {}

    using BasicPrinter::print;

private:
    typedef TreeAST::Tree Tree;

    void visit(const Node*); //!!! ???
// > DUMPER_VISITOR_DECL $ grammar -VD
    void visit(const Literal*);
    void visit(const ListElem*);
    void visit(const NodeName*);
    void visit(const NodeList*);
    void visit(const ArgName*);
    void visit(const ArgValue*);
    void visit(const NodeValue*);
    void visit(const ListValue*);
    void visit(const LiteralValue*);
    void visit(const NodeArg*);
    void visit(const NodeArgs*);
    void visit(const ListArgs*);
    void visit(const LiteralArgs*);
    void visit(const TreeNode*);
    void visit(const Tree*);
// < DUMPER_VISITOR_DECL

    using base_t::type;
    using BasicPrinter::type;
};

class SourcePrinter : private BasicPrinter
{
public:
    SourcePrinter(std::ostream& _ost, const options_t* _opt, bool _pretty) :
        BasicPrinter(_ost, _opt, false),
        pretty(_pretty)
    {}

    using BasicPrinter::print;

private:
    typedef TreeAST::Tree Tree;

    void print(const Node*);
    void print(const list_elem_idx_t);
    void print_index(size_t);

    template<typename T>
    void print(const std::pair<size_t, T const* const*>&);

    void abstract(const Node*);

    void visit(const Node*); //!!! ???
// > PRETTY_PRINTER_VISITOR_DECL $ grammar -VD
    void visit(const Literal*);
    void visit(const ListElem*);
    void visit(const NodeName*);
    void visit(const NodeList*);
    void visit(const ArgName*);
    void visit(const ArgValue*);
    void visit(const NodeValue*);
    void visit(const ListValue*);
    void visit(const LiteralValue*);
    void visit(const NodeArg*);
    void visit(const NodeArgs*);
    void visit(const ListArgs*);
    void visit(const LiteralArgs*);
    void visit(const TreeNode*);
    void visit(const Tree*);
// < PRETTY_PRINTER_VISITOR_DECL

    using base_t::type;
    using BasicPrinter::type;

    bool pretty;
};

template<typename T>
void BasicPrinter::print_list(const std::pair<size_t, T const* const*>& list)
{
    Level level(this);
    ost << "list<" << node_name<T>() << "> [";
    auto ptr = list.second;
    auto end = ptr + list.first;
    size_t k = 0;
    for (; ptr != end; ptr ++, k ++) { //!!! use std::for_each
        ost << endl(1);
        if (verbose) ost << '[' << k << "] = ";
        ost << node(*ptr);
    }
    if (k > 0)
        ost << endl();
    ost << ']';
}

inline void Printer::print(const AST* ast, bool newline)
{
    base_t::print(ast->root_node());
    if (newline) ost << '\n';
    ost << std::flush;
}

inline void SourcePrinter::print(const Node* node)
{
    node->accept(visitor);
}

inline void SourcePrinter::print(const list_elem_idx_t idx)
{
    ost << Ext::integer_cast<short>(idx);
}

inline void SourcePrinter::print_index(size_t idx)
{
    ost << '[';
    print(idx);
    ost << (pretty ? "] = " : "]=");
}

template<typename T>
void SourcePrinter::print(const std::pair<size_t, T const* const*>& list)
{
    Level level(this);
    auto ptr = list.second;
    auto end = ptr + list.first;
    for (; ptr != end; ptr ++) {
        if (pretty)
            ost << endl();
        print(*ptr);
    }
    if (pretty && end - list.second > 0)
        ost << endl(-1);
}

void SourcePrinter::abstract(const Node* node)
{
    using TreeAST::error;
    error("cannot visit abstract node '%s'", node->name());
}

// > PRINTER_VISITOR_IMPL $ grammar -NP=Printer
void Printer::visit(const Node* arg)
{
    ost << empty(arg);
}

void Printer::visit(const Literal* arg)
{
    ost << name(arg) << '('
        << repr(arg->str) << ')';
}

void Printer::visit(const ListElem* arg)
{
    ost << name(arg) << '('
        << attr("index")
        << type(arg->index)
        << attr("node")
        << node(arg->node)
        << endl() << ')';
}

void Printer::visit(const NodeName* arg)
{
    ost << name(arg) << '('
        << repr(arg->ident) << ')';
}

void Printer::visit(const NodeList* arg)
{
    ost << name(arg) << '('
        << attr("base")
        << node(arg->base)
        << attr("elems")
        << list(arg->elems)
        << endl() << ')';
}

void Printer::visit(const ArgName* arg)
{
    ost << name(arg) << '('
        << repr(arg->str) << ')';
}

void Printer::visit(const ArgValue* arg)
{
    ost << empty(arg);
}

void Printer::visit(const NodeValue* arg)
{
    ost << name(arg) << '('
        << attr("node")
        << node(arg->node)
        << endl() << ')';
}

void Printer::visit(const ListValue* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << node(arg->list)
        << endl() << ')';
}

void Printer::visit(const LiteralValue* arg)
{
    ost << name(arg) << '('
        << attr("literal")
        << node(arg->literal)
        << endl() << ')';
}

void Printer::visit(const NodeArg* arg)
{
    ost << name(arg) << '('
        << attr("name")
        << node(arg->name)
        << attr("value")
        << node(arg->value)
        << endl() << ')';
}

void Printer::visit(const NodeArgs* arg)
{
    ost << empty(arg);
}

void Printer::visit(const ListArgs* arg)
{
    ost << name(arg) << '('
        << attr("list")
        << list(arg->list)
        << endl() << ')';
}

void Printer::visit(const LiteralArgs* arg)
{
    ost << name(arg) << '('
        << attr("literal")
        << node(arg->literal)
        << endl() << ')';
}

void Printer::visit(const TreeNode* arg)
{
    ost << name(arg) << '('
        << attr("name")
        << node(arg->name)
        << attr("args")
        << node(arg->args)
        << endl() << ')';
}

void Printer::visit(const Tree* arg)
{
    ost << name(arg) << '('
        << attr("root")
        << node(arg->root)
        << endl() << ')';
}
// < PRINTER_VISITOR_IMPL

// > DUMPER_VISITOR_IMPL $ grammar -NR=Dumper
void Dumper::visit(const Node* arg)
{
    ost << empty(arg);
}

void Dumper::visit(const Literal* arg)
{
    ost << name(arg) << '('
        << repr(arg->str) << ')';
}

void Dumper::visit(const ListElem* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << type(arg->index)
        << endl(1)
        << node(arg->node)
        << endl() << ')';
}

void Dumper::visit(const NodeName* arg)
{
    ost << name(arg) << '('
        << repr(arg->ident) << ')';
}

void Dumper::visit(const NodeList* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->base)
        << endl(1)
        << list(arg->elems)
        << endl() << ')';
}

void Dumper::visit(const ArgName* arg)
{
    ost << name(arg) << '('
        << repr(arg->str) << ')';
}

void Dumper::visit(const ArgValue* arg)
{
    ost << empty(arg);
}

void Dumper::visit(const NodeValue* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->node)
        << endl() << ')';
}

void Dumper::visit(const ListValue* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->list)
        << endl() << ')';
}

void Dumper::visit(const LiteralValue* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->literal)
        << endl() << ')';
}

void Dumper::visit(const NodeArg* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->name)
        << endl(1)
        << node(arg->value)
        << endl() << ')';
}

void Dumper::visit(const NodeArgs* arg)
{
    ost << empty(arg);
}

void Dumper::visit(const ListArgs* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << list(arg->list)
        << endl() << ')';
}

void Dumper::visit(const LiteralArgs* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->literal)
        << endl() << ')';
}

void Dumper::visit(const TreeNode* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->name)
        << endl(1)
        << node(arg->args)
        << endl() << ')';
}

void Dumper::visit(const Tree* arg)
{
    ost << name(arg) << '('
        << endl(1)
        << node(arg->root)
        << endl() << ')';
}
// < DUMPER_VISITOR_IMPL

void SourcePrinter::visit(const Node* arg)
{
    abstract(arg);
}

// > PRETTY_PRINTER_VISITOR_IMPL $ grammar -VI=SourcePrinter
void SourcePrinter::visit(const Literal* arg)
{
    ost << '\'' << arg->str << '\''; //!!!STR_QUOTE
}

void SourcePrinter::visit(const ListElem* arg)
{
    print_index(arg->index);
    print(arg->node);
}

void SourcePrinter::visit(const NodeName* arg)
{
    ost << arg->ident;
}

void SourcePrinter::visit(const NodeList* arg)
{
    ost << "list<";
    print(arg->base);
    ost << (pretty ? "> [" : ">[");
    print(arg->elems);
    ost << ']';
}

void SourcePrinter::visit(const ArgName* arg)
{
    ost << '.' << arg->str << (pretty ? " = " : "=");
}

void SourcePrinter::visit(const ArgValue* arg)
{
    abstract(arg);
}

void SourcePrinter::visit(const NodeValue* arg)
{
    print(arg->node);
}

void SourcePrinter::visit(const ListValue* arg)
{
    print(arg->list);
}

void SourcePrinter::visit(const LiteralValue* arg)
{
    print(arg->literal);
}

void SourcePrinter::visit(const NodeArg* arg)
{
    print(arg->name);
    print(arg->value);
}

void SourcePrinter::visit(const NodeArgs* arg)
{
    abstract(arg);
}

void SourcePrinter::visit(const ListArgs* arg)
{
    print(arg->list);
}

void SourcePrinter::visit(const LiteralArgs* arg)
{
    print(arg->literal);
}

void SourcePrinter::visit(const TreeNode* arg)
{
    print(arg->name);
    ost << '(';
    print(arg->args);
    ost << ')';
}

void SourcePrinter::visit(const Tree* arg)
{
    print(arg->root);
}
// <PRETTY_PRINTER_VISITOR_IMPL

} // namespace AST

template<typename T>
static void print(const TreeAST::AST* ast, const options_t* opt)
{
    typedef T printer_t;
    printer_t(std::cout, opt).print(ast);
}

template<bool pretty>
static void source(const TreeAST::AST* ast, const options_t* opt)
{
    AST::SourcePrinter(std::cout, opt, pretty).print(ast);
}

static Sys::ext_func_result_t exec(
    const TreeAST::AST* ast,
    const Tree::options_t* opts,
    options_t::action_t act = 0)
{
    auto opt = options_t::options(opts);
    if (opt->action == options_t::ext_func_action) {
        using namespace ExtErr;
        print_error<ext_func_not_supported>();
        return 1;
    }
    if (ast == nullptr) {
        using namespace ExtErr;
        print_error<no_input_given>();
        return 1;
    }

    static void (*const funcs[4])(const TreeAST::AST*, const options_t*) = {
        &print<AST::Printer>, // print_ast_action
        &print<AST::Dumper>,  // dump_ast_action
        &source<true>,        // pretty_ast_action
        &source<false>,       // terse_ast_action
    };
    Ext::array(funcs)[
        (act > 0 ? act : opt->action) - options_t::print_ast_action]
        (ast, opt);

    return 0;
}

EXT_FUNC_ENTRY
Sys::ext_func_result_t printer(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    return exec(ast, opts);
}

EXT_FUNC_ENTRY
Sys::ext_func_result_t print(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    return exec(ast, opts, options_t::print_ast_action);
}

EXT_FUNC_ENTRY
Sys::ext_func_result_t dump(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    return exec(ast, opts, options_t::dump_ast_action);
}

EXT_FUNC_ENTRY
Sys::ext_func_result_t pretty_print(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    return exec(ast, opts, options_t::pretty_ast_action);
}

EXT_FUNC_ENTRY
Sys::ext_func_result_t terse_print(
    const TreeAST::AST* ast, const Tree::options_t* opts)
{
    return exec(ast, opts, options_t::terse_ast_action);
}

