#pragma once

#include "openssl\include\ossl_typ.h"
#include <winsock2.h>

class openssl
{
public:
	void init();
	void connect( _in SOCKET socket );
	void cleanup();

private:
	SSL_CTX *m__context;
	SSL		*m__data;
};