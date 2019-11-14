

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
//����windows ��̬��
//#pragma comment(lib,"ws2_32.lib")
enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT_RESULT,
	CMD_LOGINOUT,
	CMD_ERROR
};

//�������ݰ� 
struct dataHead {
	//���ݳ���
	short dataLength;
	short cmd;
};

struct LoginResult :public dataHead {
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;

};
struct Loginout :public dataHead {
	Loginout()
	{
		dataLength = sizeof(Loginout);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};

struct LoginOutResult :public  dataHead {
	LoginOutResult()
	{
		dataLength = sizeof(LoginOutResult);
		cmd = CMD_LOGINOUT_RESULT;
		result = 0;
	}
	int result;

};

//��¼���ݽṹ
struct Login :public dataHead {
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char password[32];
};


//����ṹ������
struct DataPackage
{

	int age;
	char name[32];
};

int main()
{
	//����windows socket
	WORD word = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(word, &data);
	// 1������һ��socket
	// ��һ ipv����Э��  �ڶ� ʹ���ֽ�����ʽ ����  ʹ��tcp��ʽ
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 2��  bind ��������տͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	//����Э��
	_sin.sin_family = AF_INET;
	//����˿�
	_sin.sin_port = htons(7000);// 7000;
	//��ip��ַ  ip��ַ INADDR_ANY  ���޶�
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;

	if (SOCKET_ERROR == bind(sock, (sockaddr*)&_sin, sizeof(_sin)))
	{

		printf("�󶨶˿�ʧ�ܡ� \n");
	}
	else {
		printf("�󶨶˿ڳɹ��� \n");
	}
	// 3��listen ��������˿�
	//����socket  ��������Ϊ5

	if (SOCKET_ERROR == listen(sock, 5))
	{
		printf("��������˿�ʧ�ܡ�\n");
	}
	else {
		printf("��������˿ڳɹ���\n");
	}

	// 4�� accept �ȴ����ܿͻ�������
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _clientSock = INVALID_SOCKET;
	_clientSock = accept(sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _clientSock)
	{
		printf("���յ���Ч�Ŀͻ��� socket.....\n");
	}

	printf("�¿ͻ��˼��롣socket = %d,ip��ַ��=%s \n", (int)_clientSock, inet_ntoa(clientAddr.sin_addr));

	//��������

	char _cMsgBuf[128] = {};
	while (true) {


		// 5 ���տͻ�����������
		//�ֽڻ�����
		char szRecv[4096] = {};
		int _cLen = recv(_clientSock, szRecv, sizeof(dataHead), 0);
		dataHead *header = (dataHead*)szRecv;
		if (_cLen <= 0)
		{
			printf("�ͻ������˳������ӽ��ա�\n");
			break;
		}
		//6 �� ��������
		printf("�յ����� ��%d,���ݳ��� %d \n", header->cmd, header->dataLength);

		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login *login = (Login*)szRecv;
			recv(_clientSock, szRecv + sizeof(dataHead), header->dataLength - sizeof(dataHead), 0);
			printf("�յ����� ���ݳ��� :%d ,userName=%s\n", login->dataLength, login->userName);

			//�˺�������֤
			LoginResult ret;
			//send(_clientSock, (char*)&header, sizeof(header), 0);
			send(_clientSock, (char*)&ret, sizeof(ret), 0);

		}
		break;
		case CMD_LOGINOUT: {
			Loginout *lout = (Loginout*)szRecv;
			recv(_clientSock, szRecv + sizeof(dataHead), header->dataLength - sizeof(dataHead), 0);
			printf("�յ����� ���ݳ��� :%d ,userName=%s\n", lout->dataLength, lout->userName);

			//�˺�������֤
			LoginResult ret;
			send(_clientSock, (char*)&ret, sizeof(ret), 0);
		}
						   break;
		default: {
			dataHead hd_Len = { 0,CMD_ERROR };
			send(_clientSock, (char*)&hd_Len, sizeof(dataHead), 0);

		}
				 break;

		}

		// 7��send ��ͻ��˷���һ������

	}
	// 8���ر�socket
	closesocket(sock);
	printf("���˳� ��������� \n");
	getchar();
	//windows socket �ر�
	WSACleanup();
	return 0;
}