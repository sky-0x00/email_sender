#pragma once

#include <winsock2.h>
#include <list>

#define NETWORK_SUPPORT( ver_hi, ver_lo )		smtp::wsalib wsalib( ver_hi, ver_lo )

bool operator <( _in const in_addr &lhs, _in const in_addr &rhs );

namespace smtp
{
	typedef uint16_t port_t;

	struct default
	{
		static const port_t port = 25;
		static const size_t buffer_size = 512;
	};

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

		enum class operations
		{
			receive = 0,
			send,
			both			// receive + send
		};
		
		// данные по возвращенному адресу д.б. освобождены через FreeAddrInfoW()
		static wsalib::status resolve_hostname( _in cstr_t hostname, _out std::list<in_addr> &address_list );

		// выполнение подключения к узлу
		wsalib::status connect( _in cstr_t server_name, _in port_t server_port = default::port );
		wsalib::status connect( _in const in_addr &server_address, _in port_t server_port = default::port );

		wsalib::status disconnect( _in operations operations = operations::both );

		int send( _in cdata_t format, _in ... ) const;
		int receive();

	public:
		SOCKET get_socket() const noexcept;
		__declspec( property ( get = get_socket ) ) SOCKET socket;

		data_t get_buffer() noexcept;
		cdata_t get_buffer() const noexcept;
		__declspec( property ( get = get_buffer ) ) data_t buffer;
		__declspec( property ( get = get_buffer ) ) cdata_t buffer;

	private:
		SOCKET			m__socket;
		bool			m__is_connected;
		data_t			m__buffer;
		const size_t	m__buffer_size;
	};
}