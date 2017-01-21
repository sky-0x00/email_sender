#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "smtp.h"
#include "settings.h"

#include <system_error>
#include <ws2tcpip.h>
#include <string>
//#include <regex>
#include <vector>
#include <iterator>
#include "crypto.h"

#pragma comment(lib, "ws2_32.lib")

static_assert( sizeof( sockaddr ) == sizeof( sockaddr_in ), "не совпадают размеры" );

bool operator <( _in const in_addr &lhs, _in const in_addr &rhs ) 
{
	return lhs.s_addr < rhs.s_addr;
}

smtp::wsalib::wsalib(
	_in unsigned ver_hi, 
	_in unsigned ver_lo
) {
	TRACE_NORMAL( L"WSA version, request: %hu.%hu", ver_hi, ver_lo );

	WSADATA wsadata_response;
	const auto result = WSAStartup( MAKEWORD( ver_hi, ver_lo ), &wsadata_response );
	if ( !result )
	{
		TRACE_NORMAL( L"WSA version, response: working %hu.%hu, maximum_supported %hu.%hu", 
			LOBYTE( wsadata_response.wVersion ), HIBYTE( wsadata_response.wVersion ), LOBYTE( wsadata_response.wHighVersion ), HIBYTE( wsadata_response.wHighVersion ) );
		return;
	}

	TRACE_ERROR( L"WSAStartup(), error: %li", result );
	throw std::system_error( result, std::system_category() );
}

smtp::wsalib::~wsalib()
{
	const auto result = WSACleanup();
	if ( !result )
		return;

	if ( SOCKET_ERROR == result )
	{
		TRACE_ERROR( L"WSACleanup(), error: %li", get_error() );
		return;
	}

	TRACE_ERROR( L"WSACleanup(), error: %li", result );
	//throw std::system_error( result, std::system_category() );
}

/*static*/ int smtp::wsalib::get_error(
) {
	return WSAGetLastError();
}

smtp::client::client(
	_in size_t buffer_size /*= default::buffer_size */
) :
	m__is_connected( false ),
	m__buffer_size( buffer_size ),
	m__buffer( new ansichar_t[ buffer_size ] ),
	m__socket( ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) )
{
	if ( INVALID_SOCKET != m__socket )
		return;
	
	const auto error = wsalib::get_error();
	TRACE_ERROR( L"socket(), error: %li", error );
	throw std::system_error( error, std::system_category() );
}

smtp::client::~client(
) {
	delete[] m__buffer;

	if ( m__is_connected )
		disconnect();

	if ( SOCKET_ERROR != ::closesocket( m__socket ) )
		return;

	TRACE_ERROR( L"closesocket(), error: %li", wsalib::get_error() );
}

/*static*/ smtp::wsalib::status smtp::client::resolve_hostname( 
	_in ansicstr_t hostname,
	_out std::list<in_addr> &address_list
) {
	address_list.clear();

	ADDRINFOA addrinfo_hint = { 0 };
	addrinfo_hint.ai_family = AF_INET;
	addrinfo_hint.ai_socktype = SOCK_STREAM;
	addrinfo_hint.ai_protocol = IPPROTO_TCP;

	PADDRINFOA p_addrinfo_result = nullptr;

	auto get_portstr = []()
	{
		static ansichar_t port_str[ 6 ];
		_itoa_s( default::port, port_str, _countof(port_str), 10 );
		return port_str;
	};	
	static const auto portstr = get_portstr();
	const auto result = getaddrinfo( hostname, portstr, &addrinfo_hint, &p_addrinfo_result );
	if ( result )
	{
		TRACE_ERROR( L"getaddrinfo(\"%s\"), error: %li", hostname, result ); 
		return result;
	}

	for ( decltype(p_addrinfo_result) p = p_addrinfo_result; p; p = p->ai_next )
	{
		const auto &sin_addr = reinterpret_cast<const sockaddr_in*>( p->ai_addr )->sin_addr;
		//TRACE_NORMAL( L"p_in: %S", inet_ntoa( sin_addr ) );
		address_list.push_back( sin_addr );
	}
	freeaddrinfo( p_addrinfo_result );

	TRACE_NORMAL( L"\"%s\", ok (%lu address)", hostname, address_list.size() );
	return ERROR_SUCCESS;
}

smtp::wsalib::status smtp::client::connect(
	_in ansicstr_t server_name,
	_in port_t server_port,
	_in connect_security security /*= connect_security::no_cypher*/
) noexcept
{
	std::list<in_addr> s_hostaddr;

	// получаем все адреса узла с именем SMTP_SERVER_NAME - нашего почтового сервера для исходящих сообщений
	wprintf_s( L"получение адресов узла \"%S\"...", server_name );	
	const auto status = resolve_hostname( server_name, s_hostaddr );
	if ( status )
	{
		wprintf_s( L" не удалось разрешить, ошибка %li\n", status );
		return status;
	}

	auto cit = s_hostaddr.cbegin();
	std::string hostaddr_str = inet_ntoa( *cit );
	while ( ++cit != s_hostaddr.cend() )
		hostaddr_str += std::string( ", " ) + inet_ntoa( *cit );
	wprintf_s( L" ok (%lu): %S\n", s_hostaddr.size(), hostaddr_str.c_str() );

	// подключаемся к найденному с использованием первого адреса из списка
	return connect( s_hostaddr.front(), server_port, security );
}

smtp::wsalib::status smtp::client::connect(
	_in const in_addr &server_address,
	_in port_t server_port,
	_in connect_security security /*= connect_security::no_cypher*/
) noexcept
{
	sockaddr_in sockaddr = { 0 };
	sockaddr.sin_family = AF_INET;			// то же семейство, что и в ф-ии socket()
	sockaddr.sin_port = htons(server_port);
	sockaddr.sin_addr = server_address;
	//sockaddr.sin_addr.s_addr = inet_addr("217.69.139.162");

	wprintf_s( L"подключение к %S:%hu...", inet_ntoa(server_address), server_port );
	if ( ::connect( m__socket, reinterpret_cast<const ::sockaddr*>(&sockaddr), sizeof( sockaddr ) ) )
	{
		const auto error = wsalib::get_error();
		TRACE_ERROR( L"connect(), error: %li", error );

		wprintf_s( L" ошибка %li\n", error );
		return error;
	}

	m__is_connected = true;
	switch ( security )
	{
	case connect_security::ssl:
		try
		{
			m__openssl.reset( new openssl() );
			m__openssl->connect( m__socket );
		} 
		catch ( const std::system_error &error )
		{
			TRACE_ERROR( L"ssl error: \"%s\"", error.what() );

			const auto error_code = error.code().value();
			wprintf_s( L" ошибка ssl %li\n", error_code );

			return error_code;
		}
		break;
	}

	receive();	

	const auto result = get_result();
	220 == result ? wprintf_s( L" ok\n" ) : wprintf_s( L" ok, code %i\n", result );

	return ERROR_SUCCESS;
}

smtp::wsalib::status smtp::client::disconnect(
	_in disconnect_operations operations /*= disconnect_operations::both*/
) {
	if ( is_secured() )
		m__openssl.reset();

	if ( !::shutdown( m__socket, static_cast<int>(operations) ) )
	{
		m__is_connected = false;
		return ERROR_SUCCESS;
	}

	const auto error = wsalib::get_error();
	TRACE_ERROR( L"shutdown(), error: %li", error );

	return error;
}

bool smtp::client::is_secured(
) const noexcept
{
	return nullptr != m__openssl.get();
}

SOCKET smtp::client::get_socket(
) const noexcept
{
	return m__socket;
}

char* smtp::client::get_buffer(
) noexcept
{
	return m__buffer;
}
const char* smtp::client::get_buffer(
) const noexcept
{
	return m__buffer;
}

void smtp::client::send_ehlo(
	_in ansicstr_t client_name /*= nullptr */
) {
	if ( !client_name )
		client_name = "client_name";

	send( "EHLO %s\r\n", client_name );
	receive();

	int res_code;
	if ( check_result( 250, &res_code ) )
		return;

	throw std::logic_error( m__buffer );
}

size_t smtp::client::send(
	_in ansicstr_t format,
	_in ...
) {
	va_list va_args;
	va_start( va_args, format );
	const auto result = send( format, va_args );
	va_end( va_args );
	return result;
}

size_t smtp::client::send(
	_in ansicstr_t format, 
	_in va_list va_args
) {
	auto length = vsprintf_s( m__buffer, m__buffer_size, format, va_args );
	assert( length >= 0 );

	const auto result = is_secured() ? send__ssl( length ) : send__no_cypher( length );
	TRACE_NORMAL( L"C> %S", m__buffer );

	return result;	
}

size_t smtp::client::send__ssl(
	_in size_t length
) {
	return m__openssl->write( m__buffer, length );
}

size_t smtp::client::send__no_cypher(
	_in size_t length
) {
	const auto result = ::send( m__socket, m__buffer, length, 0 );
	if ( result > 0 )
		return result;

	const auto error = wsalib::get_error();
	TRACE_ERROR( L"send(), error: %li", error );

	const std::string error_string( "send(): " + std::to_string( error ) );
	throw std::system_error( std::error_code( error, std::system_category() ), error_string.c_str() );
}

size_t smtp::client::receive__no_cypher(
) {
	const int result = ::recv( m__socket, m__buffer, m__buffer_size, 0 );
	if ( result > 0 )
	{
		m__buffer[result] = '\0';
		return result;
	}

	const auto error = wsalib::get_error();
	TRACE_ERROR( L"recv(), error: %li", error );

	const std::string error_string( "recv(): " + std::to_string( error ) );
	throw std::system_error( std::error_code( error, std::system_category() ), error_string.c_str() );
}

size_t smtp::client::receive__ssl(
) {
	return m__openssl->read( m__buffer, m__buffer_size );
}

size_t smtp::client::receive(
) {
	const auto result = is_secured() ? receive__ssl() :	receive__no_cypher();	
	TRACE_NORMAL( L"S> %S", m__buffer );
	return result;
}

int smtp::client::get_result(
	_out _option ansicstr_t *data /*= nullptr */
) const
{
	const auto result = strtoul( m__buffer, const_cast<char**>( data ), 10 );

	if ( data )
	{
		auto &p_data = *data;
		while ( (*p_data == ' ') || (*p_data == '-') )
			++p_data;
	}

	return result;
}

bool smtp::client::check_result(
	_in int result_expected,
	_out int *p_result /*= nullptr */
) const
{
	const auto result = get_result();
	if ( p_result )
		*p_result = result;
	if ( result_expected == result )
		return true;

	TRACE_ERROR( L"result: %i (expected: %i)", result, result_expected );
	return false;
}


bool smtp::client::auth(
	_in ansicstr_t user_name,
	_in ansicstr_t user_pass,
	_in auth_type auth_type
) {
	switch ( auth_type )
	{
	case auth_type::plain:
		return auth_plain( user_name, user_pass );
	
	default:
		TRACE_ERROR( L"authentication of [auth_type = %i] is not supported by client yet", static_cast<int>( auth_type ) );
		const std::string &error = "authentication of [auth_type = " + std::to_string( static_cast<int>( auth_type ) ) + "] is not supported by client yet";
		throw std::logic_error( error );
	}
}

bool smtp::client::auth_plain(
	_in ansicstr_t user_name,
	_in ansicstr_t user_pass,
	_in bool is_forced /*= true */
) {
	// формат комбинированной строки для передачи - "\0<user_name>\0<user_pass>"
	std::vector<byte_t> combined;
	combined.push_back( '\0' );
	for ( ansicstr_t p = user_name; *p; ++p )
		combined.push_back( *p );
	combined.push_back( '\0' );
	for ( ansicstr_t p = user_pass; *p; ++p )
		combined.push_back( *p );

	// кодируем комбинированную строку в base64
	const auto &combined_hash = base64::encode( combined.data(), combined.size() );

	// и отправляем серверу
	if ( is_forced )										// в форсированном режиме
		send( "AUTH PLAIN %s\r\n", combined_hash.c_str() );		
	else													// в режиме с предварительным уведомлением и получением ответа
	{
		send( "AUTH PLAIN\r\n" );
		receive();

		if ( !check_result( 334 ) )
			throw std::logic_error( m__buffer );

		send( "%s\r\n", combined_hash.c_str() );
	}

	receive();	
	switch ( get_result() )
	{
	case 235:
		return true;
	case 535:
		return false;
	default:
		throw std::logic_error( m__buffer );
	}
	
	return true;
}