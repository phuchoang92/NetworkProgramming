#include <stdio.h>
#include <string>
#include <format>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

char* replaceNull(char* array) {
    for (int i = 0; i < sizeof(array); i++)
    {
        if (array[i]=='\0') array[i] = ' ';
        
    }

    int cx = 0;
    char buffer[100];
    char* token = strtok(array," ");
    
    while (token != NULL)
    {
        DWORD lpBytesPerSector = 0;
        DWORD lpSectorsPerCluster = 0;
        DWORD totalNumberOfClusters = 0;
        DWORD amountOfKBytesOfComputer = 0;
        GetDiskFreeSpaceA(token, &lpSectorsPerCluster, &lpBytesPerSector, NULL, &totalNumberOfClusters);
        amountOfKBytesOfComputer = lpBytesPerSector * lpSectorsPerCluster * (totalNumberOfClusters/1024/1024);
        cx = snprintf(buffer+cx, 100 - cx, " %s %lu MB\n", token, amountOfKBytesOfComputer);

        token = strtok(NULL, " ");
    }
    return buffer;
}

int main(int argc, char* argv[])
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    
    addrinfo* info;

    int ret = getaddrinfo(argv[1], "http", NULL, &info);
    if (ret != 0) {
        printf("Phan giai ten mien that bai.");
    }

    addrinfo* temp = info->ai_next;
    while (temp->ai_next != NULL) {
        if (temp->ai_family == 2)
            break;
        temp = temp->ai_next;
    }

    int portNumber = stoi(argv[2]);

    SOCKADDR_IN addr;
    memcpy(&addr, temp->ai_addr, temp->ai_addrlen);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);

    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));

    if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        printf("Ket noi khong thanh cong - %d\n", ret);
        return 1;
    }

    char name[MAX_COMPUTERNAME_LENGTH + 1];
    char listOfDisk[40];

    DWORD size = sizeof(name);

    GetComputerNameA(name, &size);
    GetLogicalDriveStringsA(sizeof(listOfDisk), listOfDisk);
    char* diskInfor = replaceNull(listOfDisk);

    char buff[256];
    snprintf(buff, sizeof(buff), 
        "- Name of Computer: %s\n"
        "- List of disk:\n%s"
        , name, diskInfor);

    printf("Start send message to server!!\n");

    ret = send(client, buff, sizeof(buff), 0);

    while (1)
    {

        ret = recv(client, buff, sizeof(buff), 0);
        if (ret <= 0)
            break;

        printf("Du lieu gui tu server: %s", buff);

    }

    closesocket(client);
    WSACleanup();
}
