#include "SocketComm.h"

extern CRITICAL_SECTION g_csout;

CSocketComm::CSocketComm()
{
	memset(m_szBuff, 0, 1024);
	m_sSocket = INVALID_SOCKET;
	m_hEventBuff = NULL;
	memset(&m_sockAddr, 0, sizeof(SOCKADDR_IN));
	m_bRunning = false;

	InitializeCriticalSection(&m_csBuff);
	m_hEventBuff = CreateEvent(NULL, FALSE, FALSE, NULL);
}


CSocketComm::~CSocketComm()
{
	DeleteCriticalSection(&m_csBuff);
	if (NULL != m_hEventBuff)
	{
		CloseHandle(m_hEventBuff);
	}

	if (INVALID_SOCKET != m_sSocket)
	{
		shutdown(m_sSocket, SB_BOTH);
		closesocket(m_sSocket);
	}
}

unsigned __stdcall CSocketComm::RecvDataThreadProc(void* lpParam)
{
	CSocketComm *pThis = (CSocketComm *)lpParam;
	pThis->m_bRunning = true;
	int nLen = 0;
	while (true)
	{
		EnterCriticalSection(&pThis->m_csBuff);
		memset(pThis->m_szBuff, 0, 1024);
		nLen = recv(pThis->m_sSocket, pThis->m_szBuff, 1024, 0);
		if (nLen <= 0)
		{
			int nErr = WSAGetLastError();
			EnterCriticalSection(&g_csout);
			cout << "recv failed with:" << nErr << endl;
			LeaveCriticalSection(&g_csout);
			if (WSAECONNRESET == nErr)
			{
				EnterCriticalSection(&g_csout);
				cout << pThis->m_sSocket << "一个现有的连接被远程主机强制关闭" << endl;
				LeaveCriticalSection(&g_csout);
				closesocket(pThis->m_sSocket);
				pThis->m_bRunning = false;
				break;
			}
		}

		LeaveCriticalSection(&pThis->m_csBuff);
		SetEvent(pThis->m_hEventBuff);
	}
	return 0;
}

unsigned __stdcall CSocketComm::WorkingThreadProc(void* lpParam)
{
	CSocketComm *pThis = (CSocketComm *)lpParam;
	while (true)
	{
		EnterCriticalSection(&pThis->m_csBuff);
		DWORD nRet = WaitForSingleObject(pThis->m_hEventBuff, INFINITE);		
		if (WAIT_OBJECT_0 == nRet)
		{
			if (false == pThis->m_bRunning)
			{
				break;
			}

			Msg msg;
			memcpy(&msg, pThis->m_szBuff, sizeof(Msg));
			if (1 == msg.nEvent)
			{
				EnterCriticalSection(&g_csout);
				cout << "*****************" << endl;
				cout << "socket: " << pThis->m_sSocket << endl;
				cout << msg.szName << endl;
				cout << msg.nAge << endl;
				cout << msg.chSex << endl;
				cout << msg.szAddr << endl;
				cout << msg.dWages << endl << endl << endl;
				LeaveCriticalSection(&g_csout);
			}
		}
		LeaveCriticalSection(&pThis->m_csBuff);
		ResetEvent(pThis->m_hEventBuff);
	}
	return 0;
}

//UTF-8到GB2312的转换
char* CSocketComm::U2G(const char* utf8)
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
char* CSocketComm::G2U(const char* gb2312)
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

void CSocketComm::Run(SOCKET sSocket, SOCKADDR_IN *sockAddr)
{
	if (NULL != m_hEventBuff)
	{
		m_sSocket = sSocket;
		memcpy(&m_sockAddr, sockAddr, sizeof(SOCKADDR_IN));

		HANDLE hRecvDataThread = (HANDLE)_beginthreadex(NULL, 0, RecvDataThreadProc, this, 0, NULL);
		HANDLE hWorkingThread = (HANDLE)_beginthreadex(NULL, 0, WorkingThreadProc, this, 0, NULL);
		CloseHandle(hRecvDataThread);
		CloseHandle(hWorkingThread);
	}
}