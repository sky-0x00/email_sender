#include "stdafx.h"

#include "crypto.h"
#include <iterator>
#include <windows.h>

//---------------------------------------------------------------------------------------------------------------------
#define BASE64__SCHEMA_SIZE		64

namespace crypto
{
	ansichar_t get_hexdigit( _in byte_t nibble, _in bool is_upper = true )
	{
		// nibble - полубайт 0..F
		
		if ( nibble < 10 )
			return nibble + '0';
		if ( nibble < 16 )
		{
			nibble -= 10;
			return is_upper ? nibble + 'A' : nibble + 'a';
		}

		TRACE_ERROR( L"nibble: 0x%02X, expected [0..0xF)", nibble );
		throw std::logic_error( "nibble too large" );
	}
}

//---------------------------------------------------------------------------------------------------------------------
std::string crypto::encode(
	_in ansicstr_t str_ansi,
	_in method method
) {
	switch ( method )
	{
		case method::base64:
			return base64::encode( str_ansi );
		
		case method::quoted_printable:
			return quoted_printable::encode( str_ansi );
	}
	
	throw std::logic_error( NOT_IMPLEMENTED );
}

//---------------------------------------------------------------------------------------------------------------------
// схема кодирования - на входе число [0..64), на выходе - символ из массива schema
/*static*/ char crypto::base64::encoder::get_char(
	_in unsigned index
) {
	static const char schema[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static_assert( _countof(schema) == BASE64__SCHEMA_SIZE + 1, "schema size incorrect" );

	if ( index < BASE64__SCHEMA_SIZE )
		return schema[ index ];

	TRACE_ERROR( L"schema index too large: %u, range: [0..%u)", index, BASE64__SCHEMA_SIZE );
	throw std::logic_error( std::to_string( index ) );
}

/*static*/ void crypto::base64::encoder::encode_triplet(
	_in const triplet_t &data,
	_out word_t &result
) {
	// 1-ые 6 бит - это старшие биты из 1-го байта
	unsigned index = (data[0] >> 2) & 0x3F;
	result[0] = get_char( index );

	// 2-ые 6 бит - 2 младших бита из 1-го + 4 старших бита из 2-го байта
	index = ( (data[1] >> 4) & 0x0F) | ( (data[0] << 4) & 0x30);
	result[1] = get_char( index );

	// 3-ие 6 бит - 4 младших бита из 2-го + 2 старших бита из 3-го байтов
	index = ( (data[2] >> 6) & 0x03) | ( (data[1] << 2) & 0x3C);
	result[2] = get_char( index );

	// 4-ые 6 бит - 6 младших бита из 4-го байта
	index = data[2] & 0x3F;
	result[3] = get_char( index );
}


// схема декодирования - на входе символ из массива schema (см. ф-ию encode), на выходе - число [0..64)
/*static*/ unsigned crypto::base64::decoder::get_index(
	_in char ch
) {
	//static const char schema[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	if ( in_range<char>( ch, { 'A', 'Z' } ) )
		return ch - 'A';							// ['A'..'Z'] -> [0..25]

	if ( in_range<char>( ch, { 'a', 'z' } ) )
		return ch - 'a' + 26;						// ['a'..'z'] -> [26..51]

	if ( in_range<char>( ch, { '0', '9' } ) )
		return ch - '0' + 52;						// ['0'..'9'] -> [52..61]

	if ( ch == '+' )
		return 62;

	if ( ch == '/' )
		return 63;

	TRACE_ERROR( L"not base64-alphabet char: \'%c\' (0x%02X)", ch, ch );
	throw std::logic_error( std::to_string( ch ) );
}

//---------------------------------------------------------------------------------------------------------------------
/*static*/ std::string crypto::base64::encode(
	_in ansicstr_t str
) {
	std::string result;

	triplet_t data_block;
	word_t result_block;
	unsigned index_mod = 0;

	for ( decltype(str) p_data = str; *p_data; ++p_data, index_mod = (index_mod + 1) % BASE64__TRIPLET_SIZE )
	{
		data_block[ index_mod ] = *p_data;

		if ( index_mod + 1 != BASE64__TRIPLET_SIZE )
			continue;
		
		encoder::encode_triplet( data_block, result_block );
		result.append( result_block, BASE64__WORD_SIZE );

	}	// for

	if ( index_mod )
	{
		// дополняем нулями
		for ( decltype(index_mod) i = index_mod; i < BASE64__TRIPLET_SIZE; ++i )
			data_block[i] = 0;

		encoder::encode_triplet( data_block, result_block );
		for ( decltype(index_mod) i = index_mod; i < BASE64__TRIPLET_SIZE; )
			result_block[++i] = '=';

		result.append( result_block, BASE64__WORD_SIZE );
	}	

	return result;
}
/*static*/ std::string crypto::base64::decode(
	_in ansicstr_t str_base64
) {
	
	std::string result;
	int step = -1;
	byte ch = 0;				// in range [0..256)

	for ( decltype(str_base64) p_ch = str_base64; p_ch[0]; ++p_ch )
	{
		if ( p_ch[0] == '=' )
		{
			if ( p_ch[1] == '\0' )
			{
				//result.push_back( ch );
				break;
			}
			
			if ( p_ch[1] == '=' )
				if ( p_ch[2] == '\0' )
				{
					//result.push_back( ch );
					break;
				}

			TRACE_ERROR( L"incorrect base64-line suffix" );
			throw std::logic_error( "incorrect base64-line suffix" );
		}

		//if ( schema[ p_ch[0] ] >= BASE64__SCHEMA_SIZE )
		//{
		//	TRACE_ERROR( L"incorrect base64-character" );
		//	throw std::system_error( std::error_code(), "incorrect base64-character" );
		//}

		switch ( ++step )
		{
		case 0:
			// получили 6 старших битов текущего байта
			ch = ( decoder::get_index( p_ch[0] ) << 2) & 0xFC;
			break;

		case 1:
			// получили 2 младших бита предыдущего байта и 4 старших текущего
			ch |= ( decoder::get_index( p_ch[0] ) >> 4) & 0x03;
			result.push_back( ch );
			ch = ( decoder::get_index( p_ch[0] ) << 4) & 0xF0;
			break;

		case 2:
			// получили 4 младших бита предыдущего байта и 2 старших текущего
			ch |= ( decoder::get_index( p_ch[0] ) >> 2) & 0x0F;
			result.push_back( ch );
			ch = ( decoder::get_index( p_ch[0] ) << 6) & 0xC0;
			break;

		case 3:
			// получили 6 младших битов текущего байта
			ch |= decoder::get_index( p_ch[0] ) & 0x3F;
			result.push_back( ch );

			// это последний шаг, далее начинаем с шага 0
			step = -1;
			break;
		}
	}

	return result;
}


/*static*/ std::string crypto::base64::encode(
	_in cdata_t data,
	_in size_t size
) {
	std::string result;

	triplet_t data_block;
	word_t result_block;

	std::pair< unsigned, unsigned > count( std::make_pair( size / BASE64__TRIPLET_SIZE, size % BASE64__TRIPLET_SIZE ) );
	while ( count.first-- )
	{
		for ( unsigned i = 0; i < BASE64__TRIPLET_SIZE; ++i )
			data_block[i] = data[i];
		
		encoder::encode_triplet( data_block, result_block );
		result.append( result_block, BASE64__WORD_SIZE );

		data += BASE64__TRIPLET_SIZE;
	}	// for

	if ( count.second )
	{
		// заполняем остаточные данные
		decltype(count.second) i = 0;
		while ( i < count.second)
			data_block[i++] = data[i];

		// и дополняем нулями
		while ( i < BASE64__TRIPLET_SIZE )
			data_block[i++] = 0;

		encoder::encode_triplet( data_block, result_block );
		for ( i = count.second; i < BASE64__TRIPLET_SIZE; )
			result_block[++i] = '=';

		result.append( result_block, BASE64__WORD_SIZE );
	}	

	return result;
}
/*static*/ void crypto::base64::decode(
	_in ansicstr_t str_base64,
	_out std::vector<byte_t> &result
) {
	UNREFERENCED_PARAMETER( str_base64 );
	UNREFERENCED_PARAMETER( result );
}

//---------------------------------------------------------------------------------------------------------------------
crypto::base64::test::test(
)
	: m__data_hash( "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Njc\
4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+\
AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsf\
IycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/w==" )
{
	init_data();
}

void crypto::base64::test::init_data(
) {
	auto size = 0x100;
	m__data.resize( size );
	
	while ( size-- )
		m__data[ size ] = static_cast<byte_t>( size );
}

/*static*/ bool crypto::base64::test::encode__string(
	_in ansicstr_t str_ansi,
	_in ansicstr_t str_base64
) const
{
	const auto &str_enc = base64::encode( str_ansi );
	printf_s( "encode( \"%s\" ): \"%s\" => ", str_ansi, str_enc.c_str() );
	const auto result = 0 == strcmp( str_enc.c_str(), str_base64 );
	if ( result )
		printf_s( "ok\n" );
	else
		printf_s( "error, expected: \"%s\"\n", str_base64 );
	return result;
}

/*static*/ void crypto::base64::test::encode__strings(
) const 
{
	encode__string( "", "" );

	encode__string( "A", "QQ==" );
	encode__string( "AB", "QUI=" );
	encode__string( "ABC", "QUJD" );
	encode__string( "ABCD", "QUJDRA==" );

	encode__string( "ABCDE", "QUJDREU=" );
	encode__string( "ABCDEF", "QUJDREVG" );
	encode__string( "ABCDEFG", "QUJDREVGRw==" );
	encode__string( "ABCDEFGH", "QUJDREVGR0g=" );
}

//---------------------------------------------------------------------------------------------------------------------
/*static*/ bool crypto::base64::test::decode__string(
	_in ansicstr_t str_base64,
	_in ansicstr_t str_ansi
) const 
{
	const auto &str_dec = base64::decode( str_ansi );
	printf_s( "decode( \"%s\" ): \"%s\" => ", str_base64, str_dec.c_str() );
	const auto result = 0 == strcmp( str_dec.c_str(), str_ansi );
	if ( result )
		printf_s( "ok\n" );
	else
		printf_s( "error, expected: \"%s\"\n", str_ansi );
	return result;
}

/*static*/ void crypto::base64::test::decode__strings(
) const 
{
	decode__string( "", "" );

	decode__string( "QQ==", "A" );
	decode__string( "QUI=", "AB" );
	decode__string( "QUJD", "ABC" );
	decode__string( "QUJDRA==", "ABCD" );

	decode__string( "QUJDREU=", "ABCDE" );
	decode__string( "QUJDREVG", "ABCDEF" );
	decode__string( "QUJDREVGRw==", "ABCDEFG" );
	decode__string( "QUJDREVGR0g=", "ABCDEFGH" );
}

//---------------------------------------------------------------------------------------------------------------------
/*static*/ bool crypto::base64::test::encode__data_block(
	_in cdata_t data,
	_in size_t size,
	_in ansicstr_t str_base64
) const
{
	const auto &str_enc = base64::encode( data, size );
	printf_s( "encode(): \"%s\" => ", str_enc.c_str() );
	const auto result = 0 == strcmp( str_enc.c_str(), str_base64 );
	if ( result )
		printf_s( "ok\n" );
	else
		printf_s( "error, expected: \"%s\"\n", str_base64 );
	return result;
}
	
/*static*/ void crypto::base64::test::encode__data(
) const 
{
	encode__data_block( reinterpret_cast<cdata_t>( "" ), 0, "" );

	encode__data_block( reinterpret_cast<cdata_t>( "A" ), 1, "QQ==" );
	encode__data_block( reinterpret_cast<cdata_t>( "AB" ), 2, "QUI=" );
	encode__data_block( reinterpret_cast<cdata_t>( "ABC" ), 3, "QUJD" );
	encode__data_block( reinterpret_cast<cdata_t>( "ABCD" ), 4, "QUJDRA==" );

	encode__data_block( reinterpret_cast<cdata_t>( "ABCDE" ), 5, "QUJDREU=" );
	encode__data_block( reinterpret_cast<cdata_t>( "ABCDEF" ), 6, "QUJDREVG" );
	encode__data_block( reinterpret_cast<cdata_t>( "ABCDEFG" ), 7, "QUJDREVGRw==" );
	encode__data_block( reinterpret_cast<cdata_t>( "ABCDEFGH" ), 8, "QUJDREVGR0g=" );

	encode__data_block( m__data.data(), m__data.size(), m__data_hash );
}
//---------------------------------------------------------------------------------------------------------------------
/*static*/ bool crypto::base64::test::decode__data_block(
	_in ansicstr_t str_base64,
	_in const std::vector< byte_t > &data
) const 
{
	std::vector< byte_t > data_dec;
	base64::decode( str_base64, data_dec );
	printf_s( "decode( \"%s\" ) => ", str_base64 );
	if ( data_dec == data )
	{
		printf_s( "ok\n" );
		return true;
	}

	printf_s( " error, arrays not equal\ndata (%u bytes): ", data.size() );
	for ( auto x : data )
		printf_s( "%02X", x );
	printf_s( "\ndata_dec (%u bytes): ", data_dec.size() );
	for ( auto x : data )
		printf_s( "%02X", x );
	printf_s( "\n" );
	
	return false;
}

/*static*/ const std::vector<byte_t>& crypto::base64::test::get_vector_from_string(
	_in ansicstr_t str
) {
	static std::vector<byte_t> data( 16 );
	data.clear();

	for ( decltype(str) p = str; *p; ++p )
		data.push_back( *p );

	return data;
}

/*static*/ void crypto::base64::test::decode__data(
) const
{
	decode__data_block( "", get_vector_from_string( "" ) );

	decode__data_block( "QQ==", get_vector_from_string( "A") );
	decode__data_block( "QUI=", get_vector_from_string( "AB" ) );
	decode__data_block( "QUJD", get_vector_from_string( "ABC" ) );
	decode__data_block( "QUJDRA==", get_vector_from_string( "ABCD" ) );

	decode__data_block( "QUJDREU=", get_vector_from_string( "ABCDE" ) );
	decode__data_block( "QUJDREVG", get_vector_from_string( "ABCDEF" ) );
	decode__data_block( "QUJDREVGRw==", get_vector_from_string( "ABCDEFG" ) );
	decode__data_block( "QUJDREVGR0g=", get_vector_from_string( "ABCDEFGH" ) );

	decode__data_block( m__data_hash, m__data );
}

//---------------------------------------------------------------------------------------------------------------------
//
// символы, выводимые as-is
//
/*static*/ bool crypto::quoted_printable::encoder::is_direct(
	_in ansichar_t ch
) noexcept
{
	return ('\t' == ch) || in_range< ansichar_t >( ch, { 32, 60 } ) || in_range< ansichar_t >( ch, { 62, 126 } );
}

/*static*/ void crypto::quoted_printable::encoder::encode_byte(
	_in byte_t value,
	_out ansistr_t result
) {
	result[0] = '=';
	result[1] = get_hexdigit( (value >> 4) & 0x0F );
	result[2] = get_hexdigit( value & 0x0F );
}

/*static*/ unsigned crypto::quoted_printable::encoder::encode_char(
	_in ansichar_t ch,
	_out ansistr_t result
) {
	if ( is_direct( ch ) )
	{
		result[0] = ch;
		return 1;
	}

	encode_byte( ch, result );
	return 3;
}

//---------------------------------------------------------------------------------------------------------------------
/*static*/ std::string crypto::quoted_printable::encode(
	_in ansicstr_t str
) {
	std::string result;
	ansichar_t buffer[3];

	for ( ansicstr_t p_ch = str; *p_ch; ++p_ch )
	{
		const auto size = encoder::encode_char( *p_ch, buffer );
		assert( (1 == size) || (3 == size) );
		result.append( buffer, size );
	}

	return result;
}

/*static*/ std::string crypto::quoted_printable::decode(
	_in ansicstr_t str_quoted
) {
	UNREFERENCED_PARAMETER( str_quoted );

	throw std::logic_error( NOT_IMPLEMENTED );
}

/*static*/ std::string crypto::quoted_printable::encode(
	_in cdata_t data,
	_in size_t size
) {
	UNREFERENCED_PARAMETER( data );
	UNREFERENCED_PARAMETER( size );

	throw std::logic_error( NOT_IMPLEMENTED );
}
	
/*static*/ void crypto::quoted_printable::decode(
	_in ansicstr_t str_quoted,
	_out std::vector<byte_t> &data
) {
	UNREFERENCED_PARAMETER( str_quoted );
	UNREFERENCED_PARAMETER( data );

	throw std::logic_error( NOT_IMPLEMENTED );
}

//---------------------------------------------------------------------------------------------------------------------