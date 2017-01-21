// email_sender.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include "smtp.h"
#include "settings.h"
#include "openssl.h"
#include <exception>
#include <locale>
#include "crypto.h"

#define LOCALE_NAME		"Russian_Russia.866"

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

bool check_result(
	_in int result_etalon,
	_in int result
) {
	if ( result_etalon == result )
		return true;

	TRACE_ERROR( L"result: %i", result );
	return false;
}

int wmain(
	int argc, 
	cstr_t* argv[]
) {
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );

	int exit_code = 0;

	base64::test test;
	
	test.encode__strings();		printf_s( "\n" );
	//test.decode__strings();		printf_s( "\n" );
	test.encode__data();		printf_s( "\n" );
	//test.decode__data();		printf_s( "\n" );

	try
	{
		if ( !setlocale( LC_ALL, LOCALE_NAME ) )
		{
			TRACE_ERROR( L"setlocale(): не удалось установить локаль \"%S\"", LOCALE_NAME );
			throw std::ios_base::failure ( "error setting locale: \"" LOCALE_NAME "\"" );
		}

		NETWORK_SUPPORT( 2, 2 );
		
		smtp::client smtp_client;

		// соединяемся с smtp-сервером для отправки почты
		exit_code = smtp_client.connect( SMTP_SERVER_NAME, SMTP_SERVER_PORT, smtp::client::connect_security::ssl );
		if ( ERROR_SUCCESS != exit_code )
			goto exit;
		
		// посылаем приветствие серверу
		//length = smtp_client.send( "EHLO %S\n", SMTP_SERVER_NAME );
		smtp_client.send_ehlo( /*EMAIL_ADDRESS_FROM*/ );

		// авторизация
		if ( !smtp_client.auth_plain( EMAIL_ADDRESS_FROM, EMAIL_ADDRESS_FROM_PASSWORD ) )
		{
			const std::string error( "authentication for \"" + std::string( EMAIL_ADDRESS_FROM ) + "\" failed (incorrect login data?)" );
			throw std::logic_error( error );
		}

		// формирование и отправка e-mail собщения
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

