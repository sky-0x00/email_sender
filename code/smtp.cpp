#include "stdafx.h"
#include "smtp.h"

#include <system_error>
#include <ws2tcpip.h>
#include "settings.h"

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
	throw std::system_error( result, std::system_category() );
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
	m__buffer( new byte_t[ buffer_size ] ),
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
	_in cstr_t hostname,
	_out std::list<in_addr> &address_list
) {
	address_list.clear();

	ADDRINFOW addrinfo_hint = { 0 };
	addrinfo_hint.ai_family = AF_INET;
	addrinfo_hint.ai_socktype = SOCK_STREAM;
	addrinfo_hint.ai_protocol = IPPROTO_TCP;

	PADDRINFOW p_addrinfo_result = nullptr;

	auto get_portstr = []()
	{
		static char_t port_str[ 6 ];
		_itow_s( default::port, port_str, _countof(port_str), 10 );
		return port_str;
	};	
	static const auto portstr = get_portstr();
	const auto result = GetAddrInfoW( hostname, portstr, &addrinfo_hint, &p_addrinfo_result );
	if ( result )
	{
		TRACE_ERROR( L"GetAddrInfoW(\"%s\"), error: %li", hostname, result ); 
		return result;
	}

	for ( PADDRINFOW p = p_addrinfo_result; p; p = p->ai_next )
	{
		const auto &sin_addr = reinterpret_cast<const sockaddr_in*>( p->ai_addr )->sin_addr;
		//TRACE_NORMAL( L"p_in: %S", inet_ntoa( sin_addr ) );
		address_list.push_back( sin_addr );
	}
	FreeAddrInfoW( p_addrinfo_result );

	TRACE_NORMAL( L"\"%s\", ok (%lu address)", hostname, address_list.size() );
	return ERROR_SUCCESS;
}

smtp::wsalib::status smtp::client::connect(
	_in cstr_t server_name,
	_in port_t server_port /*= default::port */
) {
	std::list<in_addr> s_hostaddr;

	// получаем все адреса узла с именем SMTP_SERVER_NAME - нашего почтового сервера для исходящих сообщений
	wprintf_s( L"получение адресов узла \"%s\"...", server_name );	
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
	return connect( s_hostaddr.front(), server_port );
}

smtp::wsalib::status smtp::client::connect(
	_in const in_addr &server_address,
	_in port_t server_port /*= default::port */
) {
	sockaddr_in sockaddr = { 0 };
	sockaddr.sin_family = AF_INET;			// то же семейство, что и в ф-ии socket()
	sockaddr.sin_port = htons(server_port);
	sockaddr.sin_addr = server_address;
	//sockaddr.sin_addr.s_addr = inet_addr("217.69.139.162");

	wprintf_s( L"подключение к %S:%hu...", inet_ntoa(server_address), server_port );
	if ( !::connect( m__socket, reinterpret_cast<const ::sockaddr*>(&sockaddr), sizeof( sockaddr ) ) )
	{
		m__is_connected = true;
		wprintf_s( L" ok\n" );
		return ERROR_SUCCESS;
	}

	const auto error = wsalib::get_error();
	TRACE_ERROR( L"connect(), error: %li", error );
	wprintf_s( L" ошибка %li\n", error );

	return error;
}

smtp::wsalib::status smtp::client::disconnect(
	_in operations operations /*= operations::both*/
) {
	if ( !::shutdown( m__socket, static_cast<int>(operations) ) )
	{
		m__is_connected = false;
		return ERROR_SUCCESS;
	}

	const auto error = wsalib::get_error();
	TRACE_ERROR( L"shutdown(), error: %li", error );
	return error;
}


SOCKET smtp::client::get_socket(
) const noexcept
{
	return m__socket;
}

data_t smtp::client::get_buffer(
) noexcept
{
	return m__buffer;
}
cdata_t smtp::client::get_buffer(
) const noexcept
{
	return m__buffer;
}

int smtp::client::send(
	_in cdata_t format, _in ...
) const
{
	//va_args va_args( format );
	va_list va_args;
	va_start( va_args, format );
	int result = vsprintf_s( m__buffer, m__buffer_size, format, va_args );
	result = ::send( m__socket, m__buffer, result, 0 );
	if ( result < 0 )
		TRACE_ERROR( L"send(), error: %li", wsalib::get_error() );
	va_end( va_args );
	return result;
}
int smtp::client::receive(
) {
	const int result = ::recv( m__socket, m__buffer, m__buffer_size, 0 );
	if ( result < 0 )
		TRACE_ERROR( L"recv(), error: %li", wsalib::get_error() );
	return result;
}