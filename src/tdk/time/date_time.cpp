#include "stdafx.h"
#include <tdk/time/date_time.hpp>
#include <tdk/time/time_span.hpp>
#include <tdk/time/ctime.hpp>
#include <tdk/time/remote_time.hpp>
#include <tdk/util/singleton.hpp>

using namespace tdk::time;

namespace tdk {
	
date_time::date_time( void ) : _time(0) {}

date_time::date_time( const uint64_t microSec ) 
:_time( microSec ) {
}

date_time::date_time( const date_time& rhs ) 
:_time( rhs._time ) {
}

date_time::date_time( const uint16_t year 
					 , const uint16_t month 
					 , const uint16_t day 
					 , const uint16_t hour 
					 , const uint16_t minute 
					 , const uint16_t second  ) 
{
	time_span span = time_span::from_seconds( second );
	span += time_span::from_minutes( minute );
	span += time_span::from_hours( hour );
	span += time_span::from_days( ctime::daycount( 1970 , year -1 ) );
	// day �� 1 ~ .. �̹Ƿ� ������ ó���ϱ� ���ؼ� - 1 �ؾ���/
	span += time_span::from_days( ctime::daycount( year , month , day - 1 ) );
	_time = span.delta();
}

date_time& date_time::operator=(const date_time& rhs ) {
    this->_time = rhs._time;
    return *this;
}

date_time::~date_time( void ) {}

date_time date_time::utc() {
	return date_time( tick::utc() ); 
}

date_time date_time::local() {
    return local( date_time::utc() );
}

date_time date_time::utc( const date_time& local ) {
   time_span diff = date_time::utc() - date_time::local();
   return local + diff;
}

date_time date_time::local( const date_time& utc ) {
	struct timeval val = tick::to_timeval( utc.time() );
	time_t sec = val.tv_sec;
	struct tm local;
#if defined( _WIN32 )
    localtime_s( &local , &sec );
#else
    localtime_r( &sec , &local );
#endif
	return date_time( tick::from( local ) + val.tv_usec );
}

uint64_t date_time::time( void ) const {
    return _time;
}

uint64_t date_time::time_to_milli_seconds( void ) const {
	return _time /  tick::MILLI_SECOND_TO_MICRO_SECONDS;
}

bool date_time::operator==( const date_time& rhs ) const{
    return this->_time == rhs._time;
}

bool date_time::operator!=( const date_time& rhs ) const{
    return this->_time != rhs._time;
}

bool date_time::operator> ( const date_time& rhs ) const{
    return this->_time >  rhs._time;
}

bool date_time::operator>=( const date_time& rhs ) const{
    return this->_time >= rhs._time;
}

bool date_time::operator< ( const date_time& rhs ) const{
    return this->_time <  rhs._time;
}

bool date_time::operator<=( const date_time& rhs ) const{
    return this->_time <= rhs._time;
}


date_time  date_time::operator+ ( const time_span& rhs ) const{
    return date_time( _time + rhs.delta());
}

date_time& date_time::operator+=( const time_span& rhs ) {
    _time += rhs.delta();
    return *this;
}

date_time  date_time::operator- ( const time_span& rhs ) const{
    return date_time( _time - rhs.delta());
}

date_time& date_time::operator-=( const time_span& rhs ) {
    _time -= rhs.delta();
    return *this;
}

time_span  date_time::operator- ( const date_time& rhs ) const {
	return time_span::from_micro_seconds( _time - rhs._time );
}

int32_t date_time::year( void ) const {
	return tick::to_tm( _time ).tm_year + 1900;
}

int32_t date_time::month( void ) const{
	return tick::to_tm( _time ).tm_mon + 1;
}

int32_t date_time::day( void ) const{
	return tick::to_tm( _time ).tm_mday;
}

int32_t date_time::hour( void ) const{
	return tick::to_tm( _time ).tm_hour;
}

int32_t date_time::minute( void )const{
	return tick::to_tm( _time ).tm_min;
}

int32_t date_time::second( void )const{
	return tick::to_tm( _time ).tm_sec;
}
 
date_time::day_of_the_week date_time::wday( void ) const{
	return static_cast< date_time::day_of_the_week >( tick::to_tm( _time ).tm_wday );
}

std::string date_time::to_string(void){
	time::tick::systemtime st = tick::to_systemtime( _time );
	char buffer[1024];
#if defined( _WIN32 )
	sprintf_s( buffer , "%04d%02d%02d %02d%02d%02d %04d"
		, st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond , st.wMilliseconds );
#else
	sprintf( buffer , "%04d%02d%02d %02d%02d%02d %04d"
		, st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond , st.wMilliseconds );
#endif
	return std::string( buffer );
}

date_time date_time::remote( const std::string& name ) {
	return date_time::utc() + tdk::util::singleton< tdk::remote_time >::instance()->diff( name );
}

void date_time::remote( const std::string& name , const time_span& diff ) {
	tdk::util::singleton< tdk::remote_time >::instance()->register_time( name , diff );
}

}
