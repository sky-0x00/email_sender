// email_sender.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include "smtp.h"
#include "settings.h"
#include <exception>
#include <locale>

#define LOCALE_NAME		"Russian_Russia.866"
#define BUFFER_SIZE		512

bool SetConsoleErrorToRedColor()
{
	const auto hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );		
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if ( FALSE == GetConsoleScreenBufferInfo( hConsoleOutput, &csbi ) )
	{
		TRACE_ERROR( L"GetConsoleScreenBufferInfo(), error: 0x%08X", GetLastError() );
		return false;
	}
		
	csbi.wAttributes &= 0xFFF0;
	csbi.wAttributes |= FOREGROUND_RED | FOREGROUND_INTENSITY;

	if ( FALSE == SetConsoleTextAttribute( hConsoleOutput, csbi.wAttributes ) )
	{
		TRACE_ERROR( L"SetConsoleTextAttribute(), error: 0x%08X", GetLastError() );
		return false;
	}

	return true;
}

//template <typename container>
//void echo_items(_in const container &container, char_t )
//{
//}

int wmain(
	int argc, 
	cstr_t* argv[]
) {
	int exit_code = 0;

	try
	{
		if ( !_wsetlocale( LC_ALL, TEXT(LOCALE_NAME) ) )
		{
			TRACE_ERROR( L"_wsetlocale(): не удалось установить локаль \"%S\"", LOCALE_NAME );
			throw std::ios_base::failure ( "error setting locale: \"" LOCALE_NAME "\"" );
		}

		NETWORK_SUPPORT( 2, 2 );

		smtp::client smtp_client;
		exit_code = smtp_client.connect( SMTP_SERVER_NAME, SMTP_SERVER_PORT );
		if ( ERROR_SUCCESS != exit_code )
			goto exit;

		byte_t buffer[ BUFFER_SIZE ];
		int length;

		//length = recv( smtp_client.socket, buffer, _countof( buffer ), 0 );

		//length = sprintf_s( buffer, _countof(buffer), "EHLO %S\n", SMTP_SERVER_NAME ); 
		//length = send( smtp_client.socket, buffer, length, 0 );
		//length = recv( smtp_client.socket, buffer, _countof( buffer ), 0 );
		//if ( length < 0 )
		//	TRACE_ERROR( L"recv(), error: %li", smtp::wsalib::get_error() );

		//length = smtp_client.receive();
		//length = smtp_client.send( "EHLO %S\n", SMTP_SERVER_NAME );
		length = smtp_client.send( "EHLO my_name\r\n" );
		//length = smtp_client.send( "AUTH PLAIN\r\n" );
		length = smtp_client.receive();

	}
	catch ( const std::exception &e )
	{
		if ( !SetConsoleErrorToRedColor() )
			return GetLastError();

		wprintf_s( L"ошибка: \"%S\"", e.what() );
	}
	catch (...)
	{
		if ( !SetConsoleErrorToRedColor() )
			return GetLastError();

		wprintf_s( L"неизвестная ошибка" );
	}

	exit:
	return exit_code;
}

