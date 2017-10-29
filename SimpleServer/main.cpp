#include "../Common/common.h"
#include <iostream>
using namespace std;


DWORD WINAPI SocketProc(LPVOID lpThreadParameter);

struct Msg
{
	char szName[20];
	int nAge;
	char szAddr[20];
	char chSex;
	double dWages;
};

int main(int argc, char *argv[])
{
	char szTemp[1024] = { 0 };
	Student stu = { "��־��", 23, "hubeihuangshiyangxin", "m", 34555.45 };
	string strJson = stu.encode();
	char *pTemp = G2U(strJson.c_str());
	memcpy(szTemp, pTemp, strlen(pTemp));

	WSAData wsd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsd))
	{
		cout << "WSAStartup() failed..." << endl;
		return -1;
	}

	SOCKET sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket() failed with:" << WSAGetLastError() << endl;
		return -1;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons(APP_PORT);

	if (SOCKET_ERROR == bind(sServer, (sockaddr*)&serverAddr, sizeof(sockaddr_in)))
	{
		int nErr = WSAGetLastError();
		cout << "bind() faile with: " << nErr << endl;
		if (WSAEACCES == nErr)
		{
			cout << "��ͼͨ������Ȩ�޷���һ���׽���" << endl;
		}
		else if (WSAEADDRINUSE == nErr)
		{
			cout << "�˿ڱ�ռ�ã���ʹ�������˿�����" << endl;
		}
		else if (WSAEADDRNOTAVAIL == nErr)
		{
			cout << "���粻���ã���������" << endl;
		}
		else
		{
			cout << "bind() faile ..." << endl;
		}
		closesocket(sServer);
		shutdown(sServer, SD_BOTH);
		WSACleanup();
		system("pause");
		return -1;
	}

	if (SOCKET_ERROR == listen(sServer, 4))
	{
		cout << "listen() faile with: " << WSAGetLastError() << endl;
		return -1;
	}

	sockaddr_in clientAddr;
	int nLen = sizeof(sockaddr_in);
	SOCKET sClient = NULL;
	char szIp[20] = { 0 };
	while (true)
	{
		sClient = NULL;
		memset(&clientAddr, 0, sizeof(sockaddr_in));
		memset(szIp, 0, 20);
		//accept����������������˲��᷵�ش���
		sClient = accept(sServer, (sockaddr*)&clientAddr, &nLen);
		if (INVALID_SOCKET == sClient)
		{
			cout << "accept() faile with: " << WSAGetLastError() << endl;
			return -1;
		}
		strcpy(szIp, inet_ntoa(clientAddr.sin_addr));
		cout << szIp << "���ӳɹ�" << endl; 
		CreateThread(NULL, 0, SocketProc, (LPVOID)sClient, 0, NULL);
	}

	closesocket(sServer);
	shutdown(sServer, SD_BOTH);
	WSACleanup();
	system("pause");
	return 0;
}

DWORD WINAPI SocketProc(LPVOID lpThreadParameter)
{
	SOCKET sClient = (SOCKET)lpThreadParameter;

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
				cout << "һ�����е����ӱ�Զ������ǿ�ƹر�" << endl;
				closesocket(sClient);
				break;
			}
		}
		else
		{
			Msg msg; 
			memcpy(&msg, szBuf, sizeof(Msg));
			char *p = U2G(szBuf);
			cout << sClient << "says: " << p << endl;
			char szTemp[1024] = {0};
			Student stu ;
			string strJson = stu.encode();
			char *pTemp = G2U(strJson.c_str());
			memcpy(szTemp, pTemp, strlen(pTemp));
			send(sClient, szTemp, 1024, 0);
		}
	}

	return 0;
}