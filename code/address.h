#pragma once

#include <string>
#include <fstream>
#include <array>
#include "types.h"
#include "utils.h"

namespace address
{
	class email_log
	{
	public:
		static std::string get_last_email( _in cstr_t f_name );

		email_log( _in cstr_t f_name );
		void trace( _in ansicstr_t email, _in const guid &guid, _in ansicstr_t message_id );

	private:
		std::ofstream m_fstream;
	};
	
	
	class email_base
	{
	public:
		email_base( _in cstr_t f_name, _in ansicstr_t last_email );
		std::string get_next();

	private:
		bool open_base( _in cstr_t f_name );
		bool goto_email( _in ansicstr_t last_email );

		std::ifstream m_fstream;
	};
	
}