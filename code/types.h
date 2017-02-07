#pragma once

#define _in
#define _out
#define _option

//#include <stdarg.h>
#include <stdexcept>

typedef unsigned char byte_t;
typedef unsigned short ushort_t;

typedef byte_t *data_t;
typedef const byte_t *cdata_t;

typedef wchar_t char_t;
typedef char_t *str_t;
typedef const char_t *cstr_t;

typedef char ansichar_t;
typedef ansichar_t *ansistr_t;
typedef const ansichar_t *ansicstr_t;

#define STRINGIZE(x)		#x

#define assert(x)								\
if ( !(x) )										\
{												\
	TRACE_ERROR( L"assert: %S", #x );			\
	throw std::logic_error( "assert: " #x );	\
}

// функция проверки вхождения переменной x в диапазон [range_a, range_b]
template < typename type >
bool in_range( 
	_in const type &x, 
	_in const type &range_a, 
	_in const type &range_b 
) {
	//assert( range_a <= range_b );
	return ( x >= range_a ) && ( x <= range_b );
}

template < typename type >
bool in_range( 
	_in const type &x, 
	_in const std::pair<type, type> &range
) {
	return in_range( x, range.first, range.second );
}


//class va_args
//{
//public:
//	va_args( _in const void *arg )
//	{
//		va_start( m_args, arg );
//	}
//	~va_args()
//	{
//		va_end( m_args );
//	}
//	operator va_list()
//	{
//		return m_args;
//	}
//
//private:
//	va_list m_args;
//};
