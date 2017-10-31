#ifndef __COMMON_H__
#define __COMMON_H__

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define _CRT_SECURE_NO_WARNINGS 1

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "192.168.123.47"
#define SERVER_PORT 2000

#include "../json/include/json.h"
#include <iostream>
using namespace std;

struct Msg
{
	int nEvent;
	char szName[20];
	int nAge;
	char szAddr[20];
	char chSex;
	double dWages;
};

void print(const char *pszFormat, ...);

extern CRITICAL_SECTION g_csout;

//
//struct Student
//{
//	string strName;
//	int nAge;
//	string strAddr;
//	string strSex;
//	double dWages;
//
//	Student() { reset(); }
//	Student(string strName, int nAge, string strAddr, string strSex, double dWages)
//	{
//		this->strName = strName;
//		this->nAge = nAge;
//		this->strAddr = strAddr;
//		this->strSex = strSex;
//		this->dWages = dWages;
//	}
//
//	void reset()
//	{
//		memset(this, 0, sizeof(Student));
//	}
//
//	bool decode(const char *pJson)
//	{
//		Json::Reader rd;
//		Json::Value root;
//		if (!rd.parse(pJson, root))
//		{
//			cout << "Student parse Json Date failed." << endl;
//			return false;
//		}
//		if (root["strName"].type() == Json::stringValue)
//		{
//			this->strName = root["strName"].asString();
//		}
//		if (root["nAge"].type() == Json::intValue)
//		{
//			this->nAge = root["nAge"].asInt();
//		}
//		if (root["strAddr"].type() == Json::stringValue)
//		{
//			this->strAddr = root["strAddr"].asString();
//		}
//		if (root["strSex"].type() == Json::stringValue)
//		{
//			this->strSex = root["strSex"].asString();
//		}
//		if (root["dWages"].type() == Json::realValue)
//		{
//			this->dWages = root["dWages"].asDouble();
//		}
//
//		return true;
//	}
//
//	string encode()
//	{
//		Json::Value root;
//		root["strName"] = Json::Value(strName);
//		root["nAge"] = Json::Value(nAge);
//		root["strAddr"] = Json::Value(strAddr);
//		root["strSex"] = Json::Value(strSex);
//		root["dWages"] = Json::Value(dWages);
//		return root.toStyledString();
//	}
//};

#endif

