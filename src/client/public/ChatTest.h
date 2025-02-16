#pragma once
#include "Core.h"
#include "GameMode.h"
#include <deque>


#if defined(_WIN32)           
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


#if defined(_WIN32)           // raylib uses these names as function parameters
//#undef near
//#undef far
#endif


#define TCP_SERVER_PORT 49152
#define ServerIP "216.201.76.178"

class ChatTest : public GameMode
{
public:
	~ChatTest();
	ChatTest();

	virtual void Update() override;


	SOCKET TCPSocket;
	sockaddr_in ServerAdress;
	fd_set ConnectedServerFD;

	Rectangle ChatArea;

	std::shared_ptr<TextInputBox> IPInput = nullptr;
	std::shared_ptr<TextInputBox> PortInput = nullptr;
	std::shared_ptr<TextInputBox> UserNameInput = nullptr;

	std::shared_ptr<TextInputBox> ChatWindow = nullptr;
	std::shared_ptr<Button> ConnectButton = nullptr;
	std::shared_ptr<EventDispatcher> ButtonDispatcher;

	std::deque<std::shared_ptr<TextInputBox>> ChatHistory;
	int Port;
	std::string ServerIp;

	bool bIsConnected = false;
	bool bHasSetFSET = false;

private:

	std::string UserName = "";
	bool TryConnect(SOCKET Socket, sockaddr_in Adress);
	bool ConnectWithTimeout(SOCKET Socket, sockaddr_in ServerAdress, int Time);
	bool SetUpWSA();
	bool TeardownWSA();

	int CountNewlines(const std::string MessageToCountLines);
};

