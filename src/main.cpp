/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "GameInstance.h"

// utility header for SearchAndSetResourceDir




int main (int args, char* argv[])
{
	bool isDebug = false;

	if (args > 1 && strcmp(argv[1], "-debug") == 0)
	{
		isDebug = true;
	}

	GameInstance::InitGameInstance(600, 600, 30, false, isDebug);
	GameInstance::GetInstance();
	


	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
