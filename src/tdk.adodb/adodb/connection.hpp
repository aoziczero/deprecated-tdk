#ifndef __tdk_adodb_connection_h__
#define __tdk_adodb_connection_h__

#import "c:/Program Files/Common Files/System/ado/msado15.dll" no_namespace rename("EOF", "adoEOF")

namespace tdk {
namespace adodb {

class connection {

public:
	/// ������ 
	connection();
	/// �Ҹ���
	~connection();

	//L"Provider=SQLOLEDB.1;Persist Security Info=True;Password = kttcop;User ID = kttddns;Initial Catalog = KTT_DDNS;Data Source = 172.16.71.200";
	/// \brief DB ���� ������ �����ϱ� ���� �Լ�
	bool open( const char* connstr );
	/// \brief DB ���� ���� ����
	void close();

	/// ADOQuery ��� ����ϴ� Connection ��ü�� �������� �Լ�
	_ConnectionPtr value();
		
	/// ������ �����ž� �ִ��� Ȯ���ϴ� �Լ�
	bool is_connected();

private:
	 _ConnectionPtr _connection;
};

}}
#endif