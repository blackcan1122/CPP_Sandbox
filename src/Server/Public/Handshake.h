#pragma once
#include <string>

struct HandshakeData
{
	std::string ClientName = "";
	std::string ApprovalToken = "";
};

class HandshakeObject
{
public:
	HandshakeObject(std::string Handshake);
	HandshakeObject();
	~HandshakeObject() = default;


	HandshakeData ParseHandshake(std::string Handshake);
	bool IsTokenCorrect(std::string ExpectedToken) const;
	std::string GetClientName() { return ParsedHandshake.ClientName; };
	std::string GetClientToken() { return ParsedHandshake.ApprovalToken; };

private:

	std::string OriginalHandshake;
	HandshakeData ParsedHandshake;

};

