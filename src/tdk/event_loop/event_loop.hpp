#ifndef __tdk_event_loop_h__
#define __tdk_event_loop_h__

#include <atomic>

#if defined ( linux ) || defined( __linux )
#include <tdk/io/epoll.hpp>
#elif defined( _WIN32 )
#include <tdk/io/iocp.hpp>
#endif

#include <tdk/event_loop/scheduler.hpp>
#include <tdk/threading/spin_lock.hpp>

#include <thread>
#include <list>

namespace tdk {

class event_loop{
public:

#if defined ( linux ) || defined( __linux )
	typedef tdk::io::epoll io_impl_type;
#elif defined( _WIN32 )
	typedef tdk::io::iocp io_impl_type;
#endif
	event_loop( void );
	~event_loop( void );
	
	// thread safe
	void execute( tdk::task* t );

	void run( void );
	
	bool in_loop( void );

	void schedule( tdk::timer_task* tt );
	void cancel( tdk::timer_task* tt );

	template < typename T_handler >
	void schedule( const T_handler& handler , const tdk::date_time& expired_at );

	void add_active( void );
	void remove_active( void );

	io_impl_type& io_impl( void );

	static event_loop& default_loop( void );
private:
	int _run_tasks( void );
private:
	tdk::threading::spin_lock _lock;
	std::atomic<int> _active_handles;    
	tdk::scheduler _scheduler;
	tdk::slist_queue< tdk::task > _tasks;

	/*
	tdk::slist_queue< tdk::task > _tasks;
	tdk::slist_queue< tdk::task > _tasks_thread;
	*/
	std::thread::id _thread_id;
	io_impl_type _io_impl;
};

template < typename T_handler >
void event_loop::schedule( const T_handler& handler 
						  , const tdk::date_time& expired_at )
{
	class timer_task_ex : public timer_task {
	public:
		timer_task_ex( const T_handler& h ) 
			: timer_task( &timer_task_ex::on_task , nullptr )
			, _handler(h)
		{
		}

		~timer_task_ex( void ) {
		}

		static void on_task( tdk::task* t ) {
			timer_task_ex* impl
				= static_cast< timer_task_ex* >(t);
			(impl->_handler)();
			delete impl;
		}
	private:
		T_handler _handler;
	};

	tdk::timer_task* op = new timer_task_ex( handler );
	op->expired_at( expired_at );
	schedule(  op );
}

}
#endif
