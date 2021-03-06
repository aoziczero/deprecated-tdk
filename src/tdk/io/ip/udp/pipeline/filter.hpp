/*
 * filter.hpp
 *
 *  Created on: 2013. 10. 18.
 *      Author: tk
 */

#ifndef UDP_FILTER_HPP_
#define UDP_FILTER_HPP_

#include <tdk/io/ip/udp/channel.hpp>

namespace tdk {
namespace io {
namespace ip {
class address;
namespace udp {

class filter {
public:
	filter();
	virtual ~filter();

	udp::channel* channel( void );
	void channel( udp::channel* p );

	filter* in_bound( void );
	filter* out_bound( void );

	void in_bound( filter* f );
	void out_bound( filter* f );
	void write_out_bound( const tdk::io::ip::address& addr 
		, tdk::buffer::memory_block& msg );

	const std::string& name( void );
	void name( const std::string& n );
public:
	virtual void on_open( void );
	virtual void on_error( const std::error_code& ec );
	virtual void on_read( const tdk::io::ip::address& addr 
		, tdk::buffer::memory_block& msg );
	virtual void on_closed( void );
	virtual void do_write( const tdk::io::ip::address& addr 
		, tdk::buffer::memory_block& msg );
	virtual void on_delete( void );
private:
	udp::channel* _channel;
	filter* _in_bound;
	filter* _out_bound;
	std::string _name;
};

} 
} 
} 
} 

#endif /* FILTER_HPP_ */
