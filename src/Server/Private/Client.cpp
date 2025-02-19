#include "Client.h"

Client::Client()
{
}

Client::Client(SOCKET IncomingSocket, sockaddr_in ClientAddress)
{
    m_ClientSocket = IncomingSocket;
    m_ClientAddress = ClientAddress;
}

Client::Client(SOCKET IncomingSocket)
{
    m_ClientSocket = IncomingSocket;
}

int Client::SendData(const std::string& Data)
{
    return send(m_ClientSocket, Data.c_str(), Data.size(), 0);
}

int Client::ReceiveData(std::string& Data)
{
    const int BufferSize = 16384;
    char TempBuffer[BufferSize];
    
    int ReturnValue = recv(m_ClientSocket, TempBuffer, BufferSize - 1, 0);
    if (ReturnValue > 0)
    {
        TempBuffer[ReturnValue] = '\0';
        Data = std::string(TempBuffer);
    }
    else
    {
        if (ReturnValue == 0)
        {
            std::cerr << "Connection closed by client." << std::endl;
        }
    }

    return ReturnValue;
}

void Client::CloseConnection()
{
    std::cout << "Client with ID: " << m_ID << " and Name: " << m_ClientName << " Is Closed" << std::endl;
    closesocket(m_ClientSocket);
    m_ClientSocket = INVALID_SOCKET;
}

bool Client::VerifyPeerCertificate()
{
    if (SSL_get_verify_result(m_SSLConnection) == X509_V_OK)
    {
        std::cout << "Certificate verified successfully." << std::endl;
        return true;
    }
    else 
    {
        std::cerr << "Certificate verification failed." << std::endl;
        return false;
    }
}

// Should only be called once on Connecting a Client
bool Client::PerformInitialHandshake()
{
    std::string HandShakeMessage;
    ReceiveData(HandShakeMessage);

    ClientHandshake->ParseHandshake(HandShakeMessage);

    if (ClientHandshake->IsTokenCorrect(ExpectedToken) == false)
    {     
        return false;
    }
    m_ClientName = ClientHandshake->GetClientName();
    m_bIsAuthenticated = true;
    m_bWasGreeted = true;

    return true;
}
