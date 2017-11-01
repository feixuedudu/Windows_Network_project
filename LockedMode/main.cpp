#include "../Common/common.h"
#include <iostream>
#include <map>
#include "SocketComm.h"
using namespace std;

extern CRITICAL_SECTION g_csout;

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
	//serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
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
	map<SOCKET, CSocketComm*> SocketManager;

	InitializeCriticalSection(&g_csout);

	while (true)
	{
		sClient = INVALID_SOCKET;
		memset(&clientAddr, 0, sizeof(clientAddr));
		if (INVALID_SOCKET == (sClient = accept(sListen, (sockaddr*)&clientAddr, &nAddrLen)))
		{
			//EnterCriticalSection(&g_csout);
			cout << "accept() failed with error " << WSAGetLastError() << endl;
			//LeaveCriticalSection(&g_csout);
			closesocket(sListen);
			WSACleanup();
			return -1;
		}

		CSocketComm *pSocketComm = new CSocketComm();
		if (NULL != pSocketComm)
		{
			pSocketComm->Run(sClient, &clientAddr);
			SocketManager[sClient] = pSocketComm;
		}		
	}

	for (auto &pair : SocketManager)
	{
		if (NULL != pair.second)
		{
			delete pair.second;
		}
	}

	DeleteCriticalSection(&g_csout);
	system("pause");
	closesocket(sListen);
	WSACleanup();
	return 0;
}