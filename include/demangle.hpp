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

#ifndef __DEMANGLE_HPP
#define __DEMANGLE_HPP

#include <cstring>
#include <cstdlib>

#include <typeinfo>
#include <iostream>
#include <utility>

#include <cxxabi.h>

// http://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
// Chapter 29. Demangling
// Transforming C++ ABI identifiers (like RTTI symbols) into the original
// C++ source identifiers is called "demangling."

// https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
// cxxabi.h:
//
// char* abi::__cxa_demangle(const char* mangled_name,
//                           char* output_buffer,
//                           size_t* length,
//                           int* status);
//
// Demangling routine.
// New ABI-mandated entry point in the C++ runtime library for demangling.
//
// 'mangled_name': A NUL-terminated character string containing the name to
// be demangled.
//
// 'output_buffer': A region of memory, allocated with 'malloc', of '*length'
// bytes, into which the demangled name is stored.  If 'output_buffer' is not
// long enough, it is expanded using 'realloc'.  'output_buffer' may instead
// be NULL; in that case, the demangled name is placed in a region of memory
// allocated with 'malloc'.
//
// 'length': If 'length' is non-NULL, the length of the buffer containing the
// demangled name is placed in '*length'.
//
// 'status': '*status': is set to one of the following values:
//    0: The demangling operation succeeded.
//   -1: A memory allocation failure occurred.
//   -2: 'mangled_name' is not a valid name under the C++ ABI mangling rules.
//   -3: One of the arguments is invalid.
//
// Returns: A pointer to the start of the NUL-terminated demangled name, or
// NULL if the demangling fails.  The caller is responsible for deallocating
// this memory using free.
//
// The demangling is performed using the C++ ABI mangling rules,
// with GNU extensions. For example, this function is used in:
// __gnu_cxx::__verbose_terminate_handler.

namespace Sys {

class demangle_t
{
public:
	demangle_t() noexcept :
		e(0), s(nullptr)
	{}

	demangle_t(const char* _s) noexcept :
		demangle_t()
	{ s = abi::__cxa_demangle(_s, nullptr, nullptr, &e); }

	demangle_t(const demangle_t& d) noexcept :
		e(d.e), s(nullptr)
	{ if (d.s) s = strdup(d.s); if (d.s && !s) e = -1; }

	demangle_t(demangle_t&& d) noexcept :
		e(d.e), s(d.s)
	{ d.reset(); }

	~demangle_t() noexcept
	{ if (s) free(s); reset(); }

	demangle_t& operator=(const demangle_t& d)
	// copy-and-swap idiom
	{ demangle_t t(d); swap(t); return *this; }

	demangle_t& operator=(demangle_t&& d) noexcept
	// move-and-swap idiom
	{ demangle_t t(std::move(d)); swap(t); return *this; }

	int err() const
	{ return e; }

	const char* str() const
	{ return s; }

	void print(std::ostream& ost) const
	{ ost << (s ? s : "???");  }

private:
	void swap(demangle_t& d) noexcept
	{ std::swap(e, d.e); std::swap(s, d.s); }

	void reset() noexcept
	{ e = 0; s = nullptr; }

	int   e;
	char* s;
};

inline std::ostream& operator<<(
	std::ostream& ost, const demangle_t& obj)
{ obj.print(ost); return ost; }

inline demangle_t demangle(const char* s)
{ return demangle_t(s); }

template<typename T>
inline demangle_t demangle()
{ return demangle(typeid(T).name()); }

template<typename T>
inline demangle_t demangle(const T& o)
{ return demangle(typeid(o).name()); }

} // namespace Sys

#endif /* __DEMANGLE_HPP */


