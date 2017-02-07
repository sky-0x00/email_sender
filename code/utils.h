#pragma once

#include "types.h"
#include <string>

namespace string
{
	enum class codepage
	{
		win1251
	};

	class convert
	{
	public:
		typedef char_t (*function__to_wide)( _in ansichar_t );
		typedef ansichar_t (*function__to_ansi)( _in char_t );

		static std::wstring to_wide( _in ansicstr_t str, _in codepage cp = codepage::win1251 );
		static std::string to_ansi( _in cstr_t str, _in codepage cp = codepage::win1251 );

		static void test();

	private:
		static char_t to_wide( _in ansichar_t ch ) noexcept;
		static char_t to_wide__win1251( _in ansichar_t ch );

		static ansichar_t to_ansi( _in char_t ch ) noexcept;
		static ansichar_t to_ansi__win1251( _in char_t ch );

		static void test__to_wide( _in ansicstr_t s_ansi, _in cstr_t s_wide );
		static void test__to_ansi( _in ansicstr_t s_ansi, _in cstr_t s_wide );
	};
}
