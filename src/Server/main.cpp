#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>


#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 12345
#define MAX_CLIENTS 64

int main() 
{
    // Initialize Winsock
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) 
    {
        std::cerr << "WSAStartup failed!" << std::endl;
        return 1;
    }


    SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocket == INVALID_SOCKET) 
    {
        std::cerr << "Failed to create socket!" << std::endl;
        WSACleanup();
        return 1;
    }


    u_long Mode = 1;
    ioctlsocket(ServerSocket, FIONBIO, &Mode);

    // Bind the socket
    sockaddr_in ServerAddress = {};
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(SERVER_PORT);
    if (bind(ServerSocket, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR) 
    {
        std::cerr << "Bind failed!" << std::endl;
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    // Start listening
    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) 
    {
        std::cerr << "Listen failed!" << std::endl;
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    std::vector<SOCKET> ConnectedClients;

    while (true) 
    {
        fd_set ClientFD;
        FD_ZERO(&ClientFD);
        FD_SET(ServerSocket, &ClientFD);

        for (SOCKET Client : ConnectedClients) 
        {
            FD_SET(Client, &ClientFD);
        }

        timeval Timeout = { 0, 500000 }; // 500 ms timeout
        int ClientsToDo = select(0, &ClientFD, NULL, NULL, &Timeout);
        if (ClientsToDo == SOCKET_ERROR) {
            std::cerr << "Select failed!" << std::endl;
            break;
        }

        if (FD_ISSET(ServerSocket, &ClientFD)) 
        {
            sockaddr_in ClientAddress;
            int ClientSize = sizeof(ClientAddress);
            SOCKET ClientSocket = accept(ServerSocket, (sockaddr*)&ClientAddress, &ClientSize);
            if (ClientSocket != INVALID_SOCKET) 
            {
                u_long nonBlocking = 1;
                ioctlsocket(ClientSocket, FIONBIO, &nonBlocking);
                ConnectedClients.push_back(ClientSocket);
                std::cout << "New client connected!" << std::endl;
            }
        }

        for (auto ConnectedClient = ConnectedClients.begin(); ConnectedClient != ConnectedClients.end(); ) 
        {
            SOCKET client = *ConnectedClient;
            if (FD_ISSET(client, &ClientFD)) 
            {
                char Buffer[512];
                int BytesReceived = recv(client, Buffer, sizeof(Buffer) - 1, 0);
                if (BytesReceived > 0) 
                {
                    Buffer[BytesReceived] = '\0';
                    std::cout << "Client: " << Buffer << std::endl;
                }
                else if (BytesReceived == 0 || BytesReceived == SOCKET_ERROR) 
                {
                    std::cout << "Client disconnected!" << std::endl;
                    closesocket(client);
                    ConnectedClient = ConnectedClients.erase(ConnectedClient);
                    continue;
                }
            }

            ++ConnectedClient;
        }
    }

    for (SOCKET ConnectedClient : ConnectedClients) {
        closesocket(ConnectedClient);
    }
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}


/*
* 
    // === Step 2: Start Multicast Listener ===
    SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in multicastAddr = {};
    multicastAddr.sin_family = AF_INET;
    multicastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    multicastAddr.sin_port = htons(MULTICAST_PORT);
    
    bind(udpSock, (sockaddr*)&multicastAddr, sizeof(multicastAddr));

    ip_mreq mreq = {};
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = INADDR_ANY;
    setsockopt(udpSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));

    char buffer[256];
    sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);

    while (true) {
        int bytesReceived = recvfrom(udpSock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Received discovery request from: " << inet_ntoa(clientAddr.sin_addr) << std::endl;

            // Respond with TCP server IP

            char serverIp[INET_ADDRSTRLEN];
            struct sockaddr_in sa;
            sa.sin_family = AF_INET;
            sa.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
            inet_ntop(AF_INET, &sa.sin_addr, serverIp, sizeof(serverIp));

            std::string response = "SERVER_IP: " + std::string(getLocalIP());
            sendto(udpSock, response.c_str(), response.length(), 0, (sockaddr*)&clientAddr, clientLen);
        }
        
        // Accept TCP connections
        SOCKET clientSocket = accept(tcpServer, NULL, NULL);
        if (clientSocket != INVALID_SOCKET) {
            std::cout << "Client connected!\n";
            send(clientSocket, "Hello from Server!", 18, 0);
            std::string Message = "Hey Baby Gurl";
            closesocket(clientSocket);
        }
    }

    closesocket(udpSock);
*/