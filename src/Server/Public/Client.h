#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "Handshake.h"



#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
	Client();
	Client(SOCKET IncomingSocket, sockaddr_in ClientAddress);
	Client(SOCKET IncomingSocket);
	~Client() = default;

	// Send data over the secure connection (or plain socket if not using SSL)
	int SendData(const std::string& data);

	// Receive data from the secure connection
	int ReceiveData(std::string& data);

	// Shutdown and clean up the connection
	void CloseConnection();

	// Certificate verification (if needed)
	bool VerifyPeerCertificate();

	bool PerformInitialHandshake();


	// Getter and Setter
	inline SOCKET GetSocket() { return m_ClientSocket; }
	inline void SetAddress(sockaddr_in ClientAdress) { m_ClientAddress = ClientAdress; }
	void SetID(unsigned int id) { m_ID = id; }
	inline unsigned int GetID() const { return m_ID; }
	void SetClientName(const std::string& name) { m_ClientName = name; }
	inline const std::string& GetClientName() const { return m_ClientName; }

private:

	std::shared_ptr<HandshakeObject> ClientHandshake = std::make_shared<HandshakeObject>();

	// Temporary for testing
	std::string ExpectedToken = "63ee7f365450f9586dbb31c9b59db63817797e04ea1014dd5a8ac6615d44fac1";

	SOCKET m_ClientSocket = SOCKET();
	sockaddr_in m_ClientAddress = sockaddr_in();
	unsigned int m_ID = UINT8_MAX;
	std::string m_ClientName = "NOT INITIALIZED";

	SSL* m_SSLConnection;

	bool m_bWasGreeted = false;
	bool m_bIsAuthenticated = false;

};

