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

#include <sstream>

#include "std-ext.hpp"
#include "parser-base.hpp"

namespace Parser {

BaseParser::BaseParser(
#ifdef DEBUG
    bool _debug,
#endif
    input_type_t _input_type,
    const char* _input_name,
    const char* _input) :
#ifdef DEBUG
    debug(_debug),
#endif
    input_type(_input_type),
    input_name(_input_name),
    input(_input),
    input_buf(0),
    input_ptr(nullptr),
    input_end(nullptr),
    input_nle(false)
{
    if (input_type == text_input) {
        input_ptr = input;
        input_end = input_ptr + strlen(input);
    }
    else
    if (input_type == file_input) {
        typedef Sys::file_t file_t;
        if (!strcmp(input, "-")) {
            input_name = file_t::stdin_name;
            input = nullptr;
        }
        file_t file(input);
        input_buf = file.read();
        input_ptr = input_buf.get();
        input_end = input_buf.end();
    }
    else
        SYS_UNEXPECT_ERR(
            "input_type='%d'",
            input_type);
    SYS_ASSERT(
        input_end >= input_ptr);
    // stev: adjust the pointer to end of input when the
    // input ends with a newline char; conceptually, from
    // the stand point of parser, the newline char at the
    // end of input is part of the EOS token
    if (input_end > input_ptr &&
        input_end[-1] == '\n') {
        input_nle = true;
        input_end --;
    }
}

BaseParser::~BaseParser()
{}

void BaseParser::error(const char* msg, ...)
{
    CXX_ASSERT(CONFIG_VA_END_NOOP);
    va_list args;
    va_start(args, msg);
    verror(pos, msg, args);
    //!!!VA_END va_end(args);
}

void BaseParser::error(const pos_t& pos, const char* msg, ...)
{
    CXX_ASSERT(CONFIG_VA_END_NOOP);
    va_list args;
    va_start(args, msg);
    verror(pos, msg, args);
    //!!!VA_END va_end(args);
}

void BaseParser::unexpect_char(const pos_t& pos, char ch)
{
    std::ostringstream ost;
    ost << "unexpected "
        << (!isascii(ch) ? "non-ascii " : "")
        << "char "
        << Ext::repr(ch, true);
    error(pos, "%s", ost.str().c_str());
}

void BaseParser::need_token(
    const char* name, token_t::type_t type, bool next)
{
    if (tok.type != type)
        error(tok.pos, "expected token %s but got %s",
            name, token_name(tok));
    if (next)
        eat_token();
}

void BaseParser::missed_token(const char* where)
{
    const char *ptr = strchr(where, '_');
    error(tok.pos, "unexpected token %s in %s",
        token_name(tok), ptr ? ptr + 1 : where);
}

} // namespace Parser

