#include "stdafx.h"
#include "address.h"

#include <windows.h>
#include <vector>

std::string address::get_last_email(
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
	std::vector< ansichar_t > result;
	
	for ( fpos_t i = 0; i < size; )
	{
		f_stream.seekg( --i, std::ios::end );
		const auto ch = static_cast<ansichar_t>( f_stream.get() );
		switch ( ch )
		{
		case '\n':
			if ( !result.empty() )
				goto for_break;
			break;
		
		case ' ':
		case '\t':
			break;

		default:
			result.push_back( ch );
		}
	}

for_break:
	f_stream.close();

	return std::string( result.rbegin(), result.rend() );
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
		m_fstream.getline( m_buffer.data(), m_buffer.size() );
		if ( 0 == _stricmp( last_email, m_buffer.data() ) )
			return true;
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

ansicstr_t address::email_base::get_next(
) {
	m_fstream.getline( m_buffer.data(), m_buffer.size() );
	return m_buffer.data();
}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------