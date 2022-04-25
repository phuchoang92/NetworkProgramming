#include <stdio.h>
#include <string>
#include <format>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

char* getDriveInfor(char* array, int size) {

    string temp;
    char buffer[100];
    int cx = 0, last = 0;
    
    DWORD bytesPerSector = 0;
    DWORD sectorsPerCluster = 0;
    DWORD totalNumOfClusters = 0;
    float amountSpaceOfDrivers = 0;

    for (int i = 0; i < size; i++) {

        GetDiskFreeSpaceA(&(array[i*4]), &sectorsPerCluster, &bytesPerSector, NULL, &totalNumOfClusters);
        amountSpaceOfDrivers = (float)totalNumOfClusters * (float)bytesPerSector * (float)sectorsPerCluster / (float)(1 << 30);
        cx = snprintf(buffer + last, 100, " %s %.2f GB\n", &(array[i * 4]), amountSpaceOfDrivers);
        last += cx;

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
    DWORD size = sizeof(name);
    GetComputerNameA(name, &size);

    char listOfDisk[256];
    DWORD len = GetLogicalDriveStringsA(sizeof(listOfDisk), listOfDisk);
    int number = len / 4;
    char* diskInfor = getDriveInfor(listOfDisk,number);
   
    char buff[256];
    snprintf(buff, sizeof(buff), 
        "- Name of Computer: %s\n"
        "- List of disk:\n%s"
        , name, diskInfor);

    printf("Start send message to server!!\n");

    ret = send(client, buff, sizeof(buff), 0);

    while (1){

        ret = recv(client, buff, sizeof(buff), 0);
        if (ret <= 0)
            break;

        printf("Du lieu gui tu server: %s", buff);

    }

    closesocket(client);
    WSACleanup();
}
