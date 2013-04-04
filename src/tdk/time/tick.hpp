#ifndef __tdk_time_tick_h__
#define __tdk_time_tick_h__

#include <tdk/tdk.hpp>

namespace tdk {
namespace time {

// ������ �ð��� 1601 �� 1 �� 1 �� ���� 100 �����ʸ��� 1 �� ����
// ���н� �ð��� 1970 �� 1 �� 1 �� ����   1 �ʸ���    1 �� ����
// ������ -> ���н� �ð� ��ȯ�� 
// 1 �ʴ� 1,000,000,000 ������ 
// ������ �ð��� 100 ������ �̹Ƿ�  10,000,000 �� ������ second ����
// 369 ���� �� 369 * 365  - 89 (1700 , 1800 , 1900 �� ������ ������ 89 ��) = 134774
// 134,774 * 24 �ð� * 60�� * 60 �� = 11,644,473,600 �� 
// ������ð� / 10,000,000 - 11,644,473,600 -> ���н� �ð�
// ( ���н��ð� + 11,644,473,600 ) * 10,000,000    Ȥ��
// ���н��ð� * 10,000,000 + 116,444,736,000,000,000 -> ������ �ð�
                                        
// UTC ����
class tick {
public:
	static const uint64_t BASE_YEAR					= 1900;
	static const uint64_t EPOCH_YEAR				= 1970;

	static const uint64_t HOUR_TO_MINUTES			= 60;
	static const uint64_t DAY_TO_HOURS				= 24;
	static const uint64_t WEEK_TO_DAYS				= 7;

	static const uint64_t MINUTE_TO_SECONDS			= 60;
	static const uint64_t HOUR_TO_SECONDS			= 60 * 60;
	static const uint64_t DAY_TO_SECONDS			= 60 * 60 * 24;
	static const uint64_t WEEK_TO_SECONDS			= 60 * 60 * 24 * 7;
	static const uint64_t EPOCH_SECONDS				= 11644473600;

	static const uint64_t SECOND_TO_MILLI_SECONDS		= 1000;
	static const uint64_t MILLI_SECOND_TO_MICRO_SECONDS= 1000;

	static const uint64_t SECOND_TO_MICRO_SECONDS	= 1000 * 1000;
	static const uint64_t MINUTE_TO_MICRO_SECONDS	= SECOND_TO_MICRO_SECONDS * MINUTE_TO_SECONDS;
	static const uint64_t HOUR_TO_MICRO_SECONDS		= SECOND_TO_MICRO_SECONDS * HOUR_TO_SECONDS;
	static const uint64_t DAY_TO_MICRO_SECONDS		= SECOND_TO_MICRO_SECONDS * DAY_TO_SECONDS;
	static const uint64_t WEEK_TO_MICRO_SECONDS		= SECOND_TO_MICRO_SECONDS * WEEK_TO_SECONDS;
	static const uint64_t EPOCH_TO_MICRO_SECONDS	= EPOCH_SECONDS			* SECOND_TO_MICRO_SECONDS;
public:
	typedef uint64_t value_type;
public:
	struct systemtime {
		systemtime( void ) {
			memset( this , 0x00 , sizeof( *this ) );				
		}
		systemtime( const SYSTEMTIME& st ) {
			memcpy( this , &st , sizeof( *this ) );				
		}
		uint16_t year;
		uint16_t month;
		uint16_t dayOfWeek;
		uint16_t day;
		uint16_t hour;
		uint16_t minute;
		uint16_t second;
		uint16_t milliseconds;
	};

	struct filetime {
		filetime( void ) : lowDateTime(0) , highDateTime(0){}
		filetime( const FILETIME& ft ) {
			memcpy( this , &ft , sizeof( *this ) );				
		}				
		uint32_t lowDateTime;
		uint32_t highDateTime;
	};

public:
	static value_type local( void );
	static value_type utc( void );
	static value_type from( const tm& t );
	static value_type from( const time_t& t );
	static value_type from( const timeval& tv );
	static value_type from( const systemtime& st );
	static value_type from( const filetime& ft );

	static tm			to_tm( const value_type v );
	static time_t		to_time_t( const value_type v );
	static timeval		to_timeval( const value_type v );
	static systemtime	to_systemtime( const value_type v );
	static filetime		to_filetime( const value_type v );

};


}}

#endif