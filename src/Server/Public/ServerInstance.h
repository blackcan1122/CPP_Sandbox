#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <openssl/ssl.h>
#include <openssl/err.h>

// own
#include "Client.h"


#pragma comment(lib, "ws2_32.lib")


class ServerInstance
{
public:
	ServerInstance() = default;
	~ServerInstance() = default;
	int InitServer();

	void UpdateLoop();
	void ProcessClientSockets();

	int InitWinsock();
	int SetSocketOption();
	int BindServerSocket();
	void ConfigureSelectTimeout();

	int SendMessageToClient();


private:

	SOCKET ServerSocket;
	sockaddr_in ServerAddress;
	std::shared_ptr<WSADATA> Winsock = std::make_shared<WSADATA>();
	std::vector<Client> ConnectedClients;
	fd_set ActiveClientFD;
	timeval TimeOutForSelect;
	u_long SocketMode = 1;
	int ServerPort = 49152;

	bool m_ServerRunning = true;

};

