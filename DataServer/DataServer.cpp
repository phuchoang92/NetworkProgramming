#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

SOCKET clients[64];
int numberClients = 0;
const char* path = "C:/Users/Phuc/Desktop/csdl.txt";
const char* pathOut = "C:/Users/Phuc/Desktop/out.txt";
const char* error = "Account and Password do not exist! Try again!\n";
const char* success = "Log in successfully\n";

void replaceSlash(char * string) {
	for (int i = 0; i < strlen(string); i++)
	{
		if (string[i] == (char)47)
		{
			string[i] = (char)92;
		}
	}
	
}

DWORD WINAPI threadClient(LPVOID Param) {

	SOCKET client = *(SOCKET*)Param;

	char buffer[1000];
	char account[20];
	char password[20];
	bool hasAcount = FALSE;

	char* line;
	size_t len = 0;
	

	int ret = 0;

	while (!hasAcount)
	{
		ret = recv(client, buffer, sizeof(buffer), 0);

		sscanf(buffer, "%s %s", account, password);

		FILE* f = fopen(path, "r");

		while (fgets(buffer, 256, f)) {
			char acc[20];
			char pass[20];

			sscanf(buffer, "%s %s", acc, pass);

			if (strcmp(acc, account)==0 && strcmp(pass, password)==0)
			{
				hasAcount = TRUE;
				break;
			}
		}

		if (!hasAcount)
		{
			send(client, error, strlen(error), 0);
		}
		fclose(f);
	}

	send(client, success, strlen(success), 0);

	ret = recv(client, buffer, sizeof(buffer), 0);

	buffer[ret - 1] = ' ';

	sprintf(buffer + ret, "> %s", pathOut);

	replaceSlash(buffer);

	system(buffer);

	FILE* fileOut = fopen(pathOut, "r");

	ret = fread(buffer, 1, sizeof(buffer), fileOut);

	send(client, buffer, ret, 0);
	
	closesocket(client);
	numberClients--;

	return 0;
}


int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(server, (SOCKADDR*)&addr, sizeof(addr));

	listen(server, 5);

	while (1) {
		clients[numberClients] = accept(server, NULL, NULL);
		printf("Client moi ket noi: %d\n", clients[numberClients]);
		CreateThread(0, 0, threadClient, &clients[numberClients], 0, 0);
		numberClients++;
	}
}