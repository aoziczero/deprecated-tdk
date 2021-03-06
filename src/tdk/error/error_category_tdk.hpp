#ifndef __tdk_error_category_tdk_h__
#define __tdk_error_category_tdk_h__

#include <tdk/tdk.hpp>

namespace tdk {

enum class errc {
	tdk_success = 0 ,
	tdk_fail ,
	tdk_tls_not_initialized ,
	tdk_user_abort ,
    tdk_invalid_call ,
	tdk_network_user_abort = 0x1000 ,
	tdk_network_remote_closed ,
    tdk_epoll_error , 
    tdk_epoll_hang_up ,
    tdk_invalid_event ,
	tdk_errc_end ,
};

class error_category_tdk : public std::error_category {
public:
	error_category_tdk(void);
	virtual ~error_category_tdk(void) _NOEXCEPT ;

	virtual const char *name() const _NOEXCEPT ;
	virtual std::string message(int _Errval) const ;
};
/*
class error_category_tdk : public tdk::error_category {
public:
	error_category_tdk (void);
	virtual ~error_category_tdk (void) ;
	virtual const TCHAR *name() const;
	virtual tdk::tstring message(int _Errval) const ;
};
*/
}

#endif
