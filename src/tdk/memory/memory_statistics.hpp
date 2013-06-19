#ifndef __tdk_memory_statistics_h__
#define __tdk_memory_statistics_h__

#include <tdk/threading/atomic/atomic.hpp>
#include <tdk/memory/memory.hpp>

namespace tdk { namespace memory {

struct statistics {
	tdk::threading::atomic64::value_type alloc_req_count;	// �Ҵ� ��û Ƚ��
	tdk::threading::atomic64::value_type pooling_bytes;		// Ǯ�� �������� bytes
	tdk::threading::atomic64::value_type free_req_count;	// ���� ��û Ƚ��
	tdk::threading::atomic64::value_type base_alloc_count;	// 
	tdk::threading::atomic64::value_type base_free_count;	// 
	tdk::threading::atomic64::value_type return_memory_call_count;	// Ǯ ���� �ִ�ġ�� ���� Ƚ�� 
	statistics( void ) :
		alloc_req_count(0)
		,pooling_bytes(0)
		,free_req_count(0)
		,base_alloc_count(0)
		,base_free_count(0)
		,return_memory_call_count(0)
	{
	}
	static statistics* tc_statistics;
	static statistics* base_statistics;
};

struct lib_statistics {

};

#ifndef ENABLE_MEMORY_STATISTICS
#define ENABLE_MEMORY_STATISTICS
#endif

}}

#endif
