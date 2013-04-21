#include "stdafx.h"
#include <tdk.adodb/adodb/connection.hpp>
#include "connection_impl.hpp"

namespace tdk {
namespace adodb {
/// ������ 
connection::connection() 
	: _impl( new connection_impl())
{
	_impl->connection_ptr.CreateInstance( L"ADODB.Connection" );
}
/// �Ҹ���
connection::~connection(){
	_impl->connection_ptr = nullptr;
	delete _impl;
}

//L"Provider=SQLOLEDB.1;Persist Security Info=True;Password = kttcop;User ID = kttddns;Initial Catalog = KTT_DDNS;Data Source = 172.16.71.200";
/// \brief DB ���� ������ �����ϱ� ���� �Լ�
bool connection::open( const std::string& connstr){
	try {
		_bstr_t conn_bstr( connstr.c_str() );
		_impl->connection_ptr->Open( conn_bstr , "" , "", adCmdUnspecified );
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
			_impl->connection_ptr->Close();
	} catch (_com_error& err) {
		(void)err;
	}
}

/// ADOQuery ��� ����ϴ� Connection ��ü�� �������� �Լ�
connection_impl* connection::value(){
	return _impl;
}
		
/// ������ �����ž� �ִ��� Ȯ���ϴ� �Լ�
bool connection::is_connected(){
	return _impl->connection_ptr->GetState() == adStateOpen;
}

}}