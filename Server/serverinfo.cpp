#include <stdio.h>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

int main(int arg, char* argv[])
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN addr;
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int val = stoi(argv[1]);
    addr.sin_port = htons(val);

    bind(server, (SOCKADDR*)&addr, sizeof(addr));

    listen(server, 5);

    printf("Bat dau nhan tin nhan!!\n");

    SOCKET client = accept(server, NULL, NULL);

    char buffer[3000];
    int ret = 0;

    while (1)
    {
        ret = recv(client, buffer, sizeof(buffer), 0);

        printf("Du lieu tu client:\n%s\n", buffer);
    }

    closesocket(client);
    closesocket(server);
    WSACleanup();

}