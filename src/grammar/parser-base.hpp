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

#ifndef __PARSER_BASE_HPP
#define __PARSER_BASE_HPP

#include <cstdarg>
#include <cstddef>

#include <iostream>
#include <memory>

#include "sys.hpp"

namespace Parser {

typedef unsigned token_type_t;

template<token_type_t t>
constexpr token_type_t token()
{ CXX_ASSERT(t < Ext::digits<token_type_t>()); return 1U << t; }

class BaseParser
{
public:
    enum input_type_t {
        text_input,
        file_input
    };

    BaseParser(const BaseParser&) = delete;
    BaseParser& operator=(const BaseParser&) = delete;

    BaseParser(BaseParser&&) = delete;
    BaseParser& operator=(BaseParser&&) = delete;

protected:
    BaseParser(
#ifdef DEBUG
        bool _debug,
#endif
        input_type_t _input_type,
        const char* _input_name,
        const char* _input);

    virtual ~BaseParser();

    struct range_t
    {
        range_t() :
            range_t(nullptr, nullptr)
        {}

        range_t(const char* _beg, const char* _end) :
            beg(_beg), end(_end)
        { SYS_ASSERT(beg <= end); }

        size_t size() const
        { return Ext::ptr_diff(end, beg); }

        const char *beg;
        const char *end;
    };

    struct pos_t
    {
        pos_t() :
            pos_t(1, 1)
        {}

        pos_t(size_t _line, size_t _col) :
            line(_line), col(_col)
        {}

        pos_t& operator++()
        { col ++; return *this; }
        pos_t operator++(int)
        { auto r = *this; operator++(); return r; }

        pos_t& operator+=(size_t size)
        { col += size; return *this; }

        pos_t& operator+=(const range_t& rng)
        {
            SYS_ASSERT(rng.beg <= rng.end);

            const char* last = nullptr;
            for (auto ptr = rng.beg; ptr < rng.end; ptr ++) {
                if (*ptr != '\n') continue;
                last = ptr;
                line ++;
            }
            if (last)
                col = Ext::ptr_diff(rng.end, last);
            else
                col += rng.size();

            return *this;
        }

        pos_t operator+(size_t size)
        { auto r = *this; r += size; return r; }

        pos_t operator+(const range_t& rng)
        { auto r = *this; r += rng; return r; }

        void print(std::ostream& ost) const
        { ost << "pos(" << line << ',' << col << ')'; }

        size_t line;
        size_t col;
    };

    friend std::ostream& operator<<(std::ostream& ost, const pos_t& pos)
    { pos.print(ost); return ost; }

    class lexeme_t : private Sys::buf_t
    {
        typedef Sys::buf_t base_t;

    public:
        lexeme_t(const lexeme_t&) = delete;
        lexeme_t& operator=(const lexeme_t&) = delete;

        lexeme_t(lexeme_t&&) = delete;
        lexeme_t& operator=(lexeme_t&&) = delete;

        lexeme_t(const char* v) :
            base_t(64)
        { base_t::assign(v); }
        lexeme_t() :
            lexeme_t("")
        {}

        lexeme_t& operator=(const char* v)
        { base_t::assign(v); return *this; }
        lexeme_t& operator=(const range_t& v)
        { base_t::nassign(v.beg, v.size()); return *this; }
        lexeme_t& operator=(char v)
        { base_t::nassign(&v, 1); return *this; }

        const char* get() const
        { return base_t::get(); }

        operator const char*() const
        { return base_t::get(); }
    };

    struct token_t
    {
        typedef token_type_t type_t;

        enum : type_t {
            bos = token<0>(),
            eos = token<1>(),
        };

        token_t() :
            type(bos)
        {}

        token_t(
            const pos_t& _pos,
            type_t _type,
            const char* _lex = nullptr) :
            pos(_pos),
            type(_type),
            lex(_lex)
        {}

        token_t(const token_t& tok) :
            token_t(tok.pos, tok.type, tok.lex.get())
        {}

        token_t& operator=(const token_t& tok)
        { pos = tok.pos; type = tok.type; lex = tok.lex.get(); return *this; }

        bool operator==(type_t _type)
        { return type == _type; }

        pos_t pos;
        type_t type;
        lexeme_t lex;
    };

    virtual const char* token_name(token_t::type_t type) = 0;
    virtual void next_token() = 0;
    virtual void dump(const char* where) = 0;
    virtual void verror(const pos_t& pos, const char* msg, va_list args) = 0;

    void error(const pos_t& pos, const char* msg, ...) PRINTF_FMT(3);
    void error(const char* msg, ...) PRINTF_FMT(2);

    void unexpect_char(const pos_t&, char);

    void unexpect_char(char ch)
    { unexpect_char(pos, ch); }

    const char* token_name(const token_t& tok)
    { return token_name(tok.type); }

    void eat_token()
    { prev_tok = tok; next_token(); }

    bool eat_token(token_t::type_t type)
    { eat_token(); return prev_tok.type & type; }

    bool try_token(token_t::type_t type)
    { if (tok.type & type) { eat_token(); return true; } return false; }

    bool peek_token(token_t::type_t type)
    { return tok.type & type; }

    void need_token(token_t::type_t type, bool next = true)
    { need_token(token_name(type), type, next); }

    void need_token(const char* name, token_t::type_t type, bool next = true);
    void missed_token(const char* where);

    typedef Sys::mem_t input_buf_t;

#ifdef DEBUG
    bool          const debug;
#endif
    input_type_t  const input_type;

    const char   *input_name;
    const char   *input;
    input_buf_t   input_buf;
    const char   *input_ptr;
    const char   *input_end;
    bool          input_nle;
    token_t       prev_tok;
    token_t       tok;
    pos_t         pos;
};

} // namespace Parser

#endif /* __PARSER_BASE_HPP */

