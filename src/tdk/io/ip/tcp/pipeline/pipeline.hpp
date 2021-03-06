#ifndef __tdk_tcp_pipeline_h__
#define __tdk_tcp_pipeline_h__

#include <tdk/io/ip/socket.hpp>
#include <tdk/io/task.hpp>
#include <tdk/io/ip/tcp/pipeline/filter_chain.hpp>
#include <tdk/io/ip/tcp/pipeline/config.hpp>
#include <tdk/util/rc_ptr.hpp>
#include <atomic>
#include <list>

namespace tdk {
class event_loop;
namespace io {
namespace ip {
namespace tcp {

class channel;
class pipeline {
public:
	pipeline( tcp::channel* channel );
	~pipeline( void );

	void add( const std::string& name  , filter* f );

	tcp::channel* channel( void );

	filter* in_bound_filter( void );
	filter* out_bound_filter( void );

	filter* find( const std::string& name );
private:
	tcp::channel* _channel;
	filter_chain _chain;
};

}}}}

#endif
