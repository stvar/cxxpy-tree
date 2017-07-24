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

#ifndef __EXT_PRINTER_HPP
#define __EXT_PRINTER_HPP

#include "ast-base-printer.hpp"

namespace AST {

struct NonePrint
{
    NonePrint() {}

    void print(std::ostream& ost UNUSED) const
    {}
};

inline std::ostream& operator<<(std::ostream& ost, const NonePrint& obj)
{ obj.print(ost); return ost; }

struct BoolPrint
{
    BoolPrint(bool _val) : val(_val) {}

    void print(std::ostream& ost) const
    { ost << '\'' << val << '\''; }

    bool val;
};

inline std::ostream& operator<<(std::ostream& ost, const BoolPrint& obj)
{ obj.print(ost); return ost; }

template<typename R, typename V>
class ExtBasePrinter :
    protected BasePrinter<R, V>
{
protected:
    typedef BasePrinter<R, V> base_t;

    typedef R root_t;
    typedef V visitor_t;

    ExtBasePrinter(std::ostream& _ost, visitor_t* _visitor) :
        base_t(_ost, _visitor)
    {}

    using base_t::ost;
    using base_t::node;

    typedef typename base_t::Level Level;

    void print(const root_t* root)
    { base_t::print(root); std::cout << std::endl; }

    void print_attr(const char* attr)
    { ost << '.' << attr << "="; }

    template<typename T>
    void print_list(const std::pair<size_t, T const* const*>& list);

    NonePrint endl() const
    { return NonePrint(); }
    NonePrint endl(short inc UNUSED) const
    { return NonePrint(); }

    static BoolPrint type(bool val)
    { return BoolPrint(val); }

    AttrPrint<ExtBasePrinter> attr(const char* attr)
    { return AttrPrint<ExtBasePrinter>(this, attr); }

    template<typename T>
    ListPrint<ExtBasePrinter, T> list(const std::pair<size_t, T const* const*>& list)
    { return ListPrint<ExtBasePrinter, T>(this, list); }

    friend struct AttrPrint<ExtBasePrinter>;
    template<typename T, typename U> friend struct ListPrint; //!!!
};

template<typename R, typename V>
template<typename T>
void ExtBasePrinter<R, V>::print_list(
    const std::pair<size_t, T const* const*>& list)
{
    Level level(this);
    ost << "list<" << node_name<T>() << ">[";
    auto ptr = list.second;
    auto end = ptr + list.first;
    size_t k = 0;
    for (; ptr != end; ptr ++, k ++) //!!! use std::for_each
        ost << '[' << k << "]=" << node(*ptr);
    ost << ']';
}

} // namespace AST

#endif /* __EXT_PRINTER_HPP */


