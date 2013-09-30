#ifndef __tdk_loop_timer_scheduler_h__
#define __tdk_loop_timer_scheduler_h__

#include <list>
#include <tdk/loop/timer_req.hpp>

namespace tdk {
class timer_scheduler{
public:
    timer_scheduler( void );
    ~timer_scheduler( void );

    void schedule( tdk::timer_req* req );
    void cancel( tdk::timer_req* req );
    void drain( void );
    tdk::time_span wake_up_after( void );
    bool is_empty(void);
private:
    std::list< tdk::timer_req* > _reqs;
};

}

#endif