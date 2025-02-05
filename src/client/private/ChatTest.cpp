#include "ChatTest.h"
#include "TextBox.h"
#include "Button.h"
#include <thread>
#include <future>
#include "EventDispatcher.hpp"
#include "Event.hpp"
#include "openssl/sha.h"
#include <iomanip>

void printHashClient(const unsigned char* hash, size_t length) {
	for (size_t i = 0; i < length; i++) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	std::cout << std::endl;
}

ChatTest::ChatTest()
{
	IPInput = std::make_shared<TextInputBox>();
	PortInput = std::make_shared<TextInputBox>();
	ChatWindow = std::make_shared<TextInputBox>();

	ConnectButton = std::make_shared<Button>();
	ButtonDispatcher = std::make_shared<EventDispatcher>();


	std::string Connect = "Connect";
	int TextLength = MeasureText(Connect.c_str(), 16);
	ConnectButton->Construct(GetScreenWidth() - 20 - TextLength, 50, 50, 50, "Connect", WHITE, true, 0.2f)
		.SetEventDispatcher(ButtonDispatcher)
		.SetEventPayload("Connect")
		.OnHover([this](Button* Button)
			{
				Button->UpdateColor(GRAY);
			})
		.OnHoverLeave([this](Button* Button)
			{
				Button->UpdateColor(WHITE);
			});



	IPInput->Construct(10, 100, 400, 50, WHITE).CanBeEdited(true)
		.UpdateFontColor(BLACK).SetInitialText(ServerIP)
		.ChangeMaxChars(15)
		.UpdateFontSize(19);
	PortInput->Construct(430, 100, 150, 50, WHITE)
		.CanBeEdited(true)
		.UpdateFontColor(BLACK).SetInitialText("49152")
		.ChangeMaxChars(6).UpdateFontSize(19);
	ChatWindow->Construct(20, 175 , GetScreenWidth() - 40, 400, WHITE)
		.CanBeEdited(true)
		.UpdateFontColor(BLACK)
		.SetInitialText("")
		.ChangeMaxChars(254)
		.UpdateFontSize(19);

	ButtonDispatcher->AddListener("UIEvent", [this](std::shared_ptr<Event> Event)
		{
			closesocket(TCPSocket); // Ensure previous socket (if any) is closed.
			TCPSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (TCPSocket == INVALID_SOCKET) {
				std::cerr << "Failed to create socket." << std::endl;
				return;
			}

			ServerIp = IPInput->StringToHold;
			Port = std::stoi(PortInput->StringToHold);

			if (!inet_pton(AF_INET, ServerIp.c_str(), &ServerAdress.sin_addr))
			{
				std::cerr << "Invalid IP Adress" << std::endl;
			}

			ServerAdress.sin_port = htons(Port);
			std::thread(&ChatTest::ConnectWithTimeout, this, TCPSocket, ServerAdress, 5000).detach();
		});

	WSADATA Wsa;
	WSAStartup(MAKEWORD(2, 2), &Wsa);

	TCPSocket = socket(AF_INET, SOCK_STREAM, 0);
	ServerAdress = {};
	ServerAdress.sin_family = AF_INET;

	u_long SocketMode = 1;
	ioctlsocket(TCPSocket, FIONBIO, &SocketMode);
}

void ChatTest::Update()
{
	ClearBackground(BLACK);

	IPInput->Update();
	PortInput->Update();
	ChatWindow->Update();
	ConnectButton->Update();

	if (ChatWindow->bIsFocused(GetMousePosition()) && IsKeyPressed(257) && ChatWindow->StringToHold != "")
	{
		send(TCPSocket, ChatWindow->StringToHold, sizeof(ChatWindow->StringToHold), 0);
		ChatWindow->EreaseText();
	}

	if (bIsConnected)
	{
		timeval TimeOutForSelect = { 0, 0 }; // 500 ms timeout
	
			FD_ZERO(&ConnectedServerFD);
			FD_SET(TCPSocket, &ConnectedServerFD);
			bHasSetFSET = true;


		int ClientsToDo = select(0, &ConnectedServerFD, NULL, NULL, &TimeOutForSelect);
		if (ClientsToDo == SOCKET_ERROR)
		{
			std::cerr << "select() failed with error: " << WSAGetLastError() << std::endl;
		}

		if (FD_ISSET(TCPSocket, &ConnectedServerFD))
		{
			char Buffer[512];
			int BytesReceived = recv(TCPSocket, Buffer, sizeof(Buffer) - 1, 0);
			if (BytesReceived > 0)
			{
				Buffer[BytesReceived] = '\0';
				ChatWindow->SetInitialText(Buffer);
			}
		}
	}
	



}

bool ChatTest::TryConnect(SOCKET Socket, sockaddr_in Adress)
{
	int ReturnConnect = connect(Socket, (sockaddr*)&Adress, sizeof(Adress));
	std::string Handshake = "Name:Bibi;Token:63ee7f365450f9586dbb31c9b59db63817797e04ea1014dd5a8ac6615d44fac1;";
	send(Socket, Handshake.c_str(), Handshake.size(), 0);
	
	if (ReturnConnect != -1)
	{
		std::cout << "Good Connection" << std::endl;
		return true;
	}
	closesocket(TCPSocket);
	return false;
}

bool ChatTest::ConnectWithTimeout(SOCKET Socket, sockaddr_in ServerAdress, int Time)
{
	auto future = std::async(std::launch::async, &ChatTest::TryConnect, this, Socket, ServerAdress);

	if (future.wait_for(std::chrono::milliseconds(Time)) == std::future_status::ready) {
		if (future.get()) 
		{
			std::cout << "Connected to Server" << std::endl;
			bIsConnected = true;
			return true;
		}
		else {
			std::cout << "Failed to Connect to Server" << std::endl;
			bIsConnected = false;
			return false;
		}
	}
	else {
		std::cout << "Timeout to Server" << std::endl;
		bIsConnected = false;
		return false;
	}
}
