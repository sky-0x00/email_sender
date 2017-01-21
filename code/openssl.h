#pragma once

#include <include\ossl_typ.h>
#include <winsock2.h>

#define WSAE_SSL_INIT		12000					// ошибка инициализации SSL
#define WSAE_SSL_CONNECT	WSAE_SSL_INIT + 1		// ошибка создания защищенного SSL-подключения
#define WSAE_SSL_READ		WSAE_SSL_INIT + 2		// ошибка приема и чтения зашифрованных данных
#define WSAE_SSL_WRITE		WSAE_SSL_INIT + 3		// ошибка шифрования и передачи данных

class openssl
{
public:
	openssl();
	~openssl();

	void connect( _in SOCKET socket );
	size_t read( _out char *buffer, _in size_t buffer_size ) const;
	size_t write( _in const char *buffer, _in size_t length ) const;
	
private:
	void init();				// ssl_new
	void cleanup();				// disconnect + ssl_free

	SSL_CTX *m__context;
	SSL		*m__data;
};