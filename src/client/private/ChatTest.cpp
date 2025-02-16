#include "ChatTest.h"
#include "TextBox.h"
#include "Button.h"
#include <thread>
#include <future>
#include "EventDispatcher.hpp"
#include "Event.hpp"
#include "openssl/sha.h"
#include <iomanip>
#include <deque>


void printHashClient(const unsigned char* hash, size_t length) {
	for (size_t i = 0; i < length; i++) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	std::cout << std::endl;
}

ChatTest::~ChatTest()
{
	TeardownWSA();
}

ChatTest::ChatTest()
{
	IPInput = std::make_shared<TextInputBox>();
	PortInput = std::make_shared<TextInputBox>();
	ChatWindow = std::make_shared<TextInputBox>();
	UserNameInput = std::make_shared<TextInputBox>();

	ConnectButton = std::make_shared<Button>();
	ButtonDispatcher = std::make_shared<EventDispatcher>();


	std::string Connect = "Connect";
	int TextLength = MeasureText(Connect.c_str(), 16);
	ConnectButton->Construct(GetScreenWidth() - 120 - TextLength, 65, 150, 20, "Connect", WHITE, true, 0.2f)
		.SetEventDispatcher(ButtonDispatcher)
		.SetEventPayload("Connect")
		.CenterText()
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
		.UpdateFontSize(19)
		.UseCenter();

	PortInput->Construct(430, 100, 150, 50, WHITE)
		.CanBeEdited(true)
		.UpdateFontColor(BLACK).SetInitialText("49152")
		.ChangeMaxChars(6).UpdateFontSize(19)
		.UseCenter();

	ChatWindow->Construct(20, GetScreenHeight()-45, GetScreenWidth() - 80, 40, BLACK)
		.CanBeEdited(true)
		.UpdateFontColor(GREEN)
		.SetInitialText("")
		.ChangeMaxChars(254)
		.UpdateFontSize(19)
		.UseWordWrap()
		.ResizeBoxToText();

	UserNameInput->Construct(100, 65, 150, 25, DARKGREEN)
		.CanBeEdited(true)
		.SetInitialText("")
		.ChangeMaxChars(12)
		.UpdateFontSize(19)
		.UseCenter();

	ButtonDispatcher->AddListener("UIEvent", [this](std::shared_ptr<Event> Event)
		{
			UserName = UserNameInput->StringToHold;
			if (UserName.empty())
			{
				std::cerr << "No Username Provided" << std::endl;
				return;

			}
			if (bIsConnected == false)
			{
				SetUpWSA();
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
			}
			else
			{
				TeardownWSA();
			}
		});



	ChatArea = { ChatWindow->Box.x, ChatWindow->Box.y - 375, ChatWindow->Box.width, 350 };
}

void ChatTest::Update()
{
	ClearBackground(Color{25,25,25,255});

	IPInput->Update();
	PortInput->Update();
	ChatWindow->UpdateTextPosition().Update();
	ConnectButton->Update();
	UserNameInput->Update();

	if (ChatWindow->bIsFocused(GetMousePosition()) && IsKeyPressed(257) && ChatWindow->StringToHold != "")
	{
		send(TCPSocket, ChatWindow->StringToHold, strlen(ChatWindow->StringToHold), 0);
		ChatWindow->EreaseText();
		ChatWindow->ResetSizeToInitial();
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

				std::string Message(Buffer);

				int WidthText = MeasureText(Message.c_str(), 19);
				
				int LineCount = CountNewlines(Message);

				std::shared_ptr<TextInputBox> NewMessage = std::make_shared<TextInputBox>();
				NewMessage->Construct(30, ChatWindow->Box.y, WidthText, 22 * LineCount, Color{60,60,60,128})
					.ChangeMaxChars(256)
					.UpdateFontColor(GREEN)
					.UpdateFontSize(19);

				strcpy(NewMessage->StringToHold, Message.c_str());

				ChatHistory.push_front(NewMessage);

				for (int i = 0; i < ChatHistory.size(); i++)
				{
					Vector2 NewPos;

					if (i == 0) // Latest message at the bottom
					{
						NewPos = { NewMessage->Box.x, ChatWindow->Box.y - 35 - (19 * LineCount) };
						std::cout << "The First Message in the Loop is: " << NewMessage->StringToHold << std::endl;
					}
					else // Stack older messages on top
					{
						int CurrentLineCount = CountNewlines(ChatHistory[i]->StringToHold);
						

						NewPos = { NewMessage->Box.x, ChatHistory[i - 1]->Box.y - (24 * CurrentLineCount) };
					}

					ChatHistory[i]->SetBoxPosition(NewPos);
				}
			
			}
		}
	}

	

	BeginScissorMode(ChatArea.x, ChatArea.y, ChatArea.width, ChatArea.height);

	if (CheckCollisionPointRec(GetMousePosition(), ChatArea) && GetMouseWheelMoveV() != Vector2{ 0,0 })
	{
		for (const auto& Chat : ChatHistory)
		{
			Chat->UpdateBoxPositionOffset(Vector2Scale(GetMouseWheelMoveV(), 10));
		}
	}

	for (const auto& Chat : ChatHistory)
	{
		Chat->UpdateTextPosition().Update();
	}
	

	EndScissorMode();

	


}

bool ChatTest::TryConnect(SOCKET Socket, sockaddr_in Adress)
{
	int ReturnConnect = connect(Socket, (sockaddr*)&Adress, sizeof(Adress));
	std::string HandshakeName = "Name:" + UserName;
	std::string HandshakeToken = "Token:63ee7f365450f9586dbb31c9b59db63817797e04ea1014dd5a8ac6615d44fac1;";
	std::string Handshake = HandshakeName + ";" + HandshakeToken;
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
			ConnectButton->UpdateText("Disconnect").CenterText();
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

bool ChatTest::SetUpWSA()
{
	WSADATA Wsa;
	if (WSAStartup(MAKEWORD(2, 2), &Wsa) != 0)
	{
		std::cerr << "Error in Initializing Windows Sockets: " << WSAGetLastError << std::endl;
		std::string ErrorMsg = "Error in Initializing Windows Sockets";
		ChatWindow->CanBeEdited(false);
		ChatWindow->SetInitialText(ErrorMsg.c_str());
		return false;
	}

	TCPSocket = socket(AF_INET, SOCK_STREAM, 0);
	ServerAdress = {};
	ServerAdress.sin_family = AF_INET;

	u_long SocketMode = 1;
	ioctlsocket(TCPSocket, FIONBIO, &SocketMode);
	return true;
}

bool ChatTest::TeardownWSA()
{
	bIsConnected = false;
	WSAEVENT WSAEvent = WSACreateEvent();

	if (WSAEventSelect(TCPSocket, WSAEvent, FD_CLOSE | FD_READ | FD_WRITE) == SOCKET_ERROR)
	{
		std::cerr << "WSAEventSelect failed: " << WSAGetLastError() << std::endl;
		WSACloseEvent(WSAEvent);
		return false;
	}
	// Initiate a shutdown
	if (shutdown(TCPSocket, SD_BOTH) == SOCKET_ERROR)
	{
		std::cerr << "Shutdown failed: " << WSAGetLastError() << std::endl;
		return false;
	}

	// Wait for the FD_CLOSE event with a timeout (e.g., 5 seconds)
	DWORD waitResult = WSAWaitForMultipleEvents(1, &WSAEvent, FALSE, 5000, FALSE);
	if (waitResult == WSA_WAIT_FAILED)
	{
		std::cerr << "WSAWaitForMultipleEvents failed: " << WSAGetLastError() << std::endl;
		return false;
	}
	else if (waitResult == WSA_WAIT_TIMEOUT)
	{
		std::cerr << "Timeout waiting for FD_CLOSE event" << std::endl;
		return false;
	}
	else
	{
		// Event was signaled; check for FD_CLOSE
		WSANETWORKEVENTS networkEvents;
		if (WSAEnumNetworkEvents(TCPSocket, WSAEvent, &networkEvents) == SOCKET_ERROR)
		{
			std::cerr << "WSAEnumNetworkEvents failed: " << WSAGetLastError() << std::endl;
			return false;
		}
		else if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			std::cout << "FD_CLOSE event received; remote side closed the connection" << std::endl;
		}
	}

	// Clean up: close the socket and event, then cleanup Winsock
	if (closesocket(TCPSocket) == SOCKET_ERROR)
	{
		std::cerr << "closesocket failed: " << WSAGetLastError() << std::endl;
		return false;
	}
	WSACloseEvent(WSAEvent);

	// Finally, call WSACleanup (ensure it's called the correct number of times relative to WSAStartup)
	if (WSACleanup() == SOCKET_ERROR)
	{
		std::cerr << "WSACleanup failed: " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "Connection Successfully closed" << std::endl;
	ConnectButton->UpdateText("Connect").CenterText();
	return true;
}

int ChatTest::CountNewlines(const std::string MessageToCountLines)
{
	int LineCount = 1; // Starting with 1 since we always have atleast 1 line
	size_t pos = 0;

	while ((pos = MessageToCountLines.find('\n', pos)) != std::string::npos)
	{
		LineCount++;
		pos++;
	}

	return LineCount;
}
