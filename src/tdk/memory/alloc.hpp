#ifndef __tdk_memory_alloc_h__
#define __tdk_memory_alloc_h__

/*
	����� �̻��

	������ �ľ�������

	tc_malloc �̳� je malloc ���� �ٸ��Ϳ� ��������
	

	todo 
	1. statisics ���� ó�� 
	 - ��ó����� ó�� �Ұ�
	 - �Ҵ� Ƚ�� , �ݳ� Ƚ�� , ���Ҵ��
	2. �̺�Ʈ ��� �޸� �ݳ� ó�� 
	3. last error ó��
	 - over flow
	 - corrupted
	 - dup free
*/
namespace tdk { namespace memory { 

typedef void* (*alloc_hook_fn)(std::size_t);
typedef void* (*realloc_hook_fn)(void*, std::size_t);
typedef void  (*free_hook_fn)(void*);

void* alloc( std::size_t size );
void* realloc( void* p , std::size_t size );
void  free( void* p );

void  alloc_hook( alloc_hook_fn function );
void  realloc_hook( realloc_hook_fn function );
void  free_hook( free_hook_fn function );

alloc_hook_fn alloc_hook( void );
realloc_hook_fn realloc_hook( void );
free_hook_fn free_hook( void );

struct malloc_allocator {
	static void* alloc( std::size_t size );
	static void* realloc( void* p , std::size_t size );
	static void  free( void* p );
};

struct lib_allocator {
	static void* alloc( std::size_t size );
	static void* realloc( void* p , std::size_t size );
	static void  free( void* p );
};

struct tc_allocator {
	static void* alloc( std::size_t size );
	static void* realloc( void* p , std::size_t size );
	static void  free( void* p );
};

}}

#endif