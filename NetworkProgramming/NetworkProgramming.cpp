#include <stdio.h>
#include <winsock2.h>

#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


using namespace std;

void removeDupWord(char str[])
{
    // Returns first token 
    char* token = strtok(str, " ");

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }
}

// Driver code
int main()
{
    char str[] = "Geeks for Geeks";
    removeDupWord(str);
    return 0;
}