// email_sender.cpp: определ€ет точку входа дл€ консольного приложени€.
//

#include "stdafx.h"

#include "smtp.h"
#include "settings.h"
#include "openssl.h"
#include <exception>
#include <mbctype.h>
#include "crypto.h"
#include "utils.h"
#include "address.h"
#include <cstdlib>
#include <ctime>
#include <thread>

bool SetConsoleErrorToRedColor()
{
	const auto hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );		
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if ( FALSE == GetConsoleScreenBufferInfo( hConsoleOutput, &csbi ) )
	{
		TRACE_ERROR( L"GetConsoleScreenBufferInfo(), 0x%08X", GetLastError() );
		return false;
	}
		
	csbi.wAttributes &= 0xFFF0;
	csbi.wAttributes |= FOREGROUND_RED | FOREGROUND_INTENSITY;

	if ( FALSE == SetConsoleTextAttribute( hConsoleOutput, csbi.wAttributes ) )
	{
		TRACE_ERROR( L"SetConsoleTextAttribute(), 0x%08X", GetLastError() );
		return false;
	}

	return true;
}

void console__set_locale()
{
	if ( !setlocale( LC_ALL, LOCALE_NAME ) )
	{
		TRACE_ERROR( L"setlocale(): не удалось установить локаль \"%S\"", LOCALE_NAME );
		const std::string error( "error setting locale: \"" + std::string( LOCALE_NAME ) + "\"" );
		throw std::ios_base::failure( error );
	}
	if ( _setmbcp( _MB_CP_LOCALE ) )
	{
		TRACE_ERROR( L"_setmbcp(): не удалось установить mb-локаль" );
		throw std::ios_base::failure( "error setting mb-locale" );
	}

	//if ( !_wsetlocale( LC_ALL, TEXT(LOCALE_NAME) ) )
	//{
	//	TRACE_ERROR( L"_wsetlocale(): не удалось установить локаль \"%S\"", LOCALE_NAME );
	//	const std::string error( "error setting locale: \"" + std::string( LOCALE_NAME ) + "\"" );
	//	throw std::ios_base::failure( error );
	//}
}

void trace__set_filename()
{
	char_t buffer[0x100];
	DWORD size = _countof( buffer );
	if ( !QueryFullProcessImageName( GetCurrentProcess(), 0, buffer, &size ) )
	{
		const auto error = GetLastError();
		TRACE_ERROR( L"QueryFullProcessImageName(), 0x%08X", error );
		throw std::system_error ( error, std::system_category() );
	}

	//wcscpy_s( buffer + size, _countof( buffer ) - size, L"0" );

	for ( char_t *p = buffer + size; --p > buffer; )
		if ( *p == L'.' )
		{
			wcscpy_s( p + 1, _countof( buffer ) - (p + 1 - buffer), L"trace" );
			break;
		}

	SETFILE_TRACE( buffer );
	TRACE_NORMAL( L"Trace filename: \"%s\"", buffer );
}

int wmain( _in size_t argc, _in cstr_t* argv[]
) {
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );

	int exit_code = 0;

	//const auto &email_last = address::get_last_email( LAST_EMAIL__FILE_NAME );
	//address::email_base email_base( EMAIL_BASE__FILE_NAME, email_last.c_str() );
	//auto email = email_base.get_next();
	//email = nullptr;
	//UNREFERENCED_PARAMETER( email );

	//crypto::base64::test test;	
	//test.encode__strings();		printf_s( "\n" );
	//test.decode__strings();		printf_s( "\n" );
	//test.encode__data();		printf_s( "\n" );
	//test.decode__data();		printf_s( "\n" );

	//string::convert::test();

	//{
	//	guid guid, guid_true( true );
	//	const auto str = guid.to_string();
	//	const auto str_true = guid_true.to_string();

	//	const auto x1 = guid::traits::size_data__bytes();
	//	const auto x2 = guid::traits::size_string__chars();

	//	UNREFERENCED_PARAMETER( str );
	//	UNREFERENCED_PARAMETER( str_true );
	//}

	try
	{
		console__set_locale();
		trace__set_filename();

		NETWORK_SUPPORT( 2, 2 );

		//const auto x = crypto::quoted_printable::encode( "“уры от naekvatore.ru - 23/01/2017" );

		//const auto external_ip = smtp::get_external_ip();
		smtp::client smtp_client;

		// соедин€емс€ с smtp-сервером дл€ отправки почты
		exit_code = smtp_client.connect( SMTP_SERVER_NAME, SMTP_SERVER_PORT, smtp::client::connect_security::ssl );
		if ( ERROR_SUCCESS != exit_code )
			goto exit;
		
		// посылаем приветствие серверу
		//length = smtp_client.send( "EHLO %S\n", SMTP_SERVER_NAME );
		smtp_client.send_ehlo( /*EMAIL_ADDRESS_FROM*/ );

		// авторизаци€
		if ( !smtp_client.auth_plain( EMAIL_ADDRESS_FROM, EMAIL_ADDRESS_FROM_PASSWORD ) )
		{
			const std::string error( "authentication for \"" + std::string( EMAIL_ADDRESS_FROM ) + "\" failed (incorrect login data?)" );
			throw std::logic_error( error );
		}

		// формирование и отправка e-mail собщени€
		const ansicstr_t addresslist_to[] = EMAIL_ADDRESS_LIST_TO;
		const size_t size = _countof( addresslist_to );
		
		std::string id;
		guid guid;

#ifdef DO_PAUSE_IN_PROCESS
#if PAUSE_SECS_MIN != PAUSE_SECS_MAX
		std::srand( static_cast<unsigned int>( std::time( nullptr )) );		// инициализируем rand()- генератор
#endif
#endif
		for ( size_t i = 0; i < size; ++i )
		{
			smtp_client.mail( EMAIL_ADDRESS_FROM, addresslist_to[i], EMAIL_TOPIC, EMAIL_MESSAGE, &guid, &id );

#ifdef DO_PAUSE_IN_PROCESS
#if PAUSE_SECS_MIN == PAUSE_SECS_MAX
			double sleep_for__secs = PAUSE_SECS_MIN;
#else
			double sleep_for__secs = stdex::rand( {PAUSE_SECS_MIN, PAUSE_SECS_MAX} );
#endif
			TRACE_NORMAL( L"sleep for %.3f sec...", sleep_for__secs );
			std::this_thread::sleep_for( std::chrono::milliseconds( static_cast<unsigned int>(1000.0 * sleep_for__secs) ) );
#endif
		}

		smtp_client.quit();
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

		wprintf_s( L"неизвестна€ ошибка" );
	}

	exit:
	return exit_code;
}