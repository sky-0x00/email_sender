#pragma once

#include <string>
#include <fstream>
#include <array>
#include "types.h"

namespace address
{
	std::string get_last_email( _in cstr_t f_name );
	
	class email_base
	{
	public:
		email_base( _in cstr_t f_name, _in ansicstr_t last_email );
		ansicstr_t get_next();

	private:
		bool open_base( _in cstr_t f_name );
		bool goto_email( _in ansicstr_t last_email );

		std::ifstream m_fstream;
		std::array< ansichar_t, 256 > m_buffer;
	};
	
}