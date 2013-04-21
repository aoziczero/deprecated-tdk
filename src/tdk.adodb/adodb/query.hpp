#ifndef __tdk_adodb_query_h__
#define __tdk_adodb_query_h__

#include <tdk.adodb/adodb/connection.hpp>

namespace tdk {
namespace adodb {
struct record_set_impl;
class query {
public:
	/// ������
	query();
	/// �Ҹ���
	~query();

	/// Connection ���� �Լ�
	bool connection( tdk::adodb::connection& conn );

	// - Ŀ��Ÿ��(ADO���)
	// 
	// 0. <adOpenForwardOnly> : ���ڵ带 ������ ���������� �ѹ��� �̵�
	// 1. <adOpenKeyset> : ���ڵ� �����̵�,���ڵ尻��,�ٸ�����ڰ��߰��� ���� �����Ұ�
	// 2. <adOpenDynamic> : ���ڵ� �����̵�,���ڵ尻��,�ٸ�����ڰ� �߰��� ���� ��������
	// 3. <adOpenStatic> : ���ڵ� �����̵�,���ڵ尻�źҰ�,���������͸� �������µ�����
	// 
	// -  Lock Ÿ��
	// 1. adLockReadOnly : �б�
	// 2. adLockRessimistic : ���ð��� �Ұ�.
	// 3. adLockOptimistic : ���ð��Ű����ϳ� update�޼��带 �����ϸ� ���žȵ�.
	// 4. adLockBatchOptimistic : ���Ÿ��� ���� �۾�����.
	// 
	// -  �ɼ�
	// 1. adCmdText : ������
	// 2. adCmdStoredProc : �������ν���
	// 3. adCmdTable : ���̺�
	//																		- from ������
	/// ���� ���� �Լ� 
	//bool execute( const std::string& query , CursorTypeEnum opt1 = adOpenStatic , LockTypeEnum opt2 = adLockBatchOptimistic , CommandTypeEnum opt3 = adCmdText );
	bool execute( const std::string& query );

	/// ���ڵ��� ������ Ȯ��
	bool is_eof();
	/// ���� ���ڵ�� �̵�
	void next();
	/// ����
	void close();

	/// �ʵ��� ���� �˾ƿ���
	int field_count();
	/// ���ڵ��� ���� �˾ƿ���
	int record_count();
	/// �ʵ��� ���� Int �� ��������
	bool read_int( const std::string& filed_name, int& outInt );
	/// �ʵ��� ���� String ���� ��������
	bool read_string( const std::string& filed_name , std::string& out  );
private:
	record_set_impl* _impl;
};

}}

#endif