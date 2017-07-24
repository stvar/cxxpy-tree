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

#ifndef __AST_BASE_IMPL_HPP
#define __AST_BASE_IMPL_HPP

namespace AST {

template<typename T>
struct ArgList;

template<typename T>
struct ArgResult
{
    typedef T result_t;
};

template<typename R>
struct Arg<R, const char*> : public ArgResult<const char*>
{
    //!!!
    //static void check(BaseAST<R>* self, const char* arg)
    //{
    //	if (!self->raw_alloc.find(arg))
    //		error("invalid char pointer '%p'", arg);
    //}

    static const char* alloc(BaseAST<R>* self, const char* arg)
    {
        auto sz = 1 + strlen(arg);
        auto ptr = static_cast<char*>(
            self->raw_alloc.template allocate<char>(sz));
        memcpy(ptr, arg, sz);
        return ptr;
    }
};

template<typename R>
struct Arg<R, const R*> : public ArgResult<const R*>
{
    static void check(BaseAST<R>* self, const R* arg)
    {
        if (!self->node_alloc.find(arg))
            error("invalid node pointer '%p'", arg);
    }
    static const R* alloc(BaseAST<R>* self, const R* arg)
    {
        check(self, arg);
        return arg;
    }
};

template<typename R, typename T>
struct Arg<R, const T*> : public ArgResult<const T*>
{
    static void check(BaseAST<R>* self, const T* arg)
    {
        Arg<R, const R*>::check(self, arg);
    }
    static const T* alloc(BaseAST<R>* self, const T* arg)
    {
        check(self, arg);
        return arg;
    }
};

template<typename R, typename T>
struct Arg<R, const std::list<const T*>*> :
    public ArgResult<typename ArgList<T>::list_t>
{
    typedef typename ArgList<T>::list_t list_t;

    static void check(BaseAST<R>* self, const std::list<const T*>* arg)
    {
        if (arg && arg->size()) {
            std::for_each(arg->begin(), arg->end(), [=](const T* node)
                { Arg<R, const T*>::check(self, node); });
        }
    }

    static list_t alloc(AST* self, const std::list<const T*>* arg)
    {
        check(self, arg);
        auto sz = arg ? arg->size() : 0;
        T const** ptrs = nullptr;
        if (sz) {
            ptrs = static_cast<T const**>(
                self->raw_alloc.template allocate<T const*>(sz));
            std::copy(arg->begin(), arg->end(), ptrs);
        }
        return list_t(sz, ptrs);
    }
};

template<typename R, typename T>
struct Arg<R, std::pair<size_t, T const* const*>> :
    public ArgResult<typename std::pair<size_t, T const* const*>>
{
    typedef typename std::pair<size_t, T const* const*> list_t;

    static void check(BaseAST<R>* self, const list_t arg)
    {
        auto beg = arg.second;
        auto end = beg + arg.first;
        std::for_each(beg, end, [=](const T* node)
            { Arg<R, const T*>::check(self, node); });
    }

    static list_t alloc(AST* self, const list_t arg)
    {
        check(self, arg);
        return arg;
    }
};

} // namespace AST

#endif /* __AST_BASE_IMPL_HPP */

