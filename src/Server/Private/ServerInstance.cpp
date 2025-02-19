#include "ServerInstance.h"
#include <sstream>

int ServerInstance::InitServer()
{
    // Initialize Winsock

    if (WSAStartup(MAKEWORD(2, 2), Winsock.get()) != 0)
    {
        std::cerr << "WSAStartup failed!" << std::endl;
        return 1;
    }


    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket!" << std::endl;
        WSACleanup();
        return 1;
    }


    SocketMode = 1;
    ioctlsocket(ServerSocket, FIONBIO, &SocketMode);

    // Bind the socket
    ServerAddress = {};
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(ServerPort);
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

    UpdateLoop();

    for (Client ConnectedClient : ConnectedClients) {
        ConnectedClient.CloseConnection();
    }
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}

void ServerInstance::UpdateLoop()
{
    while (m_ServerRunning)
    {
        ProcessClientSockets();
    }
}

void ServerInstance::ProcessClientSockets()
{
    FD_ZERO(&ActiveClientFD);
    FD_SET(ServerSocket, &ActiveClientFD);

    for (Client& Client : ConnectedClients)
    {
        FD_SET(Client.GetSocket(), &ActiveClientFD);
    }

    TimeOutForSelect = { 0, 500000 }; // 500 ms timeout
    int ClientsToDo = select(0, &ActiveClientFD, NULL, NULL, &TimeOutForSelect);
    if (ClientsToDo == SOCKET_ERROR)
    {
        std::cerr << "select() failed with error: " << WSAGetLastError() << std::endl;
        // Optionally, you can return here if you want to stop processing or retry
        return;
    }

    if (FD_ISSET(ServerSocket, &ActiveClientFD))
    {
        sockaddr_in ClientAddress;
        int ClientSize = sizeof(ClientAddress);
        Client ClientSocket = accept(ServerSocket, (sockaddr*)&ClientAddress, &ClientSize);
        ClientSocket.SetAddress(ClientAddress);
        if (ClientSocket.GetSocket() != INVALID_SOCKET)
        {
            if (!ClientSocket.PerformInitialHandshake())
            {
                closesocket(ClientSocket.GetSocket());
                std::cerr << "Client with IP: " << inet_ntoa(ClientAddress.sin_addr) << "failed at Handshake." << std::endl;
                return;
            }
            ioctlsocket(ClientSocket.GetSocket(), FIONBIO, &SocketMode);
            ConnectedClients.push_back(ClientSocket);
            std::cout << "Client With Name: " << ClientSocket.GetClientName() << "And IP: " << inet_ntoa(ClientAddress.sin_addr) << " Connected." << std::endl;
            for (auto CurrentClient : ConnectedClients)
            {
                if (CurrentClient.GetClientName() == ClientSocket.GetClientName())
                {
                    std::ostringstream WelcomeMessageStream;
                    WelcomeMessageStream << "Hey " << CurrentClient.GetClientName() << "\n" << "Aktuell sind folgende User Verbunden: \n";
                    for (auto CurrentClientStr : ConnectedClients)
                    {
                        if (CurrentClientStr.GetClientName() != ClientSocket.GetClientName())
                        {
                            WelcomeMessageStream << CurrentClientStr.GetClientName() << "\n";
                        }

                    }
                    CurrentClient.SendData(WelcomeMessageStream.str());
                }
                else
                {
                    std::string WelcomeMessage = ClientSocket.GetClientName() + " is now Online";
                    CurrentClient.SendData(WelcomeMessage);
                }

            }
        }
    }

    for (auto it = ConnectedClients.begin(); it != ConnectedClients.end(); /* no increment here */)
    {
        if (FD_ISSET(it->GetSocket(), &ActiveClientFD))
        {
            char Buffer[512];
            int BytesReceived = recv(it->GetSocket(), Buffer, sizeof(Buffer) - 1, 0);
            if (BytesReceived > 0)
            {
                Buffer[BytesReceived] = '\0'; // Ensure proper termination first!
                std::string Answer = it->GetClientName() + ": " + Buffer;
                for (auto& CurrentClient : ConnectedClients)
                {
                    send(CurrentClient.GetSocket(), Answer.c_str(), Answer.size(), 0);
                }
            }
            else if (BytesReceived == 0 || BytesReceived == SOCKET_ERROR)
            {
                std::cout << "Client disconnected!" << std::endl;
                std::string ClientName = it->GetClientName();
                it->CloseConnection();
                it = ConnectedClients.erase(it);  // Erase and get the next iterator
                for (auto& CurrentClients : ConnectedClients)
                {
                    CurrentClients.SendData("Server: " + ClientName + " Disconnected");
                }
                continue;  // Skip the increment and go to the next iteration
            }
        }

        ++it;
    }
}
