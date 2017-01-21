#include "stdafx.h"

#include "openssl.h"
#include "openssl\include\ssl.h"
#include "openssl\include\err.h"
#include <system_error>
#include "types.h"
#include "smtp.h"

#define TIME_IN_SEC		1*60		// how long client will wait for server response in non-blocking mode

#pragma comment( lib, "openssl\\libeay32.lib" )
#pragma comment( lib, "openssl\\ssleay32.lib" )

void openssl::init()
{
	SSL_library_init();
	SSL_load_error_strings();
	m__context = SSL_CTX_new( SSLv3_client_method() );
	assert( m__context );
	//if ( !m__context )
	//{
	//	TRACE_ERROR( L"ssl-context problem" );
	//	throw std::system_error( -1, std::system_category() );
	//}
}

void openssl::connect(
	_in SOCKET socket
) {
	assert( m__context );
	
	m__data = SSL_new (m__context);   
	assert(m__data);

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
				throw std::system_error( error, std::system_category() );
			}
			if(!res)
			{
				//timeout
				FD_ZERO(&fdwrite);
				FD_ZERO(&fdread);

				const auto error = smtp::wsalib::get_error();
				TRACE_ERROR( L"network timeout, error: %li", error );
				throw std::system_error( error, std::system_category() );
			}
		}
		res = SSL_connect( m__data );
		switch( SSL_get_error( m__data, res ) )
		{
		  case SSL_ERROR_NONE:
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
			return;
			break;
              
		  case SSL_ERROR_WANT_WRITE:
			write_blocked = 1;
			break;

		  case SSL_ERROR_WANT_READ:
			read_blocked = 1;
			break;
              
		  default:	      
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
	
			TRACE_ERROR( L"ssl-context problem" );
			throw std::system_error( -1, std::system_category() );

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