#pragma once

#include <winsock2.h>
#include <list>
#include <memory>
#include "openssl.h"
#include "crypto.h"
#include "utils.h"

#define NETWORK_SUPPORT( ver_hi, ver_lo )		smtp::wsalib wsalib( ver_hi, ver_lo )

bool operator <( _in const in_addr &lhs, _in const in_addr &rhs );

namespace smtp
{
	typedef uint16_t port_t;

	struct default
	{
		static const port_t port = 25;
		static const size_t buffer_size = 0x4000;		// 16 KB
	};

	std::string get_external_ip();

	class wsalib
	{
	public:
		typedef int status;

		wsalib( _in unsigned ver_hi, _in unsigned ver_lo );
		~wsalib();

		static int get_error();
	};

	class client
	{
	public:
		client( _in size_t buffer_size = default::buffer_size );
		~client();

		enum class connect_security
		{
			no_cypher,
			ssl
		};
		enum class disconnect_operations
		{
			receive = 0,
			send,
			both			// receive + send
		};
		enum class auth_type
		{
			plain = 0,
			login,
			xoauth2
		};

		struct error
		{
			int code;
			std::string message;
		};
		
		// ������ �� ������������� ������ �.�. ����������� ����� FreeAddrInfoW()
		static wsalib::status resolve_hostname( 
			_in ansicstr_t hostname, _out std::list<in_addr> &address_list );

		// ���������� ����������� � ����
		wsalib::status connect( 
			_in ansicstr_t server_name, _in port_t server_port, _in connect_security security = connect_security::no_cypher 
		) noexcept;
		wsalib::status connect( 
			_in const in_addr &server_address, _in port_t server_port, _in connect_security security = connect_security::no_cypher 
		) noexcept;

		wsalib::status disconnect( 
			_in disconnect_operations operations = disconnect_operations::both );

		size_t send( _in ansicstr_t format, _in ... );
		size_t receive();

		void send_ehlo( _in ansicstr_t client_name = nullptr );
		
		bool auth( _in ansicstr_t user_name, _in ansicstr_t user_pass, _in auth_type auth_type );
		bool auth_plain( _in ansicstr_t user_name, _in ansicstr_t user_pass, _in bool is_forced = true );

		bool mail( _in ansicstr_t address_from, _in ansicstr_t address_to, _in cstr_t message_title, _in cstr_t message_body, _out guid *p_guid = nullptr, _out std::string *id = nullptr );
		//void mail( _in ansicstr_t address_from, _in const ansicstr_t *addresslist_to, _in size_t size_to, _in ansicstr_t message_title, _in ansicstr_t message_body );

		bool quit();
		bool reset();

		int get_result( _out _option ansicstr_t *data = nullptr ) const;

	public:
		SOCKET get_socket() const noexcept;
		__declspec( property ( get = get_socket ) ) SOCKET socket;

		char* get_buffer() noexcept;
		const char* get_buffer() const noexcept;
		__declspec( property ( get = get_buffer ) ) char* buffer;
		__declspec( property ( get = get_buffer ) ) const char* buffer;

	private:		
		std::string encode_mime( _in cstr_t str, _in crypto::method crypto_method );
		std::string encode_mime__body( _in cstr_t str, _in crypto::method crypto_method );			// ����� 8bit, 7bit, binary - ���� �� �����������
		bool check_result( _in int result_expected, _out int *p_result = nullptr ) const;

		bool is_secured() const noexcept;
		
		size_t receive__ssl();
		size_t receive__no_cypher();

		size_t send__ssl( _in size_t length );
		size_t send__no_cypher( _in size_t length );
		size_t send( _in ansicstr_t format, _in va_list va_args );

		std::unique_ptr<openssl> m__openssl;
		SOCKET			m__socket;
		bool			m__is_connected;
		char*			m__buffer;
		const size_t	m__buffer_size;
		error			m__error;
	};
}