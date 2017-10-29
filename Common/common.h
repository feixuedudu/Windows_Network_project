#ifndef __COMMON_H__
#define __COMMON_H__

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define _CRT_SECURE_NO_WARNINGS 1

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "192.168.123.47"
#define SERVER_PORT 5705

#include "../json/include/json.h"
using namespace std;

//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

struct Student
{
	string strName;
	int nAge;
	string strAddr;
	string strSex;
	double dWages;

	Student() { reset(); }
	Student(string strName, int nAge, string strAddr, string strSex, double dWages)
	{
		this->strName = strName;
		this->nAge = nAge;
		this->strAddr = strAddr;
		this->strSex = strSex;
		this->dWages = dWages;
	}

	void reset()
	{
		memset(this, 0, sizeof(Student));
	}

	bool decode(const char *pJson)
	{
		Json::Reader rd;
		Json::Value root;
		if (!rd.parse(pJson, root))
		{
			cout << "Student parse Json Date failed." << endl;
			return false;
		}
		if (root["strName"].type() == Json::stringValue)
		{
			this->strName = root["strName"].asString();
		}
		if (root["nAge"].type() == Json::intValue)
		{
			this->nAge = root["nAge"].asInt();
		}
		if (root["strAddr"].type() == Json::stringValue)
		{
			this->strAddr = root["strAddr"].asString();
		}
		if (root["strSex"].type() == Json::stringValue)
		{
			this->strSex = root["strSex"].asString();
		}
		if (root["dWages"].type() == Json::realValue)
		{
			this->dWages = root["dWages"].asDouble();
		}

		return true;
	}

	string encode()
	{
		Json::Value root;
		root["strName"] = Json::Value(strName);
		root["nAge"] = Json::Value(nAge);
		root["strAddr"] = Json::Value(strAddr);
		root["strSex"] = Json::Value(strSex);
		root["dWages"] = Json::Value(dWages);
		return root.toStyledString();
	}
};

#endif

