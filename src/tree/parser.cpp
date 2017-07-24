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

#include <cctype>
#include <cstring>

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <iterator>
#include <limits>

#include "sys.hpp"
#include "std-ext.hpp"
#include "parser.hpp"

namespace Parser {

std::string ParserError::msg(const char* head) const
{
    std::string res;

    const auto fmt = [&](const std::string& str, bool nl = true) {
        (!nl ? res : res
            .append("\n"))
            .append(head)
            .append(": ")
            .append(str); };

    fmt(what(), false);
    for_each(msgs.begin(), msgs.end(), fmt);

    return res;
}

Parser::Parser(
#ifdef DEBUG
    bool _debug,
#endif
    bool _verbose_error,
    input_type_t _input_type,
    const char* _input_name,
    const char* _input) :
    base_t(
#ifdef DEBUG
        _debug,
#endif
        _input_type,
        _input_name,
        _input),
    input_beg(nullptr),
    verbose_error(_verbose_error),
    arg_ctxt(false),
    ast(nullptr)
{ input_beg = input_ptr; }

const char* Parser::token_name(token_t::type_t type)
{
    switch (type) {
    case token_type_t::open_paren:
        return "\"(\"";
    case token_type_t::close_paren:
        return "\")\"";
    case token_type_t::open_sqbrk:
        return "\"[\"";
    case token_type_t::close_sqbrk:
        return "\"]\"";
    case token_type_t::open_angbrk:
        return "\"<\"";
    case token_type_t::close_angbrk:
        return "\">\"";
    case token_type_t::dot:
        return "\".\"";
    case token_type_t::equal:
        return "\"=\"";
    case token_type_t::bos:
        return "BOS";
    case token_type_t::eos:
        return "EOS";
    case token_type_t::name:
        return "NAME";
    case token_type_t::ident:
        return "IDENT";
    case token_type_t::str:
        return "STR";
    case token_type_t::num:
        return "NUM";
    case token_type_t::kw_list:
        return "\"list\"";
    default:
        SYS_UNEXPECT_ERR("tok='%d'", type);
        // make the compiler happy
        return nullptr;
    }
}

void Parser::skip_spaces()
{
    using namespace std;
    auto ptr = find_if(
        input_ptr, input_end, [](char c) { return !::isspace(c); });
    if (ptr < input_end && *ptr == '#')
        ptr = find_if(
            ptr + 1, input_end, [](char c) { return c  == '\n'; });
    auto rng = range_t(input_ptr, ptr);
#ifdef DEBUG
    if (debug && rng.size() > 0) {
        if (ptr == input_end && input_nle)
            ptr ++;
        auto w1 = Sys::digits(pos.line) + Sys::digits(pos.col);
        SYS_ASSERT(w1 <= 2 + 4);
        cout
            << left << setw(21) << __func__ << pos
            << right << setw(2 + 4 - w1)
            << " ^"
            << Ext::repr(input_ptr, ptr) << '$';
    }
#endif
    pos += rng;
#ifdef DEBUG
    if (debug && rng.size() > 0)
        cout << ' ' << pos << endl;
#endif
    input_ptr = ptr;
}

inline bool Parser::isident0(char ch)
{
    return isalpha(ch) || ch == '_';
}

inline bool Parser::isident(char ch)
{
    return isident0(ch) || isdigit(ch);
}

inline bool Parser::isname(char ch)
{
    return islower(ch) || isdigit(ch) || ch == '_';
}

inline Parser::token_t::type_t Parser::ischartoken(char ch)
{
    if (ch == '(')
        return token_type_t::open_paren;
    if (ch == ')')
        return token_type_t::close_paren;
    if (ch == '[')
        return token_type_t::open_sqbrk;
    if (ch == ']')
        return token_type_t::close_sqbrk;
    if (ch == '<')
        return token_type_t::open_angbrk;
    if (ch == '>')
        return token_type_t::close_angbrk;
    if (ch == '.')
        return token_type_t::dot;
    if (ch == '=')
        return token_type_t::equal;
    else
        return 0;
}

inline bool Parser::istoken(const range_t& rng, const char* tok, size_t len)
{
    return rng.size() == len && memcmp(rng.beg, tok, len) == 0;
}

static inline bool is_plain_char(char ch)
{
    return isascii(ch) && !iscntrl(ch);
}

inline const char* Parser::scan_literal(const char* ptr)
{
    const auto pos = [this, ptr](const char* p)
        { SYS_ASSERT(p >= ptr); return this->pos + (p - ptr); };

    const char ch = *ptr ++;
    while (ptr < input_end) {
        ptr = std::find_if(ptr, input_end, [=](char c) {
            return !is_plain_char(c) || c == '\\' || c == '\n' || c == ch; });
        if (ptr == input_end || *ptr != '\\')
            break;
        ++ ptr;
        if (ptr == input_end || *ptr == '\n')
            break;
        ++ ptr;
    }
    if (ptr == input_end)
        error(pos(ptr), "end of input inside literal");
    if (*ptr == '\n')
        error(pos(ptr), "end of input line inside literal");
    if (!is_plain_char(*ptr))
        unexpect_char(pos(ptr), *ptr);
    return ptr;
}

void Parser::next_token()
{
    using namespace std;
    skip_spaces();
    auto ptr = input_ptr;
    if (ptr >= input_end) {
        tok.pos = pos;
        tok.type = token_type_t::eos;
        tok.lex = range_t();
    }
    else
    if (auto type = ischartoken(*ptr)) {
        tok.pos = pos;
        tok.type = type;
        tok.lex = *ptr;
        pos ++;
        input_ptr ++;
    }
    else
    if (isascii(*ptr) && isident0(*ptr)) {
        auto ptr2 = find_if(ptr + 1, input_end, [this](char c)
            { return !isascii(c) || !isident(c); });
        range_t rng(ptr, ptr2);
        if (find_if(rng.beg, rng.end, [this](char c)
                { return !isname(c); }) != rng.end)
            tok.type = token_type_t::ident;
        else
        if (!arg_ctxt && istoken(rng, "list", 4))
            tok.type = token_type_t::kw_list;
        else
            tok.type = token_type_t::name;
        tok.pos = pos;
        tok.lex = rng;
        pos += rng.size();
        input_ptr = ptr2;
    }
    else
    if (*ptr == '\'' || *ptr == '"') {
        auto ptr2 = scan_literal(ptr);
        SYS_ASSERT(ptr2 > ptr && ptr2 < input_end);
        SYS_ASSERT(*ptr2 == *ptr);
        range_t rng(ptr + 1, ptr2);
        tok.pos = pos;
        tok.type = token_type_t::str;
        tok.lex = rng;
        pos += rng.size() + 2;
        input_ptr = ptr2 + 1;
    }
    else
    if (isdigit(*ptr)) {
        auto ptr2 = find_if(ptr + 1, input_end, 
            [](char c) { return !::isdigit(c); });
        range_t rng(ptr, ptr2);
        tok.pos = pos;
        tok.type = token_type_t::num;
        tok.lex = rng;
        pos += rng.size();
        input_ptr = ptr2;
    }
    else
        unexpect_char(*ptr);
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
}

#ifdef DEBUG
void Parser::dump(const char* where)
{
    using namespace std;
    auto end = input_ptr + 32;
    auto name = token_name(tok);
    auto lex = tok.lex.get();
    if (end > input_end + input_nle)
        end = input_end + input_nle;
    auto w1 = Sys::digits(pos.line) + Sys::digits(pos.col);
    SYS_ASSERT(w1 <= 6 + 4);
    auto w2 = Sys::digits(tok.pos.line) + Sys::digits(tok.pos.col);
    SYS_ASSERT(w2 <= 6 + 4);
    auto w3 = strlen(name);
    SYS_ASSERT(w3 <= 5 + 8);
    auto w4 = strlen(lex);
    SYS_ASSERT(w4 <= 2 + 16);
    cout
        << left << setw(21) << where << pos
        << right << setw(6 + 4 - w1)
        << " tok: "
        << tok.pos
        << right << setw(6 + 4 - w2)
        << " type=" << name
        << right << setw(5 + 8 - w3)
        << " lex="
        << '\'' << Ext::repr(lex) << '\''
        << right << setw(2 + 16 - w4)
        << " ^" << Ext::repr(input_ptr, end);
    if (end < input_end) cout
        << "...";
    cout
        << "$" << endl;
}
#endif

void Parser::build_ln_ptrs()
{
    ln_ptrs.clear();
    auto ptr = input_beg;
    ln_ptrs.push_back(ptr);
    while (ptr < input_end) {
        if (*ptr ++ == '\n')
            ln_ptrs.push_back(ptr);
    }
}

static inline std::pair<std::string, size_t> quote_non_plain(
    const char* beg, const char* end, const char* ptr)
{
    using namespace std;
    using namespace Ext;

    SYS_ASSERT(
        ptr >= beg &&
        ptr <= end);

    size_t n = 0;
    stringstream ost;
    ReprPrint<char> repr(ost);
    for (auto p = beg; p < end; p ++) {
        auto c = *p;
        if (!is_plain_char(c)) {
            auto r = repr(c, char_repr_type_t::plain);
            if (p < ptr) {
                SYS_ASSERT(r > 0);
                n += r - 1;
            }
        }
        else
            ost << c;
    }

    return make_pair(ost.str(), n);
}

void Parser::verror(const pos_t& pos, const char* msg, va_list args)
{
    using namespace std;

    const auto fmt = [this, &pos](const string& str)
        { return Sys::format("%s:%zu:%zu: %s",
            input_name, pos.line, pos.col, str.c_str()); };

    const auto what = fmt(Sys::vformat(msg, args));

    if (verbose_error) {
        if (ln_ptrs.size() == 0)
            build_ln_ptrs();
        SYS_ASSERT(
            ln_ptrs.size() > 0);

        SYS_ASSERT(
            pos.col > 0);
        SYS_ASSERT(
            pos.line > 0 &&
            pos.line <= ln_ptrs.size());

        auto beg = ln_ptrs[pos.line - 1];
        auto end = pos.line < ln_ptrs.size()
            ? ln_ptrs[pos.line]
            : input_end;

        SYS_ASSERT(
            end >= beg &&
            end <= input_end);

        if (end > beg && end[-1] == '\n')
            end --;

        ParserError::msgs_t msgs;

        auto ptr = beg + (pos.col - 1);
        SYS_ASSERT(ptr <= end);

        auto quo = quote_non_plain(beg, end, ptr);
        msgs.push_back(fmt(quo.first));

        // stev: TODO: future improvements might include also tab
        // stops computations for the correct placement of error
        // indicator symbol on output
        msgs.push_back(
            fmt(string(pos.col - 1 + quo.second, ' ').append("^")));

        throw ParserError(what, std::move(msgs));
    }
    else
        throw ParserError(what);
}

std::unique_ptr<AST::AST> Parser::parse()
{
    auto ast = Ext::make_unique<AST::AST>(
#ifdef DEBUG
        false //!!!debug
#endif
    );

    this->ast = ast.get();

    struct Cleanup
    {
        Cleanup(AST::AST*& _ref) : ref(_ref) {}
        ~Cleanup() { ref = nullptr; }
        AST::AST*& ref;
    };
    Cleanup cleanup(this->ast);

    next_token();
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    ast->root_node(parse_tree());
    need_token(token_type_t::eos, false);
    return ast;
}

void Parser::not_yet_impl(const char *func)
{
    error("method '%s' not yet implemented", func);
}

AST::list_elem_idx_t Parser::list_elem_idx(const char* str, const pos_t& pos)
{
    AST::list_elem_idx_t idx = 0;
    if (!Ext::parse_num(str, idx))
        error(pos, "invalid list-elem-index '%s'", str);
    return idx;
}

// $ grammar -r0 grammar.g++|env LD_LIBRARY_PATH=$GRAMMAR_HOME $GRAMMAR_HOME/grammar -fgF firstset -
// firstset: tree: IDENT 
// firstset: tree_node: IDENT 
// firstset: node_name: IDENT 
// firstset: node_args: "." STR 
// firstset: node_arg: "." 
// firstset: arg_name: NAME 
// firstset: arg_value: IDENT "list" "[" STR 
// firstset: list_elem: "[" 
// firstset: liter_elem: "[" 
// firstset: literal: STR 

// > PARSER_METHOD_IMPL $ grammar -M
// <Tree> tree : tree_node
//             ;
const AST::Tree* Parser::parse_tree()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    return ast->new_tree(parse_tree_node());
}

// <TreeNode> tree_node
//             : node_name "(" node_args ")"
//             ;
const AST::TreeNode* Parser::parse_tree_node()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    const AST::NodeName* name = parse_node_name();
    need_token(token_type_t::open_paren);
    const AST::NodeArgs* args = parse_node_args();
    need_token(token_type_t::close_paren);
    return ast->new_treenode(name, args);
}

// <NodeName> node_name
//             : IDENT
//             ;
const AST::NodeName* Parser::parse_node_name()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    need_token(token_type_t::ident);
    return ast->new_nodename(prev_tok.lex);
}

// <NodeArgs> node_args
//             : node_arg *
//             | literal
//             ;
const AST::NodeArgs* Parser::parse_node_args()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    if (peek_token(
        token_type_t::close_paren |
        token_type_t::dot)) {
        std::list<const AST::NodeArg*> args;
        while (peek_token(token_type_t::dot)) {
#ifdef DEBUG
            if (debug)
                dump(__func__);
#endif
            args.push_back(parse_node_arg());
        }
        return ast->new_listargs(&args);
    }
    else
    if (peek_token(token_type_t::str)) {
        return ast->new_literalargs(parse_literal());
    }
    else {
        missed_token(__func__);
        // make the compiler happy
        return nullptr;
    }
}

// <NodeArg> node_arg
//             : "." arg_name "=" arg_value
//             ;
const AST::NodeArg* Parser::parse_node_arg()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    arg_ctxt_t guard(this);
    need_token(token_type_t::dot);
    arg_ctxt = false;
    const AST::ArgName* name = parse_arg_name();
    need_token(token_type_t::equal);
    const AST::ArgValue* value = parse_arg_value();
    return ast->new_nodearg(name, value);
}

// <ArgName> arg_name
//             : NAME
//             ;
const AST::ArgName* Parser::parse_arg_name()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    need_token(token_type_t::name);
    return ast->new_argname(prev_tok.lex);
}

// <ArgValue> arg_value
//             : tree_node
//             | "list" "<" node_name ">" "[" list_elem * "]"
//             | literal
//             ;
const AST::ArgValue* Parser::parse_arg_value()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    if (peek_token(token_type_t::ident)) {
        return ast->new_nodevalue(parse_tree_node());
    }
    else
    if (try_token(token_type_t::kw_list)) {
        need_token(token_type_t::open_angbrk);
        const AST::NodeName* base = parse_node_name();
        need_token(token_type_t::close_angbrk);
        need_token(token_type_t::open_sqbrk);
        std::list<const AST::ListElem*> elems;
        while (peek_token(token_type_t::open_sqbrk)) {
#ifdef DEBUG
            if (debug)
                dump(__func__);
#endif
            elems.push_back(parse_list_elem());
        }
        need_token(token_type_t::close_sqbrk);
        return ast->new_listvalue(ast->new_nodelist(base, &elems));
    }
    else
    if (peek_token(token_type_t::str)) {
        return ast->new_literalvalue(parse_literal());
    }
    else {
        missed_token(__func__);
        // make the compiler happy
        return nullptr;
    }
}

// <ListElem> list_elem
//             : "[" NUM "]" "=" tree_node
//             ;
const AST::ListElem* Parser::parse_list_elem()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    need_token(token_type_t::open_sqbrk);
    need_token(token_type_t::num);
    const AST::list_elem_idx_t idx = list_elem_idx(
        prev_tok.lex, prev_tok.pos);
    need_token(token_type_t::close_sqbrk);
    need_token(token_type_t::equal);
    return ast->new_listelem(idx, parse_tree_node());
}

// <Literal> literal
//             : STR
//             ;
const AST::Literal* Parser::parse_literal()
{
#ifdef DEBUG
    if (debug)
        dump(__func__);
#endif
    need_token(token_type_t::str);
    return ast->new_literal(prev_tok.lex);
}
// < PARSER_METHOD_IMPL

} // namespace Parser

