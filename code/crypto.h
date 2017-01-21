#pragma once

#include <string>
#include <vector>
#include "types.h"

#define BASE64__TRIPLET_SIZE	3		// байта
#define BASE64__WORD_SIZE		4		// символа

class base64
{
public:
	class test;

	typedef byte_t triplet[ BASE64__TRIPLET_SIZE ];			// триплет данных
	typedef ansichar_t word[ BASE64__WORD_SIZE ];			// слово в base64-алфавите (возможно, дополненное 1 или 2 символами '=')

	// преобразования для null-terminated строк
	static std::string encode( _in ansicstr_t str_ansi );
	static std::string decode( _in ansicstr_t str_base64 );

	// преобразования для блоков данных произвольного размера
	static std::string encode( _in cdata_t data, _in size_t size );
	static void decode( _in ansicstr_t str_base64, _out std::vector<byte_t> &data );

private:
	class encoder;
	class decoder;	
};

class base64::encoder
{
public:
	static void encode_triplet( _in const triplet &data, _out word &result );
private:
	static char get_char( _in unsigned index );
};

class base64::decoder
{
public:
//private:
	static unsigned get_index( _in char ch );
};

class base64::test
{
public:
	void encode__strings() const;
	void decode__strings() const;

	void encode__data() const;
	void decode__data() const;

	test();

private:
	bool encode__string( _in ansicstr_t str_ansi, _in ansicstr_t str_base64 ) const;
	bool decode__string( _in ansicstr_t str_base64, _in ansicstr_t str_ansi ) const;

	bool encode__data_block( _in cdata_t data, _in size_t size, _in ansicstr_t str_base64 ) const;
	bool decode__data_block( _in ansicstr_t str_base64, _in const std::vector<byte_t> &data ) const;

	void init_data();
	static const std::vector<byte_t>& get_vector_from_string( _in ansicstr_t str );
	
	std::vector<byte_t> m__data;
	const ansicstr_t m__data_hash;
};