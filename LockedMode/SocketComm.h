#pragma once
#include "../Common/common.h"
#include <process.h>

class CSocketComm
{
public:
	CSocketComm();
	~CSocketComm();

	void Run(SOCKET sSocket, SOCKADDR_IN *sockAddr);

private:
	static unsigned __stdcall RecvDataThreadProc(void* lpParam);
	static unsigned __stdcall WorkingThreadProc(void* lpParam);

	//UTF-8��GB2312��ת��
	char* U2G(const char* utf8);
	//GB2312��UTF-8��ת��
	char* G2U(const char* gb2312);

private:
	char m_szBuff[1024];
	SOCKET m_sSocket;
	CRITICAL_SECTION m_csBuff;
	HANDLE m_hEventBuff;
	SOCKADDR_IN m_sockAddr;
	bool m_bRunning;
	/*HANDLE m_hRecvDataThread;
	HANDLE m_hWorkingThread;*/
};

