#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

//void ProcessHeaders(char* req)
//{
//    char* p1 = strstr(req, "\r\n");
//    char* p2 = strstr(req, "\r\n\r\n");
//    int headerLength = p2 - (p1 + 2);
//    int requestURILeangth = p1 - req;
//    char* requestURI = (char*)malloc(requestURILeangth+1);
//    char* headers = (char*)malloc(headerLength + 1);
//    memcpy(requestURI, req, requestURILeangth);
//    memcpy(headers, p1 + 2, headerLength);
//    headers[headerLength] = 0;
//
//    printf("Headers:\n%s\n", headers);
//
//    char* header = strtok(headers, "\r\n");
//    while (header != NULL)
//    {
//        char* p = strstr(header, ": ");
//        // Tach phan name
//        int nameLength = p - header;
//        char* name = (char*)malloc(nameLength + 1);
//        memcpy(name, header, nameLength);
//        name[nameLength] = 0;
//        // Tach phan value
//        int valueLength = strlen(header) - nameLength - 2;
//        char* value = (char*)malloc(valueLength + 1);
//        memcpy(value, header + nameLength + 2, valueLength);
//        value[valueLength] = 0;
//        printf("Name: %s --- Value: %s\n", name, value);
//
//        header = strtok(NULL, "\r\n");
//    }
//}

void ProcessURI(char* req, int client) {
    char* p1 = strstr(req, "\r\n");
    int requestURILeangth = p1 - req;
    char* requestURI = (char*)malloc(requestURILeangth);
    memcpy(requestURI, req, requestURILeangth);
    requestURI[requestURILeangth] = 0;

    printf("Request URI:\n%s\n", requestURI);

    int params[10] = { 0 }; int i = 0;
    char* op = (char*)malloc(4);
    char sign = ' ';
    char* queryString = strstr(requestURI, "?");
    char* param = strtok(queryString+1, "&");

    while (param != NULL)
    {
        if (strncmp(param, "op", 2)==0)
        {
            memcpy(op, param + 3, 4);
        }
        else
        {
            char * temp = strstr(param, "=");
            char* value = temp+1;
            params[i] = atoi(value);
            i += 1;
        }
        param = strtok(NULL, "&");
    }
    
    if(i<2){
        const char* errorMessage = "Not enough param";
        send(client, errorMessage, strlen(errorMessage), 0);
        
    }else{
    
        int operand = 0;

        if (strcmp(op,"add")==0)
        {
            operand = params[0] + params[1];
            sign = '+';
        }
        else if(strcmp(op, "sub") == 0)
        {
            operand = params[0] - params[1];
            sign = '-';
        }
        else if (strcmp(op, "div") == 0)
        { 
            operand = params[0] / params[1];
            sign = '/';
        }
        else if (strcmp(op, "mul") == 0)
        {
            operand = params[0] * params[1];
            sign = '*';
        }
        else
        {
            const char* errorMessage = "Invalid op code";
            send(client, errorMessage, strlen(errorMessage), 0);
            return ;
        }

        char response[256];
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>%d %c %d = %d</h1></body></html>", params[0], sign, params[1], operand);
        send(client, response, strlen(response), 0);
        return;
    }
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);
    char buf[2048];
    int ret;
    while (1)
    {
        // Chap nhan ket noi
        SOCKET client = accept(listener, NULL, NULL);
        // Nhan yeu cau
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            closesocket(client);
            continue;
        }
        // Xu ly yeu cau
        if (ret < sizeof(buf))
            buf[ret] = 0;
        printf("%s\n", buf);
        ProcessURI(buf, client);
        // Tra lai ket qua
        // Dong ket noi
        closesocket(client);
    }
}
