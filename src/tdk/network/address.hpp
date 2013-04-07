#ifndef __tdk_io_net_address_h__
#define __tdk_io_net_address_h__

namespace tdk{
namespace network{
		
class address {
public:
	address( void );
	address( const address& rhs );
    
	address( const struct sockaddr* address , const int addressSize );

    address( const unsigned long ip_v4_addr , const int port  );
	address( const in6_addr& ip_v6_addr , const int port );

    address( const std::string& addr , const int port, const int af = AF_INET );
	
	address& operator=( const address& rhs );
    
	~address( void );
	
	void set( const struct sockaddr* address , const int size );

	std::string ip_address( void ) const;
	int		    port( void ) const;
	int			family( void ) const;

	struct sockaddr*	sockaddr( void ) const;
	const int		sockaddr_length( void ) const;
	int*			sockaddr_length_ptr( void );

public:
	static bool resolve( const std::string& ipAddress , int port , std::vector< address >& address );
#if defined ( _WIN32_WINNT ) && ( _WIN32_WINNT >= 0x0600 )
	static bool resolve( const std::wstring& ipAddress , const std::wstring& port , std::vector< address >& address );
#endif
	static address any( int port = 0 , int family = AF_INET );
	static address from( const std::string& addr , int port , int family = AF_INET );
public:
	struct sockaddr_storage _address;
	int	_length;

#if defined (_DEBUG)
	std::string _ip_address;
	int _port;
	std::string _family;
	void _set_debug_info() {
		_ip_address = ip_address();
		_port = port();
		switch( family() ) {
		case AF_INET:
			_family = "AF_INET";
			break;
		case AF_INET6:
			_family = "AF_INET6";
			break;
		default:
			_family = "UNKNOWN";
		}
	}
#endif
};

bool operator==( const address& lhs , const address& rhs ) ;
bool operator!=( const address& lhs , const address& rhs ) ;

}}


#endif