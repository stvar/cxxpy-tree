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

#ifndef __AST_BASE_PRINTER_HPP
#define __AST_BASE_PRINTER_HPP

namespace AST {

template<typename T>
struct NodePrint
{
    NodePrint(T* _self, const typename T::root_t* _node) :
        self(_self), node(_node)
    {}

    void print(std::ostream& ost UNUSED) const
    { self->print(node); }

    T* self;
    const typename T::root_t* node;
};

template<typename T>
inline std::ostream& operator<<(std::ostream& ost, const NodePrint<T>& obj)
{ obj.print(ost); return ost; }

template<typename T>
struct EmptyPrint
{
    EmptyPrint(T* _self, const typename T::root_t* _node) :
        self(_self), node(_node)
    {}

    void print(std::ostream& ost UNUSED) const
    { self->print_empty(node); }

    T* self;
    const typename T::root_t* node;
};

template<typename T>
inline std::ostream& operator<<(std::ostream& ost, const EmptyPrint<T>& obj)
{ obj.print(ost); return ost; }

template<typename T>
struct AttrPrint
{
    AttrPrint(T* _self, const char* _attr) :
        self(_self), attr(_attr)
    {}

    void print(std::ostream& ost UNUSED) const
    { self->print_attr(attr); }

    T* self;
    const char* attr;
};

template<typename T>
inline std::ostream& operator<<(std::ostream& ost, const AttrPrint<T>& obj)
{ obj.print(ost); return ost; }

template<typename T, typename N>
struct ListPrint
{
    typedef std::pair<size_t, N const* const*> list_t;

    ListPrint(T* _self, const list_t& _list) :
        self(_self), list(_list)
    {}

    void print(std::ostream& ost UNUSED) const
    { self->print_list(list); }

    T* self;
    list_t list;
};

template<typename T, typename R>
inline std::ostream& operator<<(std::ostream& ost, const ListPrint<T, R>& obj)
{ obj.print(ost); return ost; }

template<typename T>
struct TypePrint
{
    TypePrint(const T& _val) :
        val(_val)
    {}

    void print(std::ostream& ost) const
    { ost << val; }

    const T& val;
};

template<typename T>
inline std::ostream& operator<<(std::ostream& ost, const TypePrint<T>& obj)
{ obj.print(ost); return ost; }

struct ReprPrint
{
    ReprPrint(const char* _str) :
        str(_str)
    {}

    void print(std::ostream& ost) const
    //!!!STR_QUOTE { ost << '\'' << Ext::repr(str) << '\''; }
    { ost << '\'' << str << '\''; } //!!!STR_QUOTE

    const char* str;
};

inline std::ostream& operator<<(std::ostream& ost, const ReprPrint& obj)
{ obj.print(ost); return ost; }

struct EndlPrint
{
    EndlPrint(short _level) :
        level(_level)
    {}

    void print(std::ostream& ost) const
    {
        ost << std::endl;
        if (!level) return;
        ost << std::right << std::setw(4 * level) << ' ';
    }

    short level;
};

inline std::ostream& operator<<(std::ostream& ost, const EndlPrint& obj)
{ obj.print(ost); return ost; }

template<typename R, typename V>
class BasePrinter
{
public:
    typedef R root_t;
    typedef V visitor_t;

    BasePrinter(std::ostream& _ost, visitor_t* _visitor) :
        ost(_ost),
        visitor(_visitor),
        indent_level(-1)
    {}

protected:
    void print(const root_t* node)
    {
        Level level(this);
        node->accept(visitor);
    }

    static const char* name(const root_t* arg)
    { return arg->name(); }

    void print_empty(const root_t* arg)
    { ost << name(arg) << "()"; }

    void print_attr(const char* attr)
    { ost << endl(1) << '.' << attr << " = "; }

    template<typename T>
    void print_list(const std::pair<size_t, T const* const*>& list);

    NodePrint<BasePrinter> node(const root_t* node)
    { return NodePrint<BasePrinter>(this, node); }
    EmptyPrint<BasePrinter> empty(const root_t* node)
    { return EmptyPrint<BasePrinter>(this, node); }
    AttrPrint<BasePrinter> attr(const char* attr)
    { return AttrPrint<BasePrinter>(this, attr); }

    template<typename T>
    ListPrint<BasePrinter, T> list(const std::pair<size_t, T const* const*>& list)
    { return ListPrint<BasePrinter, T>(this, list); }

    template<typename T>
    static TypePrint<T> type(const T& val)
    { return TypePrint<T>(val); }

    static ReprPrint repr(const char* str)
    { return ReprPrint(str); }

    EndlPrint endl() const
    { return EndlPrint(indent_level); }
    EndlPrint endl(short inc) const
    { return EndlPrint(indent_level + inc); }

    friend struct NodePrint<BasePrinter>;
    friend struct EmptyPrint<BasePrinter>;
    friend struct AttrPrint<BasePrinter>;

    template<typename T, typename N> friend struct ListPrint; //!!!

    struct Level
    {
        Level(BasePrinter* _self) :
            self(_self)
        { ++ self->indent_level; }

        ~Level()
        { -- self->indent_level; }

        BasePrinter* self;
    };

    std::ostream& ost;
    visitor_t* visitor;
    short indent_level;
};

template<typename R, typename V>
template<typename T>
void BasePrinter<R, V>::print_list(
    const std::pair<size_t, T const* const*>& list)
{
    Level level(this);
    ost << "list<" << node_name<T>() << "> [";
    auto ptr = list.second;
    auto end = ptr + list.first;
    size_t k = 0;
    for (; ptr != end; ptr ++, k ++) //!!! use std::for_each
        ost << endl(1) << '[' << k << "] = " << node(*ptr);
    if (k > 0)
        ost << endl();
    ost << ']';
}

} // namespace AST

#endif /* __AST_BASE_PRINTER_HPP */

