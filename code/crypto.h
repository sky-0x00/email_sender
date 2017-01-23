#pragma once

#include <string>
#include <vector>
#include "types.h"

#define BASE64__TRIPLET_SIZE			3		// байта
#define BASE64__WORD_SIZE				4		// символа

namespace crypto
{
	enum class method
	{
		base64, quoted_printable
	};

	enum class codepage
	{
		utf_8
	};

	std::string encode( _in ansicstr_t str_ansi, _in method method );

	//---------------------------------------------------------------------------------------------------------------------
	class base64
	{
	public:
		class test;

		typedef byte_t triplet_t[ BASE64__TRIPLET_SIZE ];			// триплет данных
		typedef ansichar_t word_t[ BASE64__WORD_SIZE ];			// слово в base64-алфавите (возможно, дополненное 1 или 2 символами '=')

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
		static void encode_triplet( _in const triplet_t &data, _out word_t &result );
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

	//---------------------------------------------------------------------------------------------------------------------
	class quoted_printable
	{
	public:
		class test;

		// преобразования для null-terminated строк
		static std::string encode( _in ansicstr_t str_ansi, _in codepage codepage = codepage::utf_8 );
		static std::string decode( _in ansicstr_t str_quoted );

		// преобразования для блоков данных произвольного размера
		static std::string encode( _in cdata_t data, _in size_t size );
		static void decode( _in ansicstr_t str_quoted, _out std::vector<byte_t> &data );

	private:
		class encoder;
		class decoder;

	};

	class quoted_printable::encoder
	{
	public:
		// возвращает число символов в результирующем quoted_printable-слове - (1, 3 или 6 символов)
		static unsigned encode_char( _in ansichar_t ch, _out ansistr_t result, _in codepage codepage = codepage::utf_8 );
	private:
		static void encode_char__utf_8( _in ansichar_t ch, _out ansistr_t result );
		static void encode_byte( _in byte_t byte, _out ansistr_t result );
		static bool is_direct( _in ansichar_t ch );
	};

	class quoted_printable::decoder
	{
	public:
		//private:
		//static unsigned get_index( _in char ch );
	};

}	// namespace crypto