#include "stdafx.h"

#include "openssl.h"
#include <include\ssl.h>
#include <include\err.h>
#include <system_error>
#include <string>
#include "types.h"
#include "smtp.h"

#define TIME_IN_SEC		1*60		// how long client will wait for server response in non-blocking mode

#pragma comment( lib, "libssl.lib" )

openssl::openssl(
) :
	m__context( nullptr ),
	m__data( nullptr )
{
	init();
}

openssl::~openssl(
) {
	cleanup();
}

void openssl::init()
{
	SSL_library_init();
	SSL_load_error_strings();

	m__context = SSL_CTX_new( SSLv23_client_method() );
	if ( m__context )
		return;

	TRACE_ERROR( L"SSL_CTX_new(): NULL" );

	std::string error_string( STRINGIZE(WSAE_SSL_CONNECT) );
	error_string += ", SSL_CTX_new(): NULL";

	throw std::system_error( std::error_code( WSAE_SSL_CONNECT, std::system_category() ), error_string.c_str() );
}

void openssl::connect(
	_in SOCKET socket
) {
	assert( m__context );
	assert( !m__data );
	
	m__data = SSL_new( m__context );
	if ( !m__data )
	{
		TRACE_ERROR( L"SSL_new(): NULL" );
		
		std::string error_string( STRINGIZE(WSAE_SSL_CONNECT) );
		error_string += ", SSL_new(): NULL";

		throw std::system_error( std::error_code( WSAE_SSL_CONNECT, std::system_category() ), error_string.c_str() );
	}

	SSL_set_fd( m__data, static_cast<int>(socket) );
    SSL_set_mode( m__data, SSL_MODE_AUTO_RETRY );

	int res = 0;
	fd_set fdwrite;
	fd_set fdread;
	int write_blocked = 0;
	int read_blocked = 0;

	timeval time;
	time.tv_sec = TIME_IN_SEC;
	time.tv_usec = 0;

	while(1)
	{
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdread);

		if(write_blocked)
			FD_SET(socket, &fdwrite);
		if(read_blocked)
			FD_SET(socket, &fdread);

		if(write_blocked || read_blocked)
		{
			write_blocked = 0;
			read_blocked = 0;
			if((res = ::select( socket + 1, &fdread, &fdwrite, NULL, &time) ) == SOCKET_ERROR)
			{
				FD_ZERO(&fdwrite);
				FD_ZERO(&fdread);

				const auto error = smtp::wsalib::get_error();
				TRACE_ERROR( L"select(), error: %li", error );
				
				std::string error_string( STRINGIZE(WSAE_SSL_CONNECT) );
				error_string += ", select(): " + std::to_string( error );

				throw std::system_error( std::error_code( WSAE_SSL_CONNECT, std::system_category() ), error_string.c_str() );
			}
			if(!res)
			{
				//timeout
				FD_ZERO(&fdwrite);
				FD_ZERO(&fdread);
				
				TRACE_ERROR( L"%li (%s)", WSAETIMEDOUT, STRINGIZE(WSAETIMEDOUT) );

				std::string error_string( STRINGIZE(WSAE_SSL_CONNECT) );
				error_string += ", " + std::to_string( WSAETIMEDOUT ) + " (" + STRINGIZE(WSAETIMEDOUT) + ")";

				throw std::system_error( std::error_code( WSAE_SSL_CONNECT, std::system_category() ), error_string.c_str() );
			}
		}
		res = SSL_connect( m__data );
		const auto error = SSL_get_error( m__data, res );
		switch( error )
		{
		  case SSL_ERROR_NONE:
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
			return;
              
		  case SSL_ERROR_WANT_WRITE:
			write_blocked = 1;
			break;

		  case SSL_ERROR_WANT_READ:
			read_blocked = 1;
			break;
              
		  default:	      
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
	
			TRACE_ERROR( L"SSL_connect(), error: %li", error );

			std::string error_string( STRINGIZE(WSAE_SSL_CONNECT) );
			error_string += ", SSL_connect(): " + std::to_string( error );

			throw std::system_error( std::error_code( WSAE_SSL_CONNECT, std::system_category() ), error_string.c_str() );

		}	// switch
	}	// while (1)
}

void openssl::cleanup()
{
	if( m__data )
	{
		SSL_shutdown( m__data );		/* send SSL/TLS close_notify */
		SSL_free( m__data );
		m__data = nullptr;
	}
	if ( m__context )
	{
		SSL_CTX_free( m__context );
		m__context = nullptr;

		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
	}
}

size_t openssl::read( 
	_out char *buffer, 
	_in size_t buffer_size 
) const 
{
	const auto result = SSL_read( m__data, buffer, buffer_size );
	if ( result > 0 )
	{
		buffer[result] = '\0';
		return result;
	}

	const auto error = SSL_get_error( m__data, result );
	TRACE_ERROR( L"SSL_read(), error: &li", error );

	std::string error_string( STRINGIZE(WSAE_SSL_READ) );
	error_string += ", SSL_read(): " + std::to_string( error );

	throw std::system_error( std::error_code( WSAE_SSL_READ, std::system_category() ), error_string.c_str() );
}

size_t openssl::write(
	_in const char *buffer,
	_in size_t length
) const
{
	const auto result = SSL_write( m__data, buffer, length );
	if ( result > 0 )
		return result;

	const auto error = SSL_get_error( m__data, result );
	TRACE_ERROR( L"SSL_write(), error: &li", error );

	std::string error_string( STRINGIZE(WSAE_SSL_WRITE) );
	error_string += ", SSL_write(): " + std::to_string( error );

	throw std::system_error( std::error_code( WSAE_SSL_WRITE, std::system_category() ), error_string.c_str() );
}