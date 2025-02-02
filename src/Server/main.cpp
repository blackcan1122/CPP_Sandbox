#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define TCP_SERVER_PORT 12345
#define IP "127.0.0.1"

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        std::cerr << "Error getting hostname\n";
        return -1;
    }


    addrinfo hints = {};
    hints.ai_family = AF_INET;  // IPv4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* result = nullptr;
    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        std::cerr << "Error getting address info\n";
        return -1;
    }

    std::string ipAddress;
    char ipBuffer[INET_ADDRSTRLEN] = { 0 };

    for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        sockaddr_in* sockaddr_ipv4 = (sockaddr_in*)ptr->ai_addr;
        inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipBuffer, sizeof(ipBuffer));

        std::string currentIP = ipBuffer;

        // Ignore WSL/Hyper-V addresses (typically 172.x.x.x)
        if (currentIP.rfind("172.", 0) == 0) {
            continue;
        }

        // Prioritize LAN addresses (192.168.x.x or 10.x.x.x)
        if (currentIP.rfind("192.168.", 0) == 0 || currentIP.rfind("10.", 0) == 0) {
            ipAddress = currentIP;
            break;  // Stop searching once a LAN IP is found
        }
    }

    // If no LAN IP was found, use the first available IP
    if (ipAddress.empty() && result) {
        sockaddr_in* sockaddr_ipv4 = (sockaddr_in*)result->ai_addr;
        inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipBuffer, sizeof(ipBuffer));
        ipAddress = ipBuffer;
    }

    // Final check before using the IP
    if (ipAddress.empty()) {
        std::cerr << "No valid IP address found!" << std::endl;
    }
    else {
        std::cout << "Selected IP: " << ipAddress << std::endl;
    }

    freeaddrinfo(result);


    // === Step 1: Start TCP Server ===
    SOCKET tcpServer = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = getaddrinfo();  // Bind to all interfaces
    
    inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);
    
    serverAddr.sin_port = htons(TCP_SERVER_PORT);

    bind(tcpServer, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(tcpServer, SOMAXCONN);
    std::cout << "TCP Server listening on port " << TCP_SERVER_PORT << "...\n";

    while(true)
    {
        if (accept(tcpServer, (sockaddr*)&serverAddr, 0))
        {
            std::cout << "Found Something yaaaay" << std::endl;
        }
    }


    closesocket(tcpServer);
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