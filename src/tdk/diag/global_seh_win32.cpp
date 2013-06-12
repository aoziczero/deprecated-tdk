#include "stdafx.h"
#include <tdk/diag/global_seh_win32.hpp>

namespace tdk {
namespace diag {
namespace detail {
	static global_seh* g_seh = nullptr;

	static LONG __stdcall handle_exception( EXCEPTION_POINTERS *pExp ) {
		if ( g_seh != nullptr ) {
			g_seh->handle_exception( pExp );
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}
}

global_seh::global_seh(void)
{
}


global_seh::~global_seh(void)
{
}

bool global_seh::install( void ) {
	detail::g_seh = this;	
	SetUnhandledExceptionFilter( &tdk::diag::detail::handle_exception );
	return true;
}

void global_seh::register_handler( seh_handler* handler ) {
	_handlers.push_back( handler );
}

void global_seh::handle_exception( EXCEPTION_POINTERS *pExp ) {
	for ( auto it : _handlers ) {
		it->handle_exception( pExp );
	}
}

}}

