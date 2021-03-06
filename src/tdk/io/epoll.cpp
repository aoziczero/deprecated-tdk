#include "stdafx.h"
#include <tdk/io/epoll.hpp>
#include <sys/eventfd.h>

namespace tdk {
namespace io {

epoll::epoll( void )
	: _epoll_fd( epoll_create(4096) )
{
	this->register_handle(
			_wake_up_task.handle()
			, &_wake_up_task);
}

epoll::~epoll( void ) {
	close( _epoll_fd );
}

bool epoll::register_handle( int fd , io::task* task ) {
	if ( epoll_ctl( _epoll_fd
			, EPOLL_CTL_MOD
			, fd
			, task->impl()) == 0 )
		return true;
	if ( errno == ENOENT )
		return epoll_ctl( _epoll_fd
				, EPOLL_CTL_ADD
				, fd
				, task->impl()) == 0;
	return false;
}

void epoll::unregister_handle( int fd ) {
	epoll_ctl( _epoll_fd , EPOLL_CTL_DEL , fd , nullptr );
}

void epoll::wake_up( void ) {
	_wake_up_task.set();
}

void epoll::wait( const tdk::time_span& w ) {
	static const int k_max_events = 256;
	struct epoll_event events[k_max_events];
	int nev  = epoll_wait( _epoll_fd
		, events
	    , k_max_events
	    , w.total_milli_seconds());

	if ( nev == -1 ) {
		if ( errno != EINTR ) {
			return;
		}
	} else {
		for ( int i = 0 ; i < nev; ++i ){
			io::task* t =
	    			static_cast< io::task* >( events[i].data.ptr );
	    	if ( t ) {
	    		t->evt( events[i].events );
	    		(*t)();
	    	}
		}
	}
}

static void event_fd_task_handler( tdk::task* t ) {
	epoll::event_fd_task* eft =
			static_cast< epoll::event_fd_task* >(t);
	eventfd_t val = 0;
	eventfd_read( eft->handle() , &val );
}


epoll::event_fd_task::event_fd_task( void )
	: _event_fd( eventfd( 0 , EFD_NONBLOCK ))
{
	this->set_handler( &event_fd_task_handler );
	impl()->events = EPOLLIN;
}

epoll::event_fd_task::~event_fd_task( void ) {
	close( _event_fd );
}

int epoll::event_fd_task::handle( void ) {
	return _event_fd;
}

void epoll::event_fd_task::set( void ) {
	eventfd_write( _event_fd , 1 );
}

}
}
