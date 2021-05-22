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

#ifndef __STD_EXT_HPP
#define __STD_EXT_HPP

#include <memory>
#include <utility>
#include <type_traits>

#include "sys.hpp"

namespace Ext {

template<typename R, typename A>
struct unary_func_t
{
    typedef R result_type;
    typedef A argument_type;
};

template<typename R, typename A0, typename A1 = A0>
struct binary_func_t
{
    typedef R result_type;
    typedef A0 first_argument_type;
    typedef A1 second_argument_type;
};

template<typename T>
constexpr bool is_integer()
{ return std::numeric_limits<T>::is_integer; }

template<typename T>
constexpr bool is_signed()
{ return std::numeric_limits<T>::is_signed; }

template<typename T>
constexpr bool is_unsigned()
{ return !is_signed<T>(); }

template<typename T>
constexpr bool is_signed_int()
{ return is_integer<T>() && is_signed<T>(); }

template<typename T>
constexpr bool is_unsigned_int()
{ return is_integer<T>() && !is_signed<T>(); }

template<typename T>
constexpr bool is_enum()
{ return std::is_enum<T>::value; }

template<typename t>
constexpr bool is_integer_or_enum()
{ return is_integer<t>() || is_enum<t>(); }

template<typename T>
constexpr bool is_integral()
{ return std::is_integral<T>::value; }

template<typename T>
constexpr bool is_floating_point()
{ return std::is_floating_point<T>::value; }

template<typename T>
constexpr bool is_arithmetic()
{ return std::is_arithmetic<T>::value; }

template<typename T>
constexpr bool is_pointer()
{ return std::is_pointer<T>::value; }

template<typename T>
constexpr bool is_array()
{ return std::is_array<T>::value; }

template<typename T>
constexpr bool is_ref()
{ return std::is_reference<T>::value; }

template<typename T>
constexpr bool is_const()
{ return std::is_const<T>::value; }

template<typename T>
constexpr bool is_const_ref()
{ return is_const<T>() && is_ref<T>(); }

template<typename B, typename D>
constexpr bool is_base_of()
{ return std::is_base_of<B, D>::value; }

template<typename T>
constexpr bool is_compound()
{ return std::is_compound<T>::value; }

template<typename T>
constexpr bool is_member_function_pointer()
{ return std::is_member_function_pointer<T>::value; }

template<typename T, typename U>
constexpr bool is_same()
{ return std::is_same<T, U>::value; }

template<typename T, typename U>
constexpr bool is_cv_same()
{
    return is_same<
        typename std::remove_cv<T>::type,
        typename std::remove_cv<U>::type>();
}

template<typename T>
constexpr bool is_char_pointer()
{
    typedef
        typename std::remove_pointer<T>::type
        base_t;
    return
        is_pointer<T>()
        && (is_cv_same<base_t, char>()
            || is_cv_same<base_t, unsigned char>()
            || is_cv_same<base_t, signed char>());
}

template<typename T>
constexpr T min()
{ return std::numeric_limits<T>::min(); }

template<typename T>
constexpr T max()
{ return std::numeric_limits<T>::max(); }

template<typename T>
constexpr size_t digits()
{ return std::numeric_limits<T>::digits; }

template<typename T>
constexpr size_t digits10()
{ return std::numeric_limits<T>::digits10; }

template<typename T>
constexpr typename std::enable_if<
    is_integer<T>(),
bool>::type
    sign()
{ return is_signed<T>(); }

template<typename T>
constexpr typename std::enable_if<
    is_integer<T>(),
size_t>::type
    width()
{ return digits<T>(); }

template<typename R, typename V>
inline typename std::enable_if<
    (width<R>() == width<V>()),
bool>::type
    check_limits(V v UNUSED)
{
    return true;
}

template<typename R, typename V>
inline typename std::enable_if<
    (sign<R>() == sign<V>()) &&
    (width<R>() > width<V>()),
bool>::type
    check_limits(V v UNUSED)
{
    return true;
}

template<typename R, typename V>
inline typename std::enable_if<
    (sign<R>() == false) &&
    (sign<V>() == false) &&
    (width<R>() < width<V>()),
bool>::type
    check_limits(V v)
{
    return
        v <= max<R>();
}

template<typename R, typename V>
inline typename std::enable_if<
    (sign<R>() == true) &&
    (sign<V>() == true) &&
    (width<R>() < width<V>()),
bool>::type
    check_limits(V v)
{
    return
        v >= min<R>() &&
        v <= max<R>();
}

template<typename R, typename V>
inline typename std::enable_if<
    (sign<R>() == false) &&
    (sign<V>() == true) &&
    (width<R>() > width<V>()),
bool>::type
    check_limits(V v)
{
    CXX_ASSERT(
        //!!!CAST
        min<R>() == 0 &&
        V(min<R>()) > min<V>());
    return
        //!!!CAST
        v >= V(min<R>());
}

template<typename R, typename V>
inline typename std::enable_if<
    (sign<R>() == true) &&
    (sign<V>() == false) &&
    (width<R>() > width<V>()),
bool>::type
    check_limits(V v UNUSED)
{
    return true;
}

template<typename R, typename V>
inline typename std::enable_if<
    (sign<R>() == false) &&
    (sign<V>() == true) &&
    (width<R>() < width<V>()),
bool>::type
    check_limits(V v)
{
    CXX_ASSERT(
        //!!!CAST
        min<R>() == 0 &&
        max<R>() > 0 &&
        V(min<R>()) > min<V>() &&
        V(max<R>()) < max<V>());
    return
        //!!!CAST
        v >= V(min<R>()) &&
        v <= V(max<R>());
}

template<typename R, typename V>
inline typename std::enable_if<
    (sign<R>() == true) &&
    (sign<V>() == false) &&
    (width<R>() < width<V>()),
bool>::type
    check_limits(V v)
{
    CXX_ASSERT(
        //!!!CAST
        max<R>() > 0 &&
        V(max<R>()) < max<V>());
    return
        //!!!CAST
        v <= V(max<R>());
}

template<typename R, typename V>
inline typename std::enable_if<
    is_integer<V>() &&
    is_integer<R>(),
R>::type
    integer_cast(V v)
{
    SYS_ASSERT(
        check_limits<R>(v));
    return
        static_cast<R>(v);
}

template<typename R, typename V>
inline typename std::enable_if<
    is_integer<V>() &&
    is_integer<R>(),
bool>::type
    integer_cast(V v, R& r)
{
    if (check_limits<R>(v)) {
        r = static_cast<R>(v);
        return true;
    }
    return false;
}

template<typename R, typename V>
inline typename std::enable_if<
    is_enum<V>() &&
    is_integer<R>(),
R>::type
    integer_cast(V v)
{
    typedef
        typename
            std::underlying_type<V>::type
        enum_t;
    CXX_ASSERT(
        is_integer<enum_t>());
    return
        integer_cast<R>(
            static_cast<enum_t>(v));
}

template<typename R, typename V>
inline typename std::enable_if<
    is_enum<V>() &&
    is_integer<R>(),
bool>::type
    integer_cast(V v, R& r)
{
    typedef
        typename
            std::underlying_type<V>::type
        enum_t;
    CXX_ASSERT(
        is_integer<enum_t>());
    return
        integer_cast<R>(
            static_cast<enum_t>(v),
            r);
}

template<typename R, typename V>
inline typename std::enable_if<
    is_integer<V>() &&
    is_enum<R>(),
R>::type
    enum_cast(V v)
{
    typedef
        typename
            std::underlying_type<R>::type
        enum_t;
    CXX_ASSERT(
        is_integer<enum_t>());
    return
        static_cast<R>(
            integer_cast<enum_t>(v));
}

template<typename R, typename V>
inline typename std::enable_if<
    is_integer<V>() &&
    is_enum<R>(),
bool>::type
    enum_cast(V v, R& r)
{
    typedef
        typename
            std::underlying_type<R>::type
        enum_t;
    CXX_ASSERT(
        is_integer<enum_t>());
    enum_t e;
    if (integer_cast<enum_t>(v, e)) {
        r = static_cast<R>(e);
        return true;
    }
    return false;
}

template<typename T>
inline size_t ptr_diff(const T* p, const T* q)
{ return integer_cast<size_t>(p - q); }

template<typename T>
inline uintptr_t ptr_to_int(const T* v)
{
    CXX_ASSERT(
        CONFIG_PTR_TO_INT_IDENTOP &&
        is_unsigned_int<uintptr_t>() &&
        sizeof(T*) == sizeof(uintptr_t));
    return reinterpret_cast<uintptr_t>(v);
}

template<typename T, typename V>
inline T* int_to_ptr(V v)
{
    // stev: the static assertion below
    // ensures that 'int_to_ptr' avoids
    // narrowing casts on its input arg
    CXX_ASSERT((
        is_cv_same<V, uintptr_t>()));
    CXX_ASSERT(
        CONFIG_INT_TO_PTR_IDENTOP &&
        is_unsigned_int<uintptr_t>() &&
        sizeof(T*) == sizeof(uintptr_t));
    return reinterpret_cast<T*>(v);
}

template<typename T>
class box_t final
{
public:
    typedef
        typename
            std::enable_if<
                is_integer<T>(),
                T>::type
            type_t;

    box_t(box_t&&) = default;
    box_t(const box_t&) = default;

    box_t& operator=(box_t&&) = default;
    box_t& operator=(const box_t&) = default;

    // stev: the constructors below
    // is not 'explicit' on purpose

    //!!!ENABLE_IF
    template<typename V>
    box_t(box_t<V> a) :
        v{integer_cast<type_t>(a.v)}
    {}

    //!!!ENABLE_IF
    //template<typename V>
    //box_t(
    //	typename std::enable_if<
    //		!is_cv_same<T, V>(),
    //		box_t<V>>::type a) :
    //	v{integer_cast<type_t>(a.v)}
    //{}

    //!!!ENABLE_IF
    template<typename V>
    box_t(V a) :
        v{integer_cast<type_t>(a)}
    {
        CXX_ASSERT(
            is_integer_or_enum<V>());
    }

    //!!!ENABLE_IF
    //template<typename V>
    //box_t(
    //	typename std::enable_if<
    //		is_integer_or_enum<V>(),
    //		V>::type a) :
    //	v{integer_cast<type_t>(a)}
    //{}

    template<typename V>
    typename std::enable_if<
        is_integer_or_enum<V>(),
        box_t&>::type
    operator=(V a)
    {
        v = integer_cast<type_t>(a);
        return *this;
    }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t&>::type
    operator=(box_t<V> a)
    { return operator=(a.v); }

    //!!!ENABLE_IF
    //template<typename V>
    //operator typename std::enable_if<
    //	is_integer_or_enum<V>(),
    //	V>::type () const
    //{ return integer_cast<V>(v); }

    //!!!ENABLE_IF
    template<typename V>
    operator V() const
    { return integer_cast<V>(v); }

    template<typename V>
    typename std::enable_if<
        is_integer_or_enum<V>(),
        V>::type
    as() const
    { return integer_cast<V>(v); }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t<V>>::type
    as_box() const
    { return box_t<V>{v}; }

    type_t as() const
    { return v; }

    type_t operator()() const
    { return v; }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t>::type
    operator-(V a) const
    { return box_t{v} -= a; }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t>::type
    operator-(box_t<V> a)
    { return operator-(a.v); }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t>::type
    operator+(V a) const
    { return box_t{v} += a; }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t>::type
    operator+(box_t<V> a)
    { return operator+(a.v); }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t&>::type
    operator-=(V a)
    {
        v -= integer_cast<type_t>(a);
        return *this;
    }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t&>::type
    operator-=(box_t<V> a)
    { return operator-=(a.v); }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t&>::type
    operator+=(V a)
    {
        v += integer_cast<type_t>(a);
        return *this;
    }

    template<typename V>
    typename std::enable_if<
        is_integer<V>(),
        box_t&>::type
    operator+=(box_t<V> a)
    { return operator+=(a.v); }

    box_t& operator--()
    { v --; return *this; }

    box_t& operator++()
    { v --; return *this; }

    box_t operator--(int)
    { box_t r{v}; v --; return r; }

    box_t operator++(int)
    { box_t r{v}; v ++; return r; }

private:
    type_t v;
};

template<unsigned N, typename T>
inline size_t digits(T v)
{
    CXX_ASSERT(
        is_unsigned_int<T>() &&
        N > 0);
    size_t r = 0;
    do {
        v /= N;
        r ++;
    }
    while (v > 0);
    return r;
}

typedef long long widest_sint_t;
typedef unsigned long long widest_uint_t;

template<typename T>
constexpr bool is_widest_int()
{
    return
        is_cv_same<T, widest_sint_t>() ||
        is_cv_same<T, widest_uint_t>();
}

template<typename T>
struct widest_int_t
{
    typedef
        typename std::enable_if<
            is_integer<T>(),
            typename
                std::conditional<
                    is_unsigned<T>(),
                    widest_uint_t,
                    widest_sint_t
                >::type
            >::type
        type;
};

template<size_t N, typename T>
constexpr typename std::enable_if<
    is_unsigned_int<T>(),
T>::type
    max()
{
    CXX_ASSERT(
        N < digits<T>());
    return (T(1) << N) - 1;
}

template<size_t N, typename T>
inline typename std::enable_if<
    is_unsigned_int<T>(),
bool>::type
    check_bits_cast(T v)
{ return v <= max<N, T>(); }

template<size_t N, typename T>
inline typename std::enable_if<
    is_unsigned_int<T>(),
    typename widest_int_t<T>::type
>::type
    bits_cast(T v)
{
    typedef
        typename widest_int_t<T>::type
        result_t;
    SYS_ASSERT((
        check_bits_cast<N, T>(v)));
    return
        static_cast<result_t>(v);
}

template<size_t N, typename T>
inline typename std::enable_if<
    is_unsigned_int<T>(),
bool>::type
    bits_cast(T v, typename widest_int_t<T>::type& r)
{
    typedef
        typename widest_int_t<T>::type
        result_t;
    if (check_bits_cast<N, T>(v)) {
        r = static_cast<result_t>(v);
        return true;
    }
    return false;
}

extern bool parse_num(
    const char* str, widest_sint_t& res, const char*& end);
extern bool parse_num(
    const char* str, widest_uint_t& res, const char*& end);

template<typename T>
inline typename std::enable_if<
    !is_widest_int<T>() &&
    is_integer<T>(),
bool>::type
    parse_num(const char* str, T& res, const char*& end)
{
    typedef
        typename widest_int_t<T>::type
        wider_t;
    wider_t val;
    if (!parse_num(str, val, end))
        return false;
    if (!integer_cast(val, res)) {
        errno = ERANGE;
        return false;
    }
    return true;
}

template<typename T>
inline bool
    parse_num(const char* str, T& res)
{
    const char* end;
    if (!parse_num(str, res, end))
        return false;
    if (*end) {
        errno = EINVAL;
        return false;
    }
    return true;
}

// stev: Bjarne Stroustrup, TC++PL 4th edition,
//       34.3.1 unique_ptr, p. 990 (corrected)

template<typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args&&... args)
{ return std::unique_ptr<T>{new T{std::forward<Args>(args)...}}; }

// stev: a simple and useful wrapper class for arrays:
// it provides a range-checked  'operator[]'; the most
// relevant usage patterns are those of wrapping around
// the access to elements of statically defined arrays:
//
//   static void (*const funcs[])(std::ostream&) = {
//     f0, f1, f2
//   };
//   static void (*const funcs2[][3])(std::ostream&) = {{
//     f0, f1, f2
//   },{
//     f3, f4, f5
//   }};
//   array(funcs)[k](std::cout);
//   array(funcs2)[i][j](std::cout);
//
// Note that in the last two expression statements above
// the access to 'funcs' at index 'k' and, respectively,
// each of the accesses into 'funcs2' at indexes 'i' and
// subsequently 'j' gets range-checked in runtime.
//
// Another feature of this array class is that accesses
// to its elements via 'operator[]' member function are
// range checked against the underlying indexing type.
// For example, the access into 'funcs', with the index
// being the additive inverse of 'max<array_index_t>()'
// taken as 'long long', aborts the program:
//
//   const long long x = - static_cast<long long>(
//     Ext::max<array_index_t>());
//   CXX_ASSERT(static_cast<array_index_t>(x) == 1);
//   array(funcs)[x](std::cout);
//
// This is so due to an assertion failure in function
// 'integer_cast<array_index_t, long long>' which gets
// called in the one of the constructors of 'Ext::box_t'.
// Should the type of the argument of 'operator[]' member
// function of class 'array_t' have instead been defined
// as 'array_index_t', by the implicit conversion rules
// the 'long long' 'x' would have silently been cast as
// the value '1' prior to the 'operator[]' function call
// -- which value, unfortunately, is a valid index into
// the 'funcs' array.
//
// Yet another feature of this array class having boxed
// indexing acceses to its elements -- feature which is
// derived from the functionality of 'integer_cast<R,V>'
// template function -- is that its indexing values can
// be enumerators:
//
//   enum struct E { w = 2, x };
//
//   array(funcs)[E::w](std::cout);
//   array(funcs)[E::x](std::cout);
//
// The first access into 'funcs' above is valid since
// the value 'E::w' would be boxed as the index value
// '2'. On the other hand, the second access into the
// array is aborting the program: boxing 'E::x' calls
// for 'integer_cast<array_index_t, E>(E::x)', which
// gets out the value '3', value which is out of the
// bounds of 'funcs' array.

// ISO/IEC 9899:1999(TC3) and 9899:2011
// 6.3 Conversions
// 6.3.1.1 Boolean, characters, and integers
// 6.3.1.3 Signed and unsigned integers
// 6.5.2.2 Function calls
// 6.5.2.2.7 (implicit argument conversions)

// ISO/IEC 14882:2003(E) and 14882:2011
// 4.5 Integral promotions
// 4.7 Integral conversions
// 5.2.2 Function call
// 5.2.2.7 (default argument promotions)
// 13.3.3.1 Implicit conversion sequences

// ISO/IEC 9899:1999(TC3) and 9899:2011
// 6.5.2.1 Array subscripting
// 6.5.2.1.3 (successive subscript operators)

// ISO/IEC 14882:2003(E) and 14882:2011
// 8.3.4 Arrays
// 8.3.4.7 (multidimensional arrays)

typedef unsigned int array_index_t;

template<typename T, array_index_t N>
class array_t final
{
public:
    typedef T elem_t;
    typedef T type_t[N];

    typedef box_t<array_index_t> index_t;

    explicit array_t(type_t& _a) : a{_a} {}

    elem_t operator[](index_t i) const
    { SYS_ASSERT(i() < N); return a[i()]; }

private:
    elem_t* a;
};

template<typename T, array_index_t N, array_index_t M>
class array_t<T[M], N> final
{
public:
    typedef array_t<T, M> elem_t;
    typedef T type_t[N][M];

    typedef box_t<array_index_t> index_t;

    explicit array_t(type_t& _a) : a{_a} {}

    elem_t operator[](index_t i) const
    { SYS_ASSERT(i() < N); return elem_t(a[i()]); }

private:
    typedef T inner_t[M];
    inner_t* a;
};

template<typename T, size_t N>
inline array_t<T, N> array(T (&v)[N])
{ return array_t<T, N>(v); }

template<typename T, size_t N>
constexpr size_t array_size(T (&v)[N] UNUSED)
{ return N; }

template <typename T>
class Repr;

template <typename T>
inline std::ostream& operator<<(
    std::ostream& ost, const Repr<T>& repr)
{ repr.print(ost); return ost; }

template<typename T>
class ReprPrint
{
public:
    explicit ReprPrint(std::ostream& _ost) :
        ost(_ost)
    {}

    void operator()(const T& val)
    { ost << val; }

private:
    std::ostream& ost;
};

enum struct char_repr_type_t {
    quoted,
    plain
};

enum struct str_repr_type_t {
    string,
    plain
};

template<>
class ReprPrint<char>
{
public:
    explicit ReprPrint(std::ostream& _ost) :
        ost(_ost)
    {}

    size_t operator()(char ch, str_repr_type_t type)
    {
        const auto str = type == str_repr_type_t::string;

        switch (ch) {
        case '\\':
            return print_out_quoted('\\');
        case '\f':
            return print_out_quoted('f');
        case '\n':
            return print_out_quoted('n');
        case '\r':
            return print_out_quoted('r');
        case '\t':
            return print_out_quoted('t');
        case '\v':
            return print_out_quoted('v');
        case '"':
            if (str)
                return print_out_quoted('"');
            else
                return print_out(ch);
        case '\'':
            if (!str)
                return print_out_quoted('\'');
            else
                return print_out(ch);
        default:
            if (isascii(ch) && !iscntrl(ch))
                return print_out(ch);
            else
                return print_out_hex(ch, str);
        }
    }

    size_t operator()(char ch, char_repr_type_t type)
    {
        const auto q = type == char_repr_type_t::quoted;
        size_t n = 0;

        if (q)
            n += print_out('\'');

        n += operator()(ch, q
            ? str_repr_type_t::plain
            : str_repr_type_t::string);

        if (q)
            n += print_out('\'');

        return n;
    }

private:
    size_t print_out(char ch)
    { ost << ch; return 1; }

    size_t print_out_quoted(char ch)
    { ost << '\\' << ch; return 2; }

    size_t print_out_hex(char ch, bool str)
    {
        using namespace std;
        auto f = ost.fill();
        auto w = ost.width();
        auto m = ost.flags();
        auto c = static_cast<unsigned char>(ch); 
        ost << "\\x"
            << hex
            << right
            << setfill('0')
            << setw(1 + str)
            << integer_cast<unsigned>(c)
            << setfill(f)
            << setw(w);
        ost.flags(m);
        return 3 + (str || c >= '\x10');
    }

    std::ostream& ost;
};

template<>
struct ReprPrint<const char> : public ReprPrint<char>
{
    typedef ReprPrint<char> base_t;

    explicit ReprPrint(std::ostream& _ost) :
        base_t(_ost)
    {}
};

template<typename T>
class ReprPrint<T*>
{
public:
    explicit ReprPrint(std::ostream& _ost) :
        ost(_ost)
    {}

    void operator()(T* beg, T* end, bool str)
    {
        ReprPrint<T> repr(ost);
        if (str)
            ost << '"';
        std::for_each(
            beg, end, [&](char ch)
                { repr(ch, str
                    ? str_repr_type_t::string
                    : str_repr_type_t::plain); });
        if (str)
            ost << '"';
    }

private:
    std::ostream& ost;
};

template<typename T>
class Repr
{
public:
    explicit Repr(const T& _val) : val(_val) {}

    void print(std::ostream& ost) const
    { ReprPrint<T>{ost}(val); }

private:
    typedef
        typename
            std::conditional<
                is_compound<T>(),
                const T&,
                T
            >::type
        val_t;

    val_t val;
};

template<>
class Repr<char>
{
public:
    Repr(char _ch, char_repr_type_t _type) :
        ch(_ch), type(_type)
    {}

    void print(std::ostream& ost) const
    { ReprPrint<char>{ost}(ch, type); }

private:
    char             ch;
    char_repr_type_t type;
};

template<typename T>
class Repr<T*>
{
public:
    Repr(const T* _beg, const T* _end, bool _str) :
        beg(_beg), end(_end), str(_str)
    {}

    Repr(const T* _beg, size_t _size, bool _str) :
        beg(_beg), end(_beg + _size), str(_str)
    {}

    void print(std::ostream& ost) const
    { ReprPrint<const T*>{ost}(beg, end, str); }

private:
    const T *beg;
    const T *end;
    bool     str;
};

template<typename T>
inline typename std::enable_if<
    !is_char_pointer<T>(),
Repr<T>>::type
repr(const T& val)
{ return Repr<T>{val}; }

template<typename T>
inline Repr<T*> repr(T* beg, T* end, bool str = false)
{ return Repr<T*>{beg, end, str}; }

template<typename T>
inline Repr<T*> repr(T* ptr, size_t size, bool str = false)
{ return Repr<T*>{ptr, size, str}; }

inline Repr<char> repr(char ch, bool quote = false)
{ return Repr<char>{
    ch, quote ? char_repr_type_t::quoted : char_repr_type_t::plain}; }

inline Repr<const char*> repr(const char* ptr, bool str = false)
{ return Repr<const char*>{ptr, ptr ? strlen(ptr) : 0, str}; }

inline Repr<const char*> repr(const std::string& obj, bool str = false)
{ return repr(obj.data(), obj.size(), str); }

inline Repr<const char*> repr(const std::vector<char>& obj, bool str = false)
{ return repr(obj.data(), obj.size(), str); }

} // namespace Ext

#endif /* __STD_EXT_HPP */

