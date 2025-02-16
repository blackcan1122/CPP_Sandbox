#include "ServerInstance.h"

int main()
{
	std::shared_ptr<ServerInstance> CurrentServer = std::make_shared<ServerInstance>();

	CurrentServer->InitServer();
	
}