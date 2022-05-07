#include <stdio.h>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


using namespace std;

bool isIPAddress(char * string) {
	
	int index = 0;
	char temp= string[index];

	while (temp != '\0')
	{
		index += 1;
		if (temp != '.' && ((int)temp > 57 || (int)temp < 48))
		{
			return false;
		}
		temp = string[index];
	}

	return true;
}

SOCKADDR_IN resolveDomain(char* input, SOCKADDR_IN &addr) {

	addrinfo* info;

	int ret = getaddrinfo(input, "http", NULL, &info);

	if (ret != 0)
	{
		printf("Phan giai ten mien that bai.");
	}

	addrinfo* temp = info->ai_next;
	while (temp->ai_next != NULL) {
		if (temp->ai_family == 2)
			break;
		temp = temp->ai_next;
	}

	memcpy(&addr, temp->ai_addr, temp->ai_addrlen);
	addr.sin_family = AF_INET;

	return addr;

}


int  main(int argc, char* argv[]) {

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	char* input1 = argv[1];
	char* input2 = argv[2];
	
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;

	int portNumber = stoi(input2);
	addr.sin_port = htons(portNumber);

	if (isIPAddress(input1))
		addr.sin_addr.s_addr = inet_addr(input1);
	
	else
		addr = resolveDomain(input1, addr);
	
	system("pause");

	int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));

	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		printf("Ket noi khong thanh cong - %d\n", ret);
		return 1;
	}

	char buff[256];
	ret = recv(client, buff, sizeof(buff), 0);
	buff[ret] = 0;
	printf("Server: %s\n", buff);

	memset(&buff[0], 0, sizeof(buff));

	while (true)
	{
		printf("Enter data to send: ");
		fgets(buff, sizeof(buff), stdin);

		int length = strcspn(buff, "\0");
		
		if (strncmp(buff, "stop", 4)==0)
		{
			break;
		}

		send(client, buff, length+1, 0);
	}


	closesocket(client);
	WSACleanup();
}














