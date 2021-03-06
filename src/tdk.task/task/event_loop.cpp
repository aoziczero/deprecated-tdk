#include "stdafx.h"
#include <tdk.task/task/event_loop.hpp>
#include <tdk/threading/thread_local.hpp>

namespace tdk {
namespace task {
namespace detail {

tdk::threading::thread_local< event_loop* > loop;

}

event_loop::event_loop()
	: _engine( *this )
	, _scheduler(*this)
{
}

event_loop::~event_loop(){

};

bool event_loop::open( void ){
	return _engine.open();
}

void event_loop::close( void ) {
	_engine.close();
}

io_engine& event_loop::engine( void ){ 
	return _engine;
}

void event_loop::run( void ) {
	detail::loop.set( this );
	while ( true ) {
		int exp = 1;
		int change = 1;
		_ref.compare_exchange_strong(exp,change);
		//while ( _ref.compare_and_swap( 1 , 1 ) != 0 ) {
		_engine.run( _scheduler.next_schedule() );
		_scheduler.drain();
		if ( exp == 0 ) 
			return;
	}
	detail::loop.set( nullptr );
	
}

bool event_loop::run_once( const tdk::time_span& wait ) {
	detail::loop.set( this );
	int exp = 1;
	int change = 1;
	_ref.compare_exchange_strong(exp,change);
	if ( exp == 0 ) {
	//if ( _ref.compare_and_swap( 1 , 1 ) == 0 )
		return false;
	}
	_engine.run( _scheduler.next_schedule() );
	_scheduler.drain();
	detail::loop.set( nullptr );
	return true;
}

tdk::task::scheduler& event_loop::scheduler( void ) {
	return _scheduler;
}

event_loop& event_loop::default_loop( void ) {
	static event_loop loop;
	return loop;
}

void event_loop::post( tdk::task::operation* op) {
	increment_ref();
	_engine.post( op );
}

void event_loop::increment_ref( void ) {
	++_ref;
}

void event_loop::decrement_ref( void ) {
	--_ref;
}

event_loop* event_loop::current( void ) {
	return detail::loop.get();
}


}}