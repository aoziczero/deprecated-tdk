#ifndef __tdk_CryptoSeed128_h__
#define __tdk_CryptoSeed128_h__

namespace tdk {
namespace crypto {

class seed{
public: 
    seed( void );
    ~seed( void );

    // userKey �� 128 bit �� ��� �Ѵ�.
    // ������ 0 ���н� -1
    int open( void* userkey , int size );
    void close( void );
    
    // 128 bit ����
    bool encrypt( void* data , int size );
    // 128 bit ����
    bool decrypt( void* data , int size );

private:
    DWORD roundKey_[32];
};


}}

#endif