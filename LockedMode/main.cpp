#include "../Common/common.h"
#include <iostream>
using namespace std;

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
	serverAddr.sin_port = htonl(SERVER_PORT);

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
	while (true)
	{
		if (INVALID_SOCKET == (sClient = accept(sListen, (sockaddr*)&clientAddr, &nAddrLen)))
		{
			cout << "accept() failed with error " << WSAGetLastError() << endl;
			closesocket(sListen);
			WSACleanup();
			return -1;
		}
	}

	system("pause");
	return 0;
}