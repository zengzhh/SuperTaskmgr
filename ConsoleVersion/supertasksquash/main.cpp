// supertasktest.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "MySupertaskmgr.h"
#include "easylog.h"

int main()
{
	//EasyLog::init(L"D:\\clinkapi.log");
	EasyLog::printf("\n\n");
	LOG_INFO("(%s %s) %d", __DATE__, __TIME__, GetCurrentProcessId());

	LOG_INFO("%d\n", sizeof(LARGE_INTEGER));
	MySupertaskmgr my;
	for (int i = 0; i < 10000000; i++)
	{
		Sleep(10000);
		my.Refresh();
		//Sleep(1000);
		//my.Refresh();
		//std::string str1 = my.GetGlobalInfo();
		//std::string str2 = my.GetTop5Info();
		//LOG_INFO("%s, %s\n\n", str1.c_str(), str2.c_str());
	}
	getchar();
    return 0;
}

