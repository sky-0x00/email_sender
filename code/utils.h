#pragma once

#include "types.h"
#include <string>
#include <rpc.h>

namespace string
{
	enum class codepage
	{
		win1251,
		utf8
	};

	//
	// конвертер строк с поддержкой русского языка
	//
	class convert
	{
	public:
		//typedef char_t (*function__to_wide)( _in ansichar_t );
		//typedef ansichar_t (*function__to_ansi)( _in char_t );

		static std::wstring to_wide( _in ansicstr_t str, _in codepage cp = codepage::win1251 );
		static std::string to_ansi( _in cstr_t str, _in codepage cp = codepage::win1251 );

		static void test();

	private:
		static char_t to_wide__win1251( _in ansichar_t ch );
		static char_t to_wide__utf8( _in char_t ch );				// на входе wchar- символ (составлен из первых 2 ansi- символов)
		static char_t to_wide__utf8( _in ansicstr_t str );			// на входе строка (первые 2 ansi- символа)

		static ansichar_t to_ansi__win1251( _in char_t ch );
		static char_t to_ansi__utf8( _in char_t ch );				// на выходе 2 ansi- символа

		static void test__to_wide__win1251( _in ansicstr_t s_ansi, _in cstr_t s_wide );
		static void test__to_wide__utf8( _in ansicstr_t s_ansi, _in cstr_t s_wide );
		static void test__to_ansi__win1251( _in ansicstr_t s_ansi, _in cstr_t s_wide );
		static void test__to_ansi__utf8( _in ansicstr_t s_ansi, _in cstr_t s_wide );
	};
}

class guid
{
public:
	guid( _in bool is_new = true );
	operator const GUID&() const noexcept;
	void create_new();
	std::wstring to_string() const;
private:
	GUID m_data;
};
