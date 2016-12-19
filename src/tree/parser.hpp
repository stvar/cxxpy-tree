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

#ifndef __PARSER_HPP
#define __PARSER_HPP

#include <vector>
#include <stdexcept>

#include "sys.hpp"
#include "ast.hpp"
#include "parser-base.hpp"

namespace Parser {

namespace AST = TreeAST;

class ParserError :
    public std::runtime_error
{
public:
    typedef std::runtime_error base_t;
    typedef std::vector<std::string> msgs_t;

    ParserError(const std::string& _what) :
        base_t(_what),
        msgs(0)
    {}

    ParserError(const std::string& _what, const msgs_t& _msgs) :
        base_t(_what),
        msgs(_msgs)
    {}
    ParserError(const std::string& _what, msgs_t&& _msgs) :
        base_t(_what),
        msgs(std::move(_msgs))
    {}

    using base_t::what;

    std::string msg(const char* head) const;

    std::string msg(const std::string& head) const
    { return msg(head.c_str()); }

private:
    msgs_t msgs;
};

class Parser : public BaseParser
{
public:
    typedef BaseParser base_t;
    typedef base_t::input_type_t input_type_t;

    Parser(
#ifdef DEBUG
        bool _debug,
#endif
        bool _verbose_error,
        input_type_t _input_type,
        const char* _input_name,
        const char* _input);

    std::unique_ptr<AST::AST> parse();

private:
    using base_t::token_name;

    // $ grammar -r0 grammar.g++|env LD_LIBRARY_PATH=$GRAMMAR_HOME $GRAMMAR_HOME/grammar --verbose -fgFstats -|ssed -nR '/^ast-/!p'
    // statistics:
    // symbols:        22
    // names:          9 list_elem node_args node_arg literal node_name tree_node arg_value tree arg_name
    // idents:         4 NAME STR IDENT NUM
    // literals:       9 "." "<" "=" ">" "(" ")" "[" "list" "]"

    struct token_type_t
    {
        enum : token_t::type_t {
            bos          = token_t::bos,
            eos          = token_t::eos,

            open_paren   = token<2>(),  // '('
            close_paren  = token<3>(),  // ')'
            open_sqbrk   = token<4>(),  // '['
            close_sqbrk  = token<5>(),  // ']'
            open_angbrk  = token<6>(),  // '<'
            close_angbrk = token<7>(),  // '>'
            dot          = token<8>(),  // '.'
            equal        = token<9>(),  // '='
            name         = token<10>(),
            ident        = token<11>(),
            str          = token<12>(),
            num          = token<13>(),
            kw_list      = token<14>(), // "list"
        };
    };

    const char* token_name(token_t::type_t type);
    void next_token();
#ifdef DEBUG
    void dump(const char* where);
#endif
    void verror(const pos_t& pos, const char* msg, va_list args);
    void build_ln_ptrs();

    void skip_spaces();
    static bool isident0(char ch);
    static bool isident(char ch);
    static bool isname(char ch);
    static token_t::type_t ischartoken(char ch);
    static bool istoken(const range_t& rng, const char* tok, size_t len);
    const char* scan_literal(const char* ptr);

    void not_yet_impl(const char *func);

    AST::list_elem_idx_t list_elem_idx(const char*, const pos_t&);

// > PARSER_METHOD_DECL $ grammar -D
    const AST::Tree* parse_tree();
    const AST::TreeNode* parse_tree_node();
    const AST::NodeName* parse_node_name();
    const AST::NodeArgs* parse_node_args();
    const AST::NodeArg* parse_node_arg();
    const AST::ArgName* parse_arg_name();
    const AST::ArgValue* parse_arg_value();
    const AST::ListElem* parse_list_elem();
    const AST::Literal* parse_literal();
// < PARSER_METHOD_DECL

    template<bool Parser::*what>
    struct ctxt_t
    {
        ctxt_t(Parser* _self) : self(_self) { self->*what = true; }
        ~ctxt_t()                           { self->*what = false; }
        Parser* self;
    };

    typedef std::vector<const char*> ln_ptrs_t;

    ln_ptrs_t   ln_ptrs;
    const char *input_beg;
    const bool  verbose_error;
    bool        arg_ctxt;
    AST::AST   *ast;

    typedef ctxt_t<&Parser::arg_ctxt> arg_ctxt_t;
};

} // namespace Parser

#endif /*  __PARSER_HPP */

