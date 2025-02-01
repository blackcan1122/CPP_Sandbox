#include <iostream>

#include <winsock2.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

int main(int args, char* argv[])
{
    WSADATA wsa;
    SOCKET server, client;
    struct sockaddr_in server_addr, client_addr;
    int client_size = sizeof(client_addr);

    WSAStartup(MAKEWORD(2, 2), &wsa);
    server = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);

    bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server, 1);

    while (true)
    {
        client = accept(server, (struct sockaddr*)&client_addr, &client_size);
        char buffer[256] = "Hello, Client!";

        char clientIP[16];  // Buffer to hold the IP address
        int clientPort = ntohs(client_addr.sin_port);  // Client port number

        strcpy(clientIP, inet_ntoa(client_addr.sin_addr));

        std::cout << "Found Client With IP: " << clientIP << " and Port: " << clientPort << std::endl;

        send(client, buffer, sizeof(buffer), 0);
    }



    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}