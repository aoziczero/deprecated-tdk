#ifndef __tdk_memory_statistics_h__
#define __tdk_memory_statistics_h__

#include <tdk/memory/memory.hpp>
#include <atomic>

namespace tdk { namespace memory {

struct statistics {
	std::atomic<int64_t> alloc_req_count;	// �Ҵ� ��û Ƚ��
	std::atomic<int64_t> pooling_bytes;		// Ǯ�� �������� bytes
	std::atomic<int64_t> free_req_count;	// ���� ��û Ƚ��
	std::atomic<int64_t> base_alloc_count;	// 
	std::atomic<int64_t> base_free_count;	// 
	std::atomic<int64_t> return_memory_call_count;	// Ǯ ���� �ִ�ġ�� ���� Ƚ�� 
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
