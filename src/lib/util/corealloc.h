// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    corealloc.h

    Memory allocation helpers for the helper library.

***************************************************************************/

#pragma once

#ifndef __COREALLOC_H__
#define __COREALLOC_H__

#include <stdlib.h>
#include <new>
#include <type_traits>
#include <utility>
#include <memory>
#include "osdcore.h"


//**************************************************************************
//  MACROS
//**************************************************************************

// global allocation helpers -- use these instead of new and delete
#define global_alloc(_type)                         new _type
#define global_alloc_nothrow(_type)                 new (std::nothrow) _type
#define global_alloc_array(_type, _num)             new _type[_num]
#define global_alloc_array_nothrow(_type, _num)     new (std::nothrow) _type[_num]
#define global_free(_ptr)                           do { delete _ptr; } while (0)
#define global_free_array(_ptr)                     do { delete[] _ptr; } while (0)

#if defined(__APPLE__)
namespace std
{
	template<class T>
	using remove_extent_t = typename remove_extent<T>::type;
}
#endif

template<typename _Tp, typename... _Args>
inline _Tp* global_alloc_clear(_Args&&... __args)
{
	unsigned char * ptr = new unsigned char[sizeof(_Tp)]; // allocate memory
	memset(ptr, 0, sizeof(_Tp));
	return new(ptr) _Tp(std::forward<_Args>(__args)...);
}

template<typename _Tp>
inline _Tp* global_alloc_array_clear(size_t __num)
{
	auto size = sizeof(_Tp) * __num;
	unsigned char* ptr = new unsigned char[size]; // allocate memory
	memset(ptr, 0, size);
	return new(ptr) _Tp[__num]();
}

#if defined(__APPLE__) || defined(NOTCPP14)
namespace std
{
	template<class T>
	using remove_extent_t = typename remove_extent<T>::type;

	template <bool B, class T = void>
	using enable_if_t = typename enable_if<B, T>::type;

	template<class T>
	using make_signed_t = typename make_signed<T>::type;

	template<class T>
	using make_unsigned_t = typename make_unsigned<T>::type;

	template<class T>
	using remove_const_t = typename remove_const<T>::type;

	template<class T>
	using remove_volatile_t = typename remove_volatile<T>::type;

	template<class T>
	using remove_pointer_t = typename remove_pointer<T>::type;

	template<class T>
	using remove_cv_t = typename remove_cv<T>::type;

	template<class T>
	using remove_reference_t = typename remove_reference<T>::type;

	template<class T>
	using add_const_t = typename add_const<T>::type;
}
#endif

template<typename _Tp>
struct _MakeUniqClear
{
	typedef std::unique_ptr<_Tp> __single_object;
};

template<typename _Tp>
struct _MakeUniqClear<_Tp[]>
{
	typedef std::unique_ptr<_Tp[]> __array;
};

template<typename _Tp, size_t _Bound>
struct _MakeUniqClear<_Tp[_Bound]>
{
	struct __invalid_type { };
};

/// make_unique_clear for single objects
template<typename _Tp, typename... _Args>
inline typename _MakeUniqClear<_Tp>::__single_object make_unique_clear(_Args&&... __args)
{
	unsigned char* ptr = new unsigned char[sizeof(_Tp)]; // allocate memory
	memset(ptr, 0, sizeof(_Tp));
	return std::unique_ptr<_Tp>(new(ptr) _Tp(std::forward<_Args>(__args)...));
}

/// make_unique_clear for arrays of unknown bound
template<typename _Tp>
inline typename _MakeUniqClear<_Tp>::__array make_unique_clear(size_t __num)
{
	auto size = sizeof(std::remove_extent_t<_Tp>) * __num;
	unsigned char* ptr = new unsigned char[size]; // allocate memory
	memset(ptr, 0, size);
	return std::unique_ptr<_Tp>(new(ptr) std::remove_extent_t<_Tp>[__num]());
}

template<typename _Tp, unsigned char _F>
inline typename _MakeUniqClear<_Tp>::__array make_unique_clear(size_t __num)
{
	auto size = sizeof(std::remove_extent_t<_Tp>) * __num;
	unsigned char* ptr = new unsigned char[size]; // allocate memory
	memset(ptr, _F, size);
	return std::unique_ptr<_Tp>(new(ptr) std::remove_extent_t<_Tp>[__num]());
}

/// Disable make_unique_clear for arrays of known bound
template<typename _Tp, typename... _Args>
inline typename _MakeUniqClear<_Tp>::__invalid_type make_unique_clear(_Args&&...) = delete;

#endif  /* __COREALLOC_H__ */
