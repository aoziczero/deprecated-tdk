#include "stdafx.h"
#include <tdk.adodb/adodb/query.hpp>
#include <iostream>
#include "connection_impl.hpp"

namespace tdk {
namespace adodb {

query::query()
	: _impl( new record_set_impl ())
{
	_impl->record_set_ptr.CreateInstance( __uuidof( Recordset ));
}
query::~query()
{
	close();
	_impl->record_set_ptr = NULL;
	delete _impl;
}
bool query::connection( tdk::adodb::connection& conn  )
{
	try
	{
		_impl->connection_ptr = conn.value()->connection_ptr;
		_impl->record_set_ptr->putref_ActiveConnection( _impl->connection_ptr );
	}
	catch (_com_error& err)
	{
		(void)err;
		return false;
	}
	return true;
}

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

/*
bool query::execute( const std::string& query 
	, CursorTypeEnum opt1 
	, LockTypeEnum opt2 
	, CommandTypeEnum opt3 )
	*/
bool query::execute( const std::string& query ) {
	_variant_t vt_conn( (IDispatch*) _impl->connection_ptr );
	_variant_t vt_query( query.c_str());
	try
	{
		close();
		_impl->record_set_ptr->Open(vt_query, vt_conn
			, adOpenStatic
			, adLockBatchOptimistic
			, adCmdText );
		//_record_set->MoveFirst();
	}
	catch (_com_error& err)
	{
		(void)err;
		return false;
	}
	return true;
}

bool query::is_eof()
{
	try
	{
		if( _impl->record_set_ptr == NULL )
			return true;
		if( _impl->record_set_ptr->adoEOF == VARIANT_FALSE )
			return false;
		else 
			return true;
	}
	catch (_com_error& err)
	{
		(void)err;
		return true;
	}
	return true;
}

void query::next()
{
	try
	{
		_impl->record_set_ptr->MoveNext();
	}
	catch (_com_error& err)
	{
		(void)err;
	}
}
void query::close()
{
	try
	{
		if( _impl->record_set_ptr->GetState() == adStateOpen )	
			_impl->record_set_ptr->Close();
	}
	catch (_com_error& err)
	{
		(void)err;
	}
		
}

int query::field_count()
{
	try
	{
		return (int)_impl->record_set_ptr->Fields->GetCount();
	}
	catch (_com_error& err)
	{
		(void)err;
		return 0;
	}
		
}
int query::record_count()
{
	try
	{
		return (int)_impl->record_set_ptr->GetRecordCount();
	}
	catch (_com_error& err)
	{
		(void)err;
		return 0;
	}
}	


bool query::read_int( const std::string& filed_name, int& outInt  )
{
	//_variant_t fieldIndex( index );
	try
	{
		outInt = _impl->record_set_ptr->Fields->GetItem(filed_name.c_str())->Value.intVal;
		//*outInt = _record_set->Fields->Item[fieldIndex]->Value.intVal;
	}
	catch (_com_error& err)
	{
		(void)err;
		std::cout << err.ErrorMessage() << std::endl;
		return false;
	}
	return true;
}

bool query::read_string( const std::string& filed_name , std::string& out  )
{
	//	_variant_t fieldIndex( index );
	try
	{
		_variant_t   variant(DISP_E_PARAMNOTFOUND, VT_ERROR);

		variant = _impl->record_set_ptr->Fields->GetItem(filed_name.c_str())->Value;

		if (variant.vt != VT_NULL){
			out = (LPCSTR) _bstr_t(variant.bstrVal);
			//strncpy_s( outbuf , outbufSize - 1 ,  (LPCSTR) _bstr_t(variant.bstrVal) , outbufSize - 1 );
		}else
			return false;
	}
	catch (_com_error& err)
	{
		(void)err;
		return false;
	}
	return true;
}

}}