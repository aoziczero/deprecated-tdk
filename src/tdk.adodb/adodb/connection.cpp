#include "stdafx.h"
#include <tdk.adodb/adodb/connection.hpp>


namespace tdk {
namespace adodb {
/// ������ 
connection::connection() {
	_connection.CreateInstance( L"ADODB.Connection" );
}
/// �Ҹ���
connection::~connection(){
	_connection = nullptr;
}

//L"Provider=SQLOLEDB.1;Persist Security Info=True;Password = kttcop;User ID = kttddns;Initial Catalog = KTT_DDNS;Data Source = 172.16.71.200";
/// \brief DB ���� ������ �����ϱ� ���� �Լ�
bool connection::open( const char* connstr ){
	try {
		_bstr_t str = connstr;
		_connection->Open( str , "" , "", adCmdUnspecified );
	} catch (_com_error& err) {
		(void)err;
		return false;
	}
	return is_connected();
}
/// \brief DB ���� ���� ����
void connection::close(){
	try {
		if( is_connected() )
			_connection->Close();
	} catch (_com_error& err) {
		(void)err;
	}
}

/// ADOQuery ��� ����ϴ� Connection ��ü�� �������� �Լ�
_ConnectionPtr connection::value(){
	return _connection;
}
		
/// ������ �����ž� �ִ��� Ȯ���ϴ� �Լ�
bool connection::is_connected(){
	return _connection->GetState() == adStateOpen;
}

}}