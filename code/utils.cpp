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
		function__to_wide function = to_wide;
		if ( static_cast< byte_t >( *p_ch ) >= 0x80 )
		{
			switch ( cp )
			{
			case codepage::win1251:
				function = to_wide__win1251;
				break;

			default:
				TRACE_ERROR( L"codepage: %i", static_cast< int >( cp ) );
				throw std::logic_error( "not yet implemented" );
			}
		}
		result.push_back( function( *p_ch ) );
	}

	return result;
}

/*static*/ char_t string::convert::to_wide( 
	_in ansichar_t ch 
) noexcept
{
	return ch;
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
	throw std::logic_error( "not supported character" );
}

//---------------------------------------------------------------------------------------------------------------------
/*static*/ std::string string::convert::to_ansi( 
	_in cstr_t str, 
	_in codepage cp /*= codepage::win1251*/
) {
	std::string result;

	for ( cstr_t p_ch = str; *p_ch; ++p_ch )
	{
		function__to_ansi function = to_ansi;
		if ( static_cast< ushort_t >( *p_ch ) >= 0x80 )
		{
			switch ( cp )
			{
			case codepage::win1251:
				function = to_ansi__win1251;
				break;

			default:
				TRACE_ERROR( L"codepage: %i", static_cast< int >( cp ) );
				throw std::logic_error( "not yet implemented" );
			}
		}
		result.push_back( function( *p_ch ) );
	}

	return result;
}

/*static*/ ansichar_t string::convert::to_ansi( 
	_in char_t ch 
) noexcept
{
	return static_cast< ansichar_t >( ch );
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

	// ¨, ¸ - 0x401, 0x451 => 0xA8 (-0x359), 0xB8 (-0x399)
	if ( value == 0x401 )
		return 0xA8;
	if ( value == 0x451 )
		return 0xB8;

	TRACE_ERROR( L"ch (wide): 0x%04X", value );
	throw std::logic_error( "not supported character" );
}
#pragma warning( pop )

//---------------------------------------------------------------------------------------------------------------------
/*static*/ void string::convert::test(
) {
	const ansichar_t s_ansi[] = "ABCDEFGHKLMNOPQRSTUVWXYZabcdefghklmnopqrstuvwxyzÀÁÂÃÄÅ¨ÆÇÊËÌÍÎÏĞÑÒÓÔÕÖ×ÃÙÚÛÜİŞßàáâãäå¸æçêëìíîïğñòóôõö÷ãùúûüışÿ";
	const char_t s_wide[] = L"ABCDEFGHKLMNOPQRSTUVWXYZabcdefghklmnopqrstuvwxyzÀÁÂÃÄÅ¨ÆÇÊËÌÍÎÏĞÑÒÓÔÕÖ×ÃÙÚÛÜİŞßàáâãäå¸æçêëìíîïğñòóôõö÷ãùúûüışÿ";

	test__to_wide( s_ansi, s_wide );
	test__to_ansi( s_ansi, s_wide );
}

/*static*/ void string::convert::test__to_wide(
	_in ansicstr_t s_ansi,
	_in cstr_t s_wide
) {
	wprintf_s( __FUNCTIONW__ L": %s\n", std::wstring( s_wide ) == to_wide( s_ansi ) ? L"ok" : L"error" );
}
/*static*/ void string::convert::test__to_ansi(
	_in ansicstr_t s_ansi,
	_in cstr_t s_wide
) {
	wprintf_s( __FUNCTIONW__ L": %s\n", std::string( s_ansi ) == to_ansi( s_wide ) ? L"ok" : L"error" );
}

//---------------------------------------------------------------------------------------------------------------------