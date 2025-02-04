#include "Handshake.h"

HandshakeObject::HandshakeObject(std::string Handshake)
{
	OriginalHandshake = Handshake;
	ParsedHandshake = ParseHandshake(Handshake);
}

HandshakeObject::HandshakeObject()
{

}

HandshakeData HandshakeObject::ParseHandshake(std::string Handshake)
{
    std::string ApprovalToken, ClientName;
    std::string delimiter = ";";

    // Key to Value Pair for InitialHandshake
    size_t pos = 0;
    while ((pos = Handshake.find(delimiter)) != std::string::npos)
    {
        std::string token = Handshake.substr(0, pos);
        size_t separator_pos = token.find(":");
        std::string key = token.substr(0, separator_pos);
        std::string value = token.substr(separator_pos + 1);

        if (key == "Token")
        {
            ApprovalToken = value;
        }
        else if (key == "Name")
        {
            ClientName = value;
        }

        Handshake.erase(0, pos + delimiter.length());
    }

    HandshakeData TempObject;
    TempObject.ClientName = ClientName;
    TempObject.ApprovalToken = ApprovalToken;

    ParsedHandshake.ApprovalToken = ApprovalToken;
    ParsedHandshake.ClientName = ClientName;

	return TempObject;
}

bool HandshakeObject::IsTokenCorrect(std::string ExpectedToken) const
{
    return ExpectedToken == ParsedHandshake.ApprovalToken;
}
