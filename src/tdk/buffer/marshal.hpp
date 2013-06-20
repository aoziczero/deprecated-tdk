#ifndef __tdk_buffer_marshal_h__
#define __tdk_buffer_marshal_h__

namespace tdk {
namespace buffer {

enum class endian {
		big
	,	littne
};

endian host_endian( void );

namespace detail {

struct host_to_host {
	template < typename R , typename T >
	static R value( T t );

	template<> static uint64_t value( uint64_t value ) {
		return value;
	}

	template<> static uint32_t value( uint32_t value ) {
		return value;
	}

	template<> static uint16_t value( uint16_t value ) {
		return value;
	}

	template<> static uint64_t value( double value ) {
		uint64_t val;
		memcpy(&val,&value,sizeof(val));
		return val;
	}

	template<> static uint32_t value( float value ) {
		uint32_t val;
		memcpy(&val,&value,sizeof(val));
		return val;
	}
	template <> static double value( uint64_t value ) {
		double val;
		memcpy(&val,&value,sizeof(val));
		return val;
	}
	template <> static float value( uint32_t value ) {
		float val;
		memcpy(&val,&value,sizeof(val));
		return val;
	}
};


struct host_to_network {
	template < typename R , typename T >
	static R value( T t );

	template<> static uint64_t value( uint64_t value ) {
		return htonll( value );
	}

	template<> static uint32_t value( uint32_t value ) {
		return htonl( value );
	}

	template<> static uint16_t value( uint16_t value ) {
		return htons( value );
	}

	template<> static uint64_t value( double value ) {
		return htond( value );
	}

	template<> static uint32_t value( float value ) {
		return htonf( value );
	}
};

struct network_to_host {
	template < typename R , typename T >
	static R value( T t );

	template <> static uint64_t value( uint64_t value ) {
		return ntohll( value );
	}

	template <> static uint32_t value( uint32_t value ) {
		return ntohl( value );
	}
	template <> static uint16_t value( uint16_t value ) {
		return ntohs( value );
	}
	template <> static double value( uint64_t value ) {
		return ntohd( value );
	}
	template <> static float value( uint32_t value ) {
		return ntohf( value );
	}
};
}

template < typename T , typename C = tdk::buffer::detail::network_to_host >
class marshal_reader {
public:
	marshal_reader( T& t )
		: _buffer( t ) 
	{
	}

	~marshal_reader( void ) {
	}
	
	template < typename T >
	bool is_readable(void) {
		return _buffer.rd_size() >= sizeof( T );
	}

	T& buffer( void ) {
		return _buffer;
	}

	template < typename T >
	marshal_reader& operator>> ( T& t ) {
		_buffer >> t;
		t = C::value<T,T>(t);
		return *this;
	}

	template <>
	marshal_reader& operator>> ( double& t ) {
		uint64_t read;
		_buffer >> read;
		t = C::value<double,uint64_t>(read);
		return *this;
	}

	template <>
	marshal_reader& operator>> ( float& t ) {
		uint32_t read;
		_buffer >> read;
		t = C::value<float,uint32_t>(read);
		return *this;
	}

	template <>
	marshal_reader& operator>> ( uint8_t& t ) {
		_buffer >> t;
		return *this;
	}
private:
	T& _buffer;
};

template < typename T ,  typename C = tdk::buffer::detail::host_to_network>
class marshal_writer {
public:
	marshal_writer( T& t )
		: _buffer(t)
	{
	}

	~marshal_writer( void ) {
	}

	T& buffer( void ) {
		return _buffer;
	}
	
	template < typename T >
	marshal_writer& operator<< ( const T& t ) {
		_buffer << C::value<T,T>(t);
		return *this;
	}

	template <>
	marshal_writer& operator<< ( const double& t ) {
		_buffer << C::value<uint64_t,double>(t);
		return *this;
	}

	template <>
	marshal_writer& operator<< ( const float& t ) {
		_buffer << C::value<uint32_t,float>(t);
		return *this;
	}

	template <>
	marshal_writer& operator<< ( const uint8_t& t ) {
		_buffer << t;
		return *this;
	}
private:
	T& _buffer;
};

}}

#endif