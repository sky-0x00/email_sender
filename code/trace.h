#pragma once

#include "types.h"

#define IMPLEMENT_TRACE( module_name )		trace::tracer tracer( module_name )
#define TRACE_NORMAL( format, ... )			tracer.trace( __FUNCTIONW__, trace::category::normal, format, __VA_ARGS__ )
#define TRACE_ERROR( format, ... )			tracer.trace( __FUNCTIONW__, trace::category::error, format, __VA_ARGS__ )

#define ERROR_UNKNOWN	L"Unknown exception"
#define TRACE_ERROR_UNKNOWN( )				TRACE_ERROR( ERROR_UNKNOWN )

namespace trace
{
	enum class category
	{
		normal,
		error
	};

	class tracer
	{
	public:
		tracer( _in cstr_t module_name );

		size_t trace( _in cstr_t function, _in category category, _in cstr_t format, _in ... );

	private:
		static const size_t buffer_size = 1024;

		cstr_t m__module_name;
		char_t m__buffer[ buffer_size ];

		size_t buffer_prepare( _in cstr_t function, _in category category, _in cstr_t format, _in va_list va_args );
		void buffer_output() const;
		size_t buffer_get_restsize( _in size_t offset ) const;

		size_t trace_args( _in cstr_t function, _in category category, _in cstr_t format, _in va_list va_args );
	};
}

extern trace::tracer tracer;