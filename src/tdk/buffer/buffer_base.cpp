#include "stdafx.h"
#include <tdk/buffer/buffer_base.hpp>

namespace tdk {
namespace buffer {
namespace detail {

tdk::buffer::allocator malloc_allocator;

}

buffer_base::buffer_base( std::size_t sz 
	, tdk::buffer::allocator* alloc ) 
	: _allocator( alloc )
	, _size( sz )
	, _base( nullptr )
{
	if ( _allocator == nullptr ) {
		_allocator = &detail::malloc_allocator;
	}
	_base = _allocator->alloc( sizeof( std::atomic<int> ) + _size );
	new (_base) std::atomic<int>;
	_counter()->store(0);
	add_ref();
}

buffer_base::buffer_base( void* base 
	, std::size_t sz 
	, tdk::buffer::allocator* alloc )
	: _allocator( alloc )
	, _size( sz )
	, _base( base )
{

}

buffer_base::~buffer_base( void ) {
	dec_ref();
}

buffer_base::buffer_base( const buffer_base& rhs )
	:_base ( rhs._base )
	,_size ( rhs._size )
	,_allocator ( rhs._allocator )
{
	if ( rhs._allocator == nullptr ) {
		throw std::exception( "Not Allowed");
	}
	add_ref();
}

buffer_base& buffer_base::operator=( const buffer_base& rhs ) {
	if ( rhs._allocator == nullptr ) {
		throw std::exception( "Not Allowed");
	}
	buffer_base _new( rhs );
	swap( _new );
	return *this;
}

int buffer_base::ref_count( void ) {
	std::atomic<int>* c = _counter();
	if (c) 
		return _counter()->load();
	return 1;
}

uint8_t* buffer_base::data_ptr( void ) const {
	return static_cast< uint8_t* >( _base )  + 
		(_allocator ? sizeof( std::atomic<int>) : 0 );
}

std::size_t buffer_base::size( void ) const {
	return _size;
}

std::atomic<int>* buffer_base::_counter(void) {
	return _allocator ? 
		static_cast<std::atomic<int>*>(_base)
		: nullptr;
}

int buffer_base::add_ref( void ) {
	if ( _allocator ) {
		return _counter()->fetch_add(1) + 1;
	}
	return 1;
}

int buffer_base::dec_ref( void ) {
	if ( _allocator ) {
		if ( _counter()->fetch_sub(1) - 1 == 0 ) {
			_allocator->free( _base );
			_allocator = nullptr;
			_base = nullptr;
		}
	}
	return 1;
}

void buffer_base::swap( buffer_base& rhs ) {
	std::swap( _base , rhs._base );
	std::swap( _size , rhs._size );
	std::swap( _allocator , rhs._allocator );
}

void buffer_base::reserve( std::size_t sz ) {
	if ( ref_count() == 1 && _size >= sz ) {
		return;
	}
	buffer_base new_buffer( sz , _allocator );
	memcpy( new_buffer.data_ptr() , data_ptr() , min( _size , sz ));
	swap( new_buffer );
	_size = sz;
}

}}