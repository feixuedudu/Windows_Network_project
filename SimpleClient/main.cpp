#include "../Common/common.h"
#include <iostream>
using namespace std;

DWORD WINAPI SocketProc(LPVOID lpThreadParameter);

// tcp keepalive�ṹ��  
typedef struct tcp_keepalive
{
	u_long onoff;
	u_long keepalivetime;
	u_long keepaliveinterval;
}TCP_KEEPALIVE;

#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)  

int main(int argc, char *argv[])
{
	WSADATA wsd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsd))
	{
		cout << "WSAStartup() failed..." << endl;
		return -1;
	}

	SOCKET sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sClient)
	{
		cout << "socket() failed with:" << WSAGetLastError() << endl;
		return -1;
	}

	// ����tcp keepalive����  
#if 1  
	// ����SO_KEEPALIVE  
	int iKeepAlive = 1;
	int iOptLen = sizeof(iKeepAlive);
	setsockopt(sClient, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, iOptLen);

	TCP_KEEPALIVE inKeepAlive = { 0, 0, 0 };
	unsigned long ulInLen = sizeof(TCP_KEEPALIVE);
	TCP_KEEPALIVE outKeepAlive = { 0, 0, 0 };
	unsigned long ulOutLen = sizeof(TCP_KEEPALIVE);
	unsigned long ulBytesReturn = 0;

	// ������������  
	inKeepAlive.onoff = 1;                  // �Ƿ�����  
	inKeepAlive.keepalivetime = 1000;       // ��tcpͨ������1000����� ��ʼ�������������  
	inKeepAlive.keepaliveinterval = 500;    // �������ļ��ʱ����500����  

	/*
	���������"������������"��
	��û�н��յ������������󣬶��ڲ�ͬ��Windows�汾���ͻ��˵��������Դ����ǲ�ͬ�ģ�
	���磬 ����Win XP/2003����, ����Դ�����5�Σ� ������Windows�汾Ҳ������ͬ��
	��Ȼ���� �������Linux�ϣ� ��ô�������Դ�ʱ��ʵ�ǿ����ڳ��������õġ�
	*/


	// ���ýӿڣ� ������������  
	WSAIoctl(sClient, SIO_KEEPALIVE_VALS,
		&inKeepAlive, ulInLen,
		&outKeepAlive, ulOutLen,
		&ulBytesReturn, NULL, NULL);
#endif  

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons(SERVER_PORT);

	if (SOCKET_ERROR == connect(sClient, (sockaddr*)&serverAddr, sizeof(sockaddr_in)))
	{
		int nErr = WSAGetLastError();
		cout << "connect() failed with:" << nErr << endl;
		return -1;
	}

	CreateThread(NULL, 0, SocketProc, (LPVOID)sClient, 0, NULL);

	while (true);

	shutdown(sClient, SD_BOTH);
	closesocket(sClient);	
	WSACleanup();
	system("pause");
	return 0;
}

DWORD WINAPI SocketProc(LPVOID lpThreadParameter)
{
	SOCKET sClient = (SOCKET)lpThreadParameter;

	char szBuf[1024] = { 0 };
	int nLen = 0;
	while (true)
	{
		Student stu;
		string strJson = stu.encode();
		send(sClient, G2U(strJson.c_str()), 1024, 0);

		memset(szBuf, 0, 1024);
		recv(sClient, szBuf, 1024, 0);
		char *p = U2G(szBuf);
		cout << "recv: " << p << endl;
		Student stu1;
		stu1.decode(p);
		cout << stu1.strName << " " << stu1.nAge << " " << stu1.strSex << " " << stu1.strAddr << " " << stu1.dWages << endl;

		/*char szBuf1[1024] = "caozhijian";
		char szBuf2[1024] = "��־��";
		char szBuf3[1024] = "zjian.��־��@qq.com";

		char *p = G2U(szBuf1);
		send(sClient, p, 1024, 0);
		memset(szBuf, 0, 1024);
		recv(sClient, szBuf, 1024, 0);		
		cout << "recv: " << szBuf << endl;

		p = G2U(szBuf2);
		send(sClient, p, 1024, 0);
		memset(szBuf, 0, 1024);
		recv(sClient, szBuf, 1024, 0);
		cout << "recv: " << szBuf << endl;

		p = G2U(szBuf3);
		send(sClient, p, 1024, 0);
		memset(szBuf, 0, 1024);
		recv(sClient, szBuf, 1024, 0);
		cout << "recv: " << szBuf << endl;*/

		//memset(szBuf, 0, 1024);
		//nLen = recv(sClient, szBuf, 1024, 0);
		//if (nLen <= 0)
		//{
		//	cout << "recv failed with:" << WSAGetLastError() << endl;
		//	if (WSAECONNRESET == WSAGetLastError())
		//	{
		//		cout << "һ�����е����ӱ�Զ������ǿ�ƹر�" << endl;
		//		/*closesocket(sClient);
		//		WSACleanup();
		//		break;*/
		//	}
		//	else if (WSAETIMEDOUT == WSAGetLastError())
		//	{
		//		cout << "���ӳ���ʧ������Ϊ���ӵ�һ����һ��ʱ���û���ʵ�����Ӧ�������Ѿ�����������ʧ�ܣ���Ϊ���ӵ�����û����Ӧ" << endl;
		//	}
		//	
		//}
		//else
		//{
		//	cout << sClient << "says: " << szBuf << endl;
		//}
	}

	return 0;
}