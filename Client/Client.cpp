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
        if (array[i] == '\0')
        {
            array[i] = ' ';
        }
    }
    return array;
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
    char listOfDisk[26 * 4];

    DWORD size = sizeof(name);
    DWORD nBufferLength = 100;
    DWORD sectorsPerCluster = 0;
    DWORD bytesPerSector = 0;
    DWORD numberOfFreeClusters = 0;
    DWORD totalNumberOfClusters = 0;

    GetComputerNameA(name, &size);
    GetLogicalDriveStringsA(sizeof(listOfDisk), listOfDisk);
    GetDiskFreeSpaceA("C:/", &sectorsPerCluster, &bytesPerSector,
        &numberOfFreeClusters, &totalNumberOfClusters);

    replaceNull(listOfDisk);

    char buff[256];
    snprintf(buff, sizeof(buff), 
        "- Name of Computer: %s\n"
        "- List of disk: %s\n"
        "- Number of free Clusters: %lu\n"
        "- Total number of Clusters: %lu\n"
        , name, listOfDisk, numberOfFreeClusters, totalNumberOfClusters);

    printf("Start send message to server!!\n");

    while (1)
    {
        ret = send(client, buff, sizeof(buff), 0);
        if (ret <= 0)
            break;
        printf("Nhap thong tin muon gui: ");
        fgets(buff, sizeof(buff), stdin);

        if (strncmp(buff, "exit", 4) == 0) break;

    }

    closesocket(client);
    WSACleanup();
}
