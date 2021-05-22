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

#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG
#define COLORIZE
#define MAP_TYPE HASH_MAP

#ifndef __GNUC__
#error we need a GCC compiler
#endif

#ifndef __GNUC_MINOR__
#error __GNUC_MINOR__ is not defined
#endif

#ifndef __GNUC_PATCHLEVEL__
#error __GNUC_PATCHLEVEL__ is not defined
#endif

#define GCC_VERSION \
    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

//
// CONFIG_PTR_TO_INT_IDENTOP
// CONFIG_INT_TO_PTR_IDENTOP
//

// stev: CONFIG_PTR_TO_INT_IDENTOP establishes whether the compiler
// implements the conversion from pointer types to integer types of
// equal size as an identity operation. That is that the bits which
// result upon casting a pointer to an integer of the same size are
// identical with those of the pointer representation itself.

// stev: CONFIG_INT_TO_PTR_IDENTOP establishes whether the compiler
// implements the conversion from integer types to pointer types of
// equal size as an identity operation.

// stev: the issues implied by these two configuration parameters
// are pertaining to specifications of C language: ISO/IEC 9899:TC3,
// 6.3.2.3 Pointers; and to specifications of C++ language: ISO/IEC
// 14882:2003(E), 5.2.10 Reinterpret cast.

// stev: the implementation of GCC at least upward from v4.0.0 does
// precisely this (see http://gcc.gnu.org/onlinedocs/gcc-4.0.0/gcc/
// Arrays-and-pointers-implementation.html):
//
//   The result of converting a pointer to an integer or vice versa
//   (C90 6.3.4, C99 6.3.2.3):
//
//   A cast from pointer to integer discards most-significant bits
//   if the pointer representation is larger than the integer type,
//   sign-extends [1] if the pointer representation is smaller than
//   the integer type, otherwise the bits are unchanged.
//
//   A cast from integer to pointer discards most-significant bits
//   if the pointer representation is smaller than the integer type,
//   extends according to the signedness of the integer type if the
//   pointer representation is larger than the integer type,
//   otherwise the bits are unchanged.
//
//   Footnotes
//
//   [1] Future versions of GCC may zero-extend, or use a target-
//       defined ptr_extend pattern. Do not rely on sign extension.
//
// Note that I only looked into the GCC documentation of versions
// between v4.0.0 and v4.9.0.
#if GCC_VERSION < 40000 || GCC_VERSION > 40900
#define CONFIG_PTR_TO_INT_IDENTOP 0
#define CONFIG_INT_TO_PTR_IDENTOP 0
#else
#define CONFIG_PTR_TO_INT_IDENTOP 1
#define CONFIG_INT_TO_PTR_IDENTOP 1
#endif

//
// CONFIG_VA_END_NOOP
//

// stev: CONFIG_VA_END_NOOP establishes whether the compiler does
// not require that invocations of 'va_start' macro be matched by
// a corresponding 'va_end' call -- in other words, the parameter
// shall evaluate to 'true' iff 'va_end' is a no-operation macro
// (or function). Note that the C standard (C99 7.15.1) says that
// each call to 'va_start' shall be paired by a call to 'va_end'.

// stev: the implementation of GCC v4.x (0 <= x <= 9) boils down
// the 'va_end' macro to a built-in function '__builtin_va_end'.
// This function is indeed a trivial no-operation (look into the
// source code gcc/builtins.{def,c}). Other versions of GCC might
// well implement the same behaviour -- but, since this issue is
// a tricky one, CONFIG_VA_END_NOOP should only be set to 'true'
// following a positive assurance about the things implied.
#if GCC_VERSION < 40000 || GCC_VERSION > 40900
#define CONFIG_VA_END_NOOP 0
#else
#define CONFIG_VA_END_NOOP 1
#endif

#endif /* CONFIG_H */

