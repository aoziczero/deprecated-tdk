// test.tdk.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tdk/error/platform_error.hpp>
#include <tdk/util/string.hpp>

#pragma comment( lib , "../Debug/tdk.lib")

#include <iostream>
#include <string>


int _tmain(int argc, _TCHAR* argv[])
{
	// default C
	std::string old_locale = setlocale( LC_ALL , nullptr );
	// 한글에선 cp949
	old_locale = setlocale( LC_ALL , "" );

	std::wcout << tdk::string::utf_8_to_wcs( tdk::platform_error( 0 ).message() ) << std::endl;
	getchar();
	return 0;
}

/*

// windows 에는 없는 로케일
//std::locale::global(std::locale("ko_KR.UTF-8")); 

utf-8 출력 로직
SetConsoleOutputCP( CP_UTF8 );
DWORD d;
WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE)
, ec.message().c_str()
, ec.message().length()
, &d
, nullptr);*/

