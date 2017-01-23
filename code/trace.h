#pragma once

#include "types.h"
#include <fstream>

#define IMPLEMENT_TRACE( module_name )		trace::tracer tracer( module_name )
#define SETFILE_TRACE( file_name, ... )		tracer.set_filename( file_name, __VA_ARGS__ )
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
		~tracer();

		size_t trace( _in cstr_t function, _in category category, _in cstr_t format, _in ... );
		bool set_filename( _in cstr_t filename, _in bool is_append = false );

	private:
		static const size_t buffer_size = 1024;

		cstr_t m__module_name;
		std::wofstream m__file_stream;
		char_t m__buffer[ buffer_size ];

		size_t buffer_prepare( _in cstr_t function, _in category category, _in cstr_t format, _in va_list va_args );
		void buffer_output__to_debug() const;
		void buffer_output__to_file();
		size_t buffer_get_restsize( _in size_t offset ) const;

		size_t trace_args( _in cstr_t function, _in category category, _in cstr_t format, _in va_list va_args );
	};
}

extern trace::tracer tracer;