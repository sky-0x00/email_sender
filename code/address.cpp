#include "stdafx.h"
#include "address.h"

#include <windows.h>
#include <vector>
#include <iomanip>
//#include <iostream>

//---------------------------------------------------------------------------------------------------------------------
#define EMAIL_LOG__DELIM_ITEMS_CHAR			';'
#define EMAIL_LOG__SEPARATOR_LINE_SIZE		120
#define EMAIL_LOG__SEPARATOR_LINE_CHAR		'-'

//---------------------------------------------------------------------------------------------------------------------
/*static*/ std::string address::email_log::get_last_email(
	_in cstr_t f_name
) {
	std::ifstream f_stream( f_name, std::ios::ate );
	
	// файл не найден - возвратим пустую строку
	if ( !f_stream.is_open() )
	{
		const auto error = GetLastError();
		TRACE_ERROR( L"file \"%s\" not opened: %i", f_name, error );
		return "";
	}

	// файл найден - возвратим последний email
	const auto size = f_stream.tellg().seekpos();
	std::vector< ansichar_t > result_v;
	
	for ( fpos_t i = 0; -i < size; )
	{
		f_stream.seekg( --i, std::ios::end );
		const auto ch = static_cast<ansichar_t>( f_stream.get() );
		switch ( ch )
		{
		case '\n':
			if ( !result_v.empty() )
				goto for_break;
			break;
		
		case ' ':
		case '\t':
		case '-':
			break;

		default:
			result_v.push_back( ch );
		}
	}

for_break:
	f_stream.close();

	if ( result_v.empty() )
		return "";

	std::string result( result_v.rbegin(), result_v.rend() );
	auto ch_pos = std::make_pair( result.find( EMAIL_LOG__DELIM_ITEMS_CHAR ), 0 );
	assert ( ch_pos.first != std::string::npos );
	
	ch_pos.second = result.find( EMAIL_LOG__DELIM_ITEMS_CHAR, ++ch_pos.first );
	assert ( ch_pos.second != std::string::npos );
	
	return std::string( result.c_str() + ch_pos.first, ch_pos.second - ch_pos.first );
}

address::email_log::email_log( 
	_in cstr_t f_name 
) {
	assert( !m_fstream.is_open() );
	m_fstream.open( f_name, std::ios::out | std::ios::app );
	if ( m_fstream.is_open() )
	{
		m_fstream << std::string( EMAIL_LOG__SEPARATOR_LINE_SIZE, EMAIL_LOG__SEPARATOR_LINE_CHAR ) << std::endl;
		return;
	}
	
	const auto error = GetLastError();
	TRACE_ERROR( L"file \"%s\" not opened: %i", f_name, error );
	throw std::logic_error( "can't open email_log" );
}

void address::email_log::trace(
	_in ansicstr_t email,
	_in const guid &guid,
	_in ansicstr_t message_id
) {
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );

	std::wstring guid_str = guid.to_string();

	m_fstream << std::setw(4) << std::setfill('0') << SystemTime.wYear << '-'
		<< std::setw(2) << std::setfill('0') << SystemTime.wMonth << '-'
		<< std::setw(2) << std::setfill('0') << SystemTime.wDay << ' '
		<< std::setw(2) << std::setfill('0') << SystemTime.wHour << ':'
		<< std::setw(2) << std::setfill('0') << SystemTime.wMinute << ':'
		<< std::setw(2) << std::setfill('0') << SystemTime.wSecond << '.'
		<< std::setw(3) << std::setfill('0') << SystemTime.wMilliseconds << EMAIL_LOG__DELIM_ITEMS_CHAR

		<< email << EMAIL_LOG__DELIM_ITEMS_CHAR 
		<< string::convert::to_ansi( guid_str.c_str(), string::codepage::win1251 ) << EMAIL_LOG__DELIM_ITEMS_CHAR << message_id 
		<< std::endl;
}

//---------------------------------------------------------------------------------------------------------------------
address::email_base::email_base(
	_in cstr_t f_name,
	_in ansicstr_t last_email
) {
	if ( !open_base( f_name ) )
		throw std::logic_error( "can't open email base" );

	TRACE_NORMAL( L"email base opened from file \"%s\"", f_name );
	if ( !last_email || (*last_email == '\0') )
	{
		TRACE_NORMAL( L"start from base beginning..." );
		return;
	}

	if ( goto_email( last_email ) )
	{
		TRACE_NORMAL( L"start from last email, next after \"%S\"...", last_email );
		return;
	}

	TRACE_ERROR( L"start from base beginning (email \"%S\" not found)...", last_email );
	m_fstream.clear();
	m_fstream.seekg( 0 );	

	//auto good = m_fstream.good();
	//UNREFERENCED_PARAMETER( good );
	//auto fail = m_fstream.fail();
	//UNREFERENCED_PARAMETER( fail );
	//auto eof = m_fstream.eof();
	//UNREFERENCED_PARAMETER( eof );
	//auto bad = m_fstream.bad();
	//UNREFERENCED_PARAMETER( bad );
}

bool address::email_base::open_base( 
	_in cstr_t f_name 
) {
	assert( !m_fstream.is_open() );
	m_fstream.open( f_name );
	if ( m_fstream.is_open() )
		return true;
	
	const auto error = GetLastError();
	TRACE_ERROR( L"file \"%s\" not opened: %i", f_name, error );
	return false;
}

bool address::email_base::goto_email(
	_in ansicstr_t last_email
) {
	do {
		const auto &fstream_line = stdex::ifstream_getline( m_fstream );
		if ( 0 == _stricmp( last_email, fstream_line.c_str() ) )
		{
			TRACE_NORMAL( L"next( %S )", fstream_line.c_str() );
			return true;
		}
	} 
	while ( !m_fstream.eof() );

	//auto good = m_fstream.good();
	//UNREFERENCED_PARAMETER( good );
	//auto fail = m_fstream.fail();
	//UNREFERENCED_PARAMETER( fail );
	//auto eof = m_fstream.eof();
	//UNREFERENCED_PARAMETER( eof );
	//auto bad = m_fstream.bad();
	//UNREFERENCED_PARAMETER( bad );
	return false;
}

std::string address::email_base::get_next(
) {
	const auto &result = stdex::ifstream_getline( m_fstream );
	TRACE_NORMAL( L"%S", result.c_str() ); 
	return result;
}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------