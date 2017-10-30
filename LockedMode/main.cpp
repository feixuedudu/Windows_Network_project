#include "../Common/common.h"
#include <iostream>
#include <map>
using namespace std;

DWORD WINAPI RecvDataThreadProc(LPVOID lpParam);
DWORD WINAPI WorkingThreadProc(LPVOID lpParam);

struct SOCKET_THREAD
{
	SOCKET sSocket;
	HANDLE hThread;
	SOCKADDR_IN addr;
};

struct Msg
{
	int nEvent;
	char szName[20];
	int nAge;
	char szAddr[20];
	char chSex;
	double dWages;
};

struct DataPackage
{
	SOCKET sSocket;
	int nSize;
	char szBuff[1024];
};

int main(int argc, char *argv[])
{
	WSADATA wsd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsd))
	{
		cout << "WSAStartup() failed with error " << WSAGetLastError() << endl;
		return -1;
	}

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sListen)
	{
		cout << "socket() failed with error " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons(SERVER_PORT);

	if (SOCKET_ERROR == bind(sListen, (const sockaddr *)&serverAddr, sizeof(SOCKADDR_IN)))
	{
		cout << "bind() failed with error " << WSAGetLastError() << endl;
		closesocket(sListen);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(sListen, 4))
	{
		cout << "listen() failed with error " << WSAGetLastError() << endl;
		closesocket(sListen);
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN clientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	SOCKET sClient = INVALID_SOCKET;
	HANDLE hThread = NULL;
	map<SOCKET, SOCKET_THREAD*> clientInfos;
	while (true)
	{
		sClient = INVALID_SOCKET;
		hThread = NULL;
		memset(&clientAddr, 0, sizeof(clientAddr));
		if (INVALID_SOCKET == (sClient = accept(sListen, (sockaddr*)&clientAddr, &nAddrLen)))
		{
			cout << "accept() failed with error " << WSAGetLastError() << endl;
			closesocket(sListen);
			WSACleanup();
			return -1;
		}

		hThread = CreateThread(NULL, 0, RecvDataThreadProc, (LPVOID)sClient, 0, NULL);

		SOCKET_THREAD* pClientInfo = new SOCKET_THREAD();
		pClientInfo->sSocket = sClient;
		pClientInfo->hThread = hThread;
		pClientInfo->addr = clientAddr;
		clientInfos[sClient] = pClientInfo;
	}

	system("pause");
	return 0;
}

DWORD WINAPI RecvDataThreadProc(LPVOID lpParam)
{
	SOCKET sClient = (SOCKET)lpParam;

	int nLen = 0;
	char szBuf[1024] = { 0 };
	while (true)
	{
		memset(szBuf, 0, 1024);
		nLen = recv(sClient, szBuf, 1024, 0);
		if (nLen <= 0)
		{
			int nErr = WSAGetLastError();
			cout << "recv failed with:" << nErr << endl;
			if (WSAECONNRESET == nErr)
			{
				cout << "一个现有的连接被远程主机强制关闭" << endl;
				closesocket(sClient);
				break;
			}
		}
		else
		{
			/*Msg msg;
			memcpy(&msg, szBuf, sizeof(Msg));
			char *p = U2G(szBuf);
			cout << sClient << "says: " << p << endl;*/
			DataPackage msg;
			memset(&msg, 0, sizeof(DataPackage));
			msg.nSize = nLen;
			msg.sSocket = sClient;
			memcpy(msg.szBuff, szBuf, nLen);
			HANDLE hThread = CreateThread(NULL, 0, WorkingThreadProc, (LPVOID)&msg, 0, NULL);
			CloseHandle(hThread);
		}
	}

	return 0;
}

DWORD WINAPI WorkingThreadProc(LPVOID lpParam)
{
	DataPackage data = *(DataPackage*)lpParam;
	Msg msg;
	memcpy(&msg, data.szBuff, sizeof(Msg));
	if (1 == msg.nEvent)
	{
		cout << "*****************" << endl;
		cout << "socket:" << data.sSocket << endl;
		cout << msg.szName << endl;
		cout << msg.nAge << endl;
		cout << msg.chSex << endl;
		cout << msg.szAddr << endl;
		cout << msg.dWages << endl << endl << endl;
	}


	return 0;
}