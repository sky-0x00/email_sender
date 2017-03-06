#include "stdafx.h"

#include "utils.h"
#include "trace.h"
#include <stdexcept>
#include <winerror.h>

//---------------------------------------------------------------------------------------------------------------------
/*static*/ std::wstring string::convert::to_wide(
	_in ansicstr_t str,
	_in codepage cp /*= codepage::win1251*/
) {
	std::wstring result;

	for ( ansicstr_t p_ch = str; *p_ch; ++p_ch )
	{
		if ( static_cast< byte_t >( *p_ch ) < 0x80 )
		{
			result.push_back( *p_ch );
			continue;
		}
		
		switch ( cp )
		{
		case codepage::win1251:
			result.push_back( to_wide__win1251( *p_ch ) );
			break;

		case codepage::utf8:
			result.push_back( to_wide__utf8( p_ch++ ) );
			break;

		default:
			TRACE_ERROR( L"codepage: %i", static_cast< int >( cp ) );
			throw std::logic_error( NOT_IMPLEMENTED );
		
		}	// switch
	}	// for

	return result;
}

/*static*/ char_t string::convert::to_wide__win1251( 
	_in ansichar_t ch 
) {
	const byte_t &value = ch;
	
	// À..ß à..ß - 0xC0..0xDF 0xE0..0xFF => 0x410..0x42F 0x430..0x44F
	if ( value >= 0xC0 )
		return value + 0x350;

	// ¨, ¸ - 0xA8, 0xB8 => 0x401 (+0x359), 0x451 (+0x399)
	if ( value == 0xA8 )
		return 0x401;
	if ( value == 0xB8 )
		return 0x451;

	TRACE_ERROR( L"ch (ansi): 0x%02X", value );
	throw std::logic_error( NOT_SUPPORTED_CHAR );
}

/*static*/ char_t string::convert::to_wide__utf8(
	_in char_t ch
) {
	const ushort_t &value = ch;
	
	// À..ß à..ï - 0xD090..0xD0AF 0xD0B0..0xD0BF => 0x410..0x42F 0x430..0x43F
	if ( in_range(value , { 0xD090, 0xD0BF } ) )
		return value - 0xCC80;

	// ğ..ÿ - 0xD180..0xD18F => 0x440..0x44F
	if ( in_range(value , { 0xD180, 0xD18F } ) )
		return value - 0xCD40;

	// ¨, ¸ - 0xD081, 0xD191 => 0x401, 0x451
	if ( value == 0xD081 )
		return 0x401;
	if ( value == 0xD191 )
		return 0x451;

	TRACE_ERROR( L"ch (wide): 0x%04X", value );
	throw std::logic_error( NOT_SUPPORTED_CHAR );
}

/*static*/ char_t string::convert::to_wide__utf8( 
	_in ansicstr_t str 
) {
	assert( '\0' != str[1] );
	const char_t ch = static_cast< byte_t >( str[1] ) | (str[0] << 8);
	return to_wide__utf8( ch );
}

//---------------------------------------------------------------------------------------------------------------------
/*static*/ std::string string::convert::to_ansi( 
	_in cstr_t str, 
	_in codepage cp /*= codepage::win1251*/
) {
	std::string result;

	for ( cstr_t p_ch = str; *p_ch; ++p_ch )
	{
		if ( *p_ch < 0x80 )
		{
			result.push_back( static_cast< ansichar_t >( *p_ch ) );
			continue;
		}

		switch ( cp )
		{
		case codepage::win1251:
			result.push_back( to_ansi__win1251( *p_ch ) );
			break;

		case codepage::utf8:
		{
			const auto ch = to_ansi__utf8( *p_ch );			
			result.push_back( (ch >> 8) & 0xFF );
			result.push_back( ch & 0xFF );
		}
			break;

		default:
			TRACE_ERROR( L"codepage: %i", static_cast< int >( cp ) );
			throw std::logic_error( NOT_IMPLEMENTED );

		}	// switch
	}	// for

	return result;
}

#pragma warning( push )
#pragma warning( disable: 4309 )		// C4309: óñå÷åíèå êîíñòàíòíîãî âûğàæåíèÿ

/*static*/ ansichar_t string::convert::to_ansi__win1251( 
	_in char_t ch 
) {
	const ushort_t &value = ch;

	// À..ß à..ß - 0x410..0x42F 0x430..0x44F => 0xC0..0xDF 0xE0..0xFF
	if ( in_range(value , { 0x410, 0x44F } ) )
		return static_cast< ansichar_t >( value - 0x350 );

	// ¨, ¸ - 0x401, 0x451 => 0xA8, 0xB8
	if ( value == 0x401 )
		return 0xA8;
	if ( value == 0x451 )
		return 0xB8;

	TRACE_ERROR( L"ch (wide): 0x%04X", value );
	throw std::logic_error( NOT_SUPPORTED_CHAR );
}
#pragma warning( pop )

/*static*/ char_t string::convert::to_ansi__utf8(
	_in char_t ch
) {
	const ushort_t &value = ch;
	
	// À..ß à..ï - 0x410..0x42F 0x430..0x43F => 0xD090..0xD0AF 0xD0B0..0xD0BF
	if ( in_range(value , { 0x410, 0x43F } ) )
		return value + 0xCC80;

	// ğ..ÿ - 0x440..0x44F => 0xD180..0xD18F
	if ( in_range(value , { 0x440, 0x44F } ) )
		return value + 0xCD40;

	// ¨, ¸ - 0x401, 0x451 => 0xD081, 0xD191
	if ( value == 0x401 )
		return 0xD081;
	if ( value == 0x451 )
		return 0xD191;

	TRACE_ERROR( L"ch (wide): 0x%04X", value );
	throw std::logic_error( NOT_SUPPORTED_CHAR );
}

//---------------------------------------------------------------------------------------------------------------------
/*static*/ void string::convert::test(
) {
	const char_t s_wide[] = L"ABCDEFGHKLMNOPQRSTUVWXYZabcdefghklmnopqrstuvwxyzÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞßàáâãäå¸æçèéêëìíîïğñòóôõö÷øùúûüışÿ";
	const ansichar_t s_ansi_win1251[] = "ABCDEFGHKLMNOPQRSTUVWXYZabcdefghklmnopqrstuvwxyzÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞßàáâãäå¸æçèéêëìíîïğñòóôõö÷øùúûüışÿ";
	const ansichar_t s_ansi_utf8[] = "ABCDEFGHKLMNOPQRSTUVWXYZabcdefghklmnopqrstuvwxyz\
\xD0\x90\xD0\x91\xD0\x92\xD0\x93\xD0\x94\xD0\x95\xD0\x81\xD0\x96\xD0\x97\xD0\x98\xD0\x99\xD0\x9A\xD0\x9B\xD0\x9C\xD0\x9D\xD0\x9E\xD0\x9F\
\xD0\xA0\xD0\xA1\xD0\xA2\xD0\xA3\xD0\xA4\xD0\xA5\xD0\xA6\xD0\xA7\xD0\xA8\xD0\xA9\xD0\xAA\xD0\xAB\xD0\xAC\xD0\xAD\xD0\xAE\xD0\xAF\
\xD0\xB0\xD0\xB1\xD0\xB2\xD0\xB3\xD0\xB4\xD0\xB5\xD1\x91\xD0\xB6\xD0\xB7\xD0\xB8\xD0\xB9\xD0\xBA\xD0\xBB\xD0\xBC\xD0\xBD\xD0\xBE\xD0\xBF\
\xD1\x80\xD1\x81\xD1\x82\xD1\x83\xD1\x84\xD1\x85\xD1\x86\xD1\x87\xD1\x88\xD1\x89\xD1\x8A\xD1\x8B\xD1\x8C\xD1\x8D\xD1\x8E\xD1\x8F";

	test__to_wide__win1251( s_ansi_win1251, s_wide );
	test__to_ansi__win1251( s_ansi_win1251, s_wide );

	test__to_wide__utf8( s_ansi_utf8, s_wide );
	test__to_ansi__utf8( s_ansi_utf8, s_wide );
}

/*static*/ void string::convert::test__to_wide__win1251(
	_in ansicstr_t s_ansi,
	_in cstr_t s_wide
) {
	const auto &s_converted = to_wide( s_ansi, codepage::win1251 );
	wprintf_s( __FUNCTIONW__ L": %s\n", std::wstring( s_wide ) == s_converted ? L"ok" : L"error" );
}
/*static*/ void string::convert::test__to_wide__utf8(
	_in ansicstr_t s_ansi,
	_in cstr_t s_wide
) {
	const auto &s_converted = to_wide( s_ansi, codepage::utf8 );
	wprintf_s( __FUNCTIONW__ L": %s\n", std::wstring( s_wide ) == s_converted ? L"ok" : L"error" );
}

/*static*/ void string::convert::test__to_ansi__win1251(
	_in ansicstr_t s_ansi,
	_in cstr_t s_wide
) {
	const auto &s_converted = to_ansi( s_wide, codepage::win1251 );
	wprintf_s( __FUNCTIONW__ L": %s\n", std::string( s_ansi ) == s_converted ? L"ok" : L"error" );
}
/*static*/ void string::convert::test__to_ansi__utf8(
	_in ansicstr_t s_ansi,
	_in cstr_t s_wide
) {
	const auto &s_converted = to_ansi( s_wide, codepage::utf8 );
	wprintf_s( __FUNCTIONW__ L": %s\n", std::string( s_ansi ) == s_converted ? L"ok" : L"error" );
}

//---------------------------------------------------------------------------------------------------------------------
guid::guid(
	_in bool is_new /*= true*/
) {
	if ( is_new )
		create_new( m_data );
	else
		ZeroMemory( &m_data, sizeof(m_data) );
}

guid::~guid(
) {
	if ( m_pstr )
		clear_string();
}

guid::operator const GUID&(
) const noexcept
{
	return m_data;
}

/*static*/ void guid::create_new(
	_out GUID &guid
) {
	const auto hr = ::CoCreateGuid( &guid );
	if ( S_OK == hr )
		return;
	
	TRACE_ERROR( L"CoCreateGuid(), 0x%08X", hr );
	throw hr;
}

const guid& guid::create_new(
) {
	create_new( m_data );
	if ( m_pstr )
		clear_string();
	return *this;
}

void guid::clear_string(
) {
	assert( m_pstr );
	m_pstr.reset();
}

cstr_t guid::to_string(
) {
	if ( m_pstr )
		return reinterpret_cast< cstr_t >( *m_pstr );

	m_pstr.reset( new RPC_WSTR, []( _out RPC_WSTR *p_str ) 
	{
		const auto rpc_status = RpcStringFree( p_str );
		if ( RPC_S_OK == rpc_status )
			return;
	
		TRACE_ERROR( L"RpcStringFree(), 0x%08X", rpc_status );
		//throw rpc_status;
	} );
	const auto rpc_status = ::UuidToStringW( &m_data, m_pstr.get() );
	if ( RPC_S_OK == rpc_status )
		return reinterpret_cast< cstr_t >( *m_pstr );
	
	TRACE_ERROR( L"UuidToStringW(), 0x%08X", rpc_status );
	throw rpc_status;
}

//---------------------------------------------------------------------------------------------------------------------