#include <stdio.h>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

int main(int argc, char * argv[]) 
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int portNumber = stoi(argv[1]);
	char* helloFile = argv[2];
	char* clientInforFile = argv[3];

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port	= htons(portNumber);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(server, (SOCKADDR*)&addr,sizeof(addr));

	listen(server, 5);

	printf("Start to receive message!!\n");

	SOCKET client = accept(server, NULL, NULL);

	char buffer[256];

	FILE* file1 = fopen(helloFile, "r");
	
	int ret = fread(buffer, 1, sizeof(buffer), file1);
	send(client, buffer, ret, 0);
	fclose(file1);

	FILE* file2 = fopen(clientInforFile, "w");

	while (true)
	{
		ret = recv(client, buffer, sizeof(buffer), 0);
	
		if (ret <= 0)
			break;

		printf("Message received: %s", buffer);

		fwrite(buffer, 1, ret, file2);
	}

	
	fclose(file2);

	closesocket(client);
	closesocket(server);
	WSACleanup();

}