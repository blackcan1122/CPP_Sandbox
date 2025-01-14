/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "TimeCalculator.h"
#include <string>
#include "WindowHandle.h"
#include "TextBox.h"
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <variant>
#include <functional>
#include <cctype>
#include "trex.h"
#include "Logo.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define MAX_INPUT_CHARS     4



int main (int args, char* argv[])
{
	bool isDebug = false;

	if (args > 1 && strcmp(argv[1], "-debug") == 0)
	{
		isDebug = true;
	}

	// Init Window Properties
	WindowHandle CurrentWindow;

	CurrentWindow.ActiveWindow.MaxFPS = 30;
	CurrentWindow.ActiveWindow.WindowDimensions = IntVector2(600, 600);

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT | FLAG_WINDOW_RESIZABLE);

	// Create the window and OpenGL context
	InitWindow(CurrentWindow.ActiveWindow.WindowDimensions.X, CurrentWindow.ActiveWindow.WindowDimensions.Y, "Time Calc");
	SetTargetFPS(CurrentWindow.ActiveWindow.MaxFPS);

	SetWindowMaxSize(900, 900);
	SetWindowMinSize(550, 550);

	Image image = LoadImageFromMemory(".jpg", trex_jpg, trex_jpg_len);     // Loaded in CPU memory (RAM)
	Image imageLogo = LoadImageFromMemory(".png", Logo_png, Logo_png_len);     // Loaded in CPU memory (RAM)
	Texture2D texture = LoadTextureFromImage(image);          // Image converted to texture, GPU memory (VRAM)
	
	SetWindowIcon(imageLogo);
	UnloadImage(imageLogo);
	UnloadImage(image);   // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM

	TimeCalculator ActiveTimeCalc;
	ActiveTimeCalc.SetDebugMode(isDebug);

	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();


		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);
		DrawTexture(texture, GetScreenWidth() / 2 - texture.width / 2, GetScreenHeight() / 2 - texture.height / 2, DARKGRAY);

		ActiveTimeCalc.UpdateCalculator();



		
		DrawText("Kommen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 200, 18, GREEN);
		DrawText("Pausen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 260, 18, GREEN);
		DrawText("Korrektur Zeit HH:MM :", GetScreenWidth() / 2.0f - 223, 320, 18, GREEN);
		DrawText("Restliche Qual:", GetScreenWidth() / 2.0f - 150, 380, 18, GREEN);


		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}


	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
