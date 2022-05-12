
#include <stdio.h>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

SOCKET clients[64];
int numberClients = 0;

DWORD WINAPI threadClient(LPVOID Param) {

	SOCKET client = *(SOCKET*) Param;

	int ret;
	char word[20];
	char buffer[256];
	char clientName[30];
	const char* mess = "Wrong syntax! Reenter message please!\n";

	do {

		ret = recv(client, buffer, sizeof(buffer), 0);

		if (ret <= 0)
		{
			break;
		}
		sscanf(buffer, "%s", word);

		if (strncmp(word, "client_id:", 9) == 0) break;

		send(client, mess, 38, 0);

	} while (1);

	buffer[ret-1] = 0;

	strncpy(clientName ,& buffer[10], 30);

	printf("Name: %s\n", clientName);

	while (true){

		char recvMess[256];
		ret = recv(client, recvMess, sizeof(recvMess), 0);

		sprintf(buffer, "%s: %s\n", clientName, recvMess);

		int lengthName = strcspn(buffer, "\n");
		
		for (int i = 0; i < numberClients; i++)
		{
			if (clients[i] != client)
			{
				send(clients[i], buffer, lengthName+1, 0);
			}
		}
	}
	closesocket(client);
	numberClients--;
}


int main() {

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(server, (SOCKADDR *)&addr, sizeof(addr));

	listen(server, 5);

	while (1) {
		clients[numberClients] = accept(server, NULL, NULL);
		printf("Client moi ket noi: %d\n", clients[numberClients]);
		CreateThread(0, 0, threadClient, &clients[numberClients], 0, 0);
		numberClients++;
	}

}

