#include "stdafx.h"

#include "trace.h"
#include <stdarg.h>				// va_list
#include <windows.h>			// GetLocalTime()
#include <stdexcept>			// std::runtime_error
#include <map>					// category_traits

trace::tracer::tracer(
	_in cstr_t module_name
) :
	m__module_name(module_name)
{}

size_t trace::tracer::trace(
	_in cstr_t function, 
	_in category category,
	_in cstr_t format, 
	_in ...
) {
	va_list va_args;
	va_start(va_args, format);
	const auto result = trace_args( function, category, format, va_args );
	va_end(va_args);
	return result;
}

size_t trace::tracer::trace_args(
	_in cstr_t function,
	_in category category,
	_in cstr_t format,
	_in va_list va_args
) {
	const auto result = buffer_prepare( function, category, format, va_args );
	buffer_output();

	return result;
}

size_t trace::tracer::buffer_prepare(
	_in cstr_t function,
	_in category category,
	_in cstr_t format,
	_in va_list va_args
) {
	
	size_t offset = 0;
	int result;

	static const std::map<trace::category, cstr_t> category_traits =
	{
		{ category::normal, L""    },
		{ category::error,  L"E> " }		// если строка не пуста, то добавим в конец пробел
	};

	// получаем текущее время
	SYSTEMTIME time_now;
	GetLocalTime( &time_now );

	// и выводим префикс - время, имя модуля, имя функции и тип категории
	result = swprintf_s( 
		m__buffer + offset, buffer_get_restsize( offset ), 
		L"[%04lu-%02lu-%02lu %02lu:%02lu:%02lu.%03lu] [%s] [%s] %s",
		time_now.wYear, time_now.wMonth, time_now.wDay, time_now.wHour, time_now.wMinute, time_now.wSecond, time_now.wMilliseconds,
		m__module_name, function, category_traits.at( category ) );
	if ( result < 0 )
		throw std::runtime_error( "swprintf_s" );
	offset += result;

	// и выводим строку данных
	result = vswprintf_s( m__buffer + offset, buffer_get_restsize( offset ), format, va_args );
	if ( result < 0 )
		throw std::runtime_error( "vswprintf_s" );
	offset += result;

	// завершаем трассировку переходом на новую строку
	result = wcscpy_s( m__buffer + offset, buffer_get_restsize( offset ), L"\n" );
	if ( result )
		throw std::runtime_error( "wcscpy_s" );
	
	return offset;
}

size_t trace::tracer::buffer_get_restsize(
	_in size_t offset
) const
{
	return (buffer_size > offset) ? (buffer_size - offset) : 0;
}

void trace::tracer::buffer_output(
) const
{
	OutputDebugString( m__buffer );
}