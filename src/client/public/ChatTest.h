#pragma once
#include "Core.h"
#include "GameMode.h"


#if defined(_WIN32)           
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif


#define TCP_SERVER_PORT 12345
#define ServerIP "172.23.144.1"

class ChatTest : public GameMode
{
public:
	~ChatTest() {  };
	ChatTest();

	virtual void Update() override;


	SOCKET TCPSocket;
	sockaddr_in ServerAdress;

	std::shared_ptr<TextInputBox> IPInput = nullptr;
	std::shared_ptr<TextInputBox> PortInput = nullptr;

	std::shared_ptr<TextInputBox> ChatWindow = nullptr;
	std::shared_ptr<Button> ConnectButton = nullptr;
	std::shared_ptr<EventDispatcher> ButtonDispatcher;

	int Port;
	std::string ServerIp;

	bool bIsConnected = false;

private:
	bool TryConnect(SOCKET Socket, sockaddr_in Adress);
	bool ConnectWithTimeout(SOCKET Socket, sockaddr_in ServerAdress, int Time);
};

