#include "../Common/common.h"
#include <iostream>
using namespace std;

DWORD WINAPI SocketProc(LPVOID lpThreadParameter);

// tcp keepalive结构体  
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

	// 启用tcp keepalive机制  
#if 1  
	// 设置SO_KEEPALIVE  
	int iKeepAlive = 1;
	int iOptLen = sizeof(iKeepAlive);
	setsockopt(sClient, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, iOptLen);

	TCP_KEEPALIVE inKeepAlive = { 0, 0, 0 };
	unsigned long ulInLen = sizeof(TCP_KEEPALIVE);
	TCP_KEEPALIVE outKeepAlive = { 0, 0, 0 };
	unsigned long ulOutLen = sizeof(TCP_KEEPALIVE);
	unsigned long ulBytesReturn = 0;

	// 设置心跳参数  
	inKeepAlive.onoff = 1;                  // 是否启用  
	inKeepAlive.keepalivetime = 1000;       // 在tcp通道空闲1000毫秒后， 开始发送心跳包检测  
	inKeepAlive.keepaliveinterval = 500;    // 心跳包的间隔时间是500毫秒  

	/*
	补充上面的"设置心跳参数"：
	当没有接收到服务器反馈后，对于不同的Windows版本，客户端的心跳尝试次数是不同的，
	比如， 对于Win XP/2003而言, 最大尝试次数是5次， 其它的Windows版本也各不相同。
	当然啦， 如果是在Linux上， 那么这个最大尝试此时其实是可以在程序中设置的。
	*/


	// 调用接口， 启用心跳机制  
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
		char szBuf2[1024] = "曹志坚";
		char szBuf3[1024] = "zjian.曹志坚@qq.com";

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
		//		cout << "一个现有的连接被远程主机强制关闭" << endl;
		//		/*closesocket(sClient);
		//		WSACleanup();
		//		break;*/
		//	}
		//	else if (WSAETIMEDOUT == WSAGetLastError())
		//	{
		//		cout << "连接尝试失败是因为连接的一方在一段时间后没有适当的响应，或者已经建立的连接失败，因为连接的主机没有响应" << endl;
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