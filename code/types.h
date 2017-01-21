#pragma once

#define _in
#define _out

//#include <stdarg.h>
#include <stdexcept>

typedef char byte_t;
typedef wchar_t char_t;

typedef char_t *str_t;
typedef const char_t *cstr_t;

typedef byte_t *data_t;
typedef const byte_t *cdata_t;

#define assert(x)								\
if (!(x))										\
{												\
	TRACE_ERROR( L"assert: %S", #x );			\
	throw std::logic_error( "assert: " #x );	\
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
