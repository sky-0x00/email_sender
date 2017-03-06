// email_sender.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"

#include "smtp.h"
#include "settings.h"
#include "openssl.h"
#include <exception>
#include <mbctype.h>
#include "crypto.h"
#include "utils.h"

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
		TRACE_ERROR( L"setlocale(): �� ������� ���������� ������ \"%S\"", LOCALE_NAME );
		const std::string error( "error setting locale: \"" + std::string( LOCALE_NAME ) + "\"" );
		throw std::ios_base::failure( error );
	}
	if ( _setmbcp( _MB_CP_LOCALE ) )
	{
		TRACE_ERROR( L"_setmbcp(): �� ������� ���������� mb-������" );
		throw std::ios_base::failure( "error setting mb-locale" );
	}

	//if ( !_wsetlocale( LC_ALL, TEXT(LOCALE_NAME) ) )
	//{
	//	TRACE_ERROR( L"_wsetlocale(): �� ������� ���������� ������ \"%S\"", LOCALE_NAME );
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

		//const auto x = crypto::quoted_printable::encode( "���� �� naekvatore.ru - 23/01/2017" );

		//const auto external_ip = smtp::get_external_ip();
		smtp::client smtp_client;

		// ����������� � smtp-�������� ��� �������� �����
		exit_code = smtp_client.connect( SMTP_SERVER_NAME, SMTP_SERVER_PORT, smtp::client::connect_security::ssl );
		if ( ERROR_SUCCESS != exit_code )
			goto exit;
		
		// �������� ����������� �������
		//length = smtp_client.send( "EHLO %S\n", SMTP_SERVER_NAME );
		smtp_client.send_ehlo( /*EMAIL_ADDRESS_FROM*/ );

		// �����������
		if ( !smtp_client.auth_plain( EMAIL_ADDRESS_FROM, EMAIL_ADDRESS_FROM_PASSWORD ) )
		{
			const std::string error( "authentication for \"" + std::string( EMAIL_ADDRESS_FROM ) + "\" failed (incorrect login data?)" );
			throw std::logic_error( error );
		}

		// ������������ � �������� e-mail ��������
		const ansicstr_t addresslist_to[] = EMAIL_ADDRESS_LIST_TO;
		const size_t size = _countof( addresslist_to );
		std::string id;
		for ( size_t i = 0; i < size; ++i )
			smtp_client.mail( EMAIL_ADDRESS_FROM, addresslist_to[i], EMAIL_TOPIC, EMAIL_MESSAGE, &id );

		smtp_client.quit();
	}
	catch ( const std::exception &e )
	{
		if ( !SetConsoleErrorToRedColor() )
			return GetLastError();

		wprintf_s( L"������: \"%S\"", e.what() );
	}
	catch (...)
	{
		if ( !SetConsoleErrorToRedColor() )
			return GetLastError();

		wprintf_s( L"����������� ������" );
	}

	exit:
	return exit_code;
}