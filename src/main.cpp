/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
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

struct TimeFormatDecimal
{
	double Hour = 0;
	double Minutes = 0;
	double Combined = 0;

	double& operator [](int Number)
	{
		switch (Number % 3)
		{
		case 0:
			return Hour;
			break;

		case 1:
			return Minutes;
			break;
		case 2:
			return Combined;
			break;
		default:
			break;
		}
	}

	void CombinedTimeToComponents()
	{
		if (Combined > 0.0)
		{
			Hour = floor(Combined);
			Minutes = Combined - floor(Combined);
		}
		else
		{
			Hour = ceil(Combined);
			Minutes = Combined - ceil(Combined);
		}

	}

};


struct TimeFormat
{
	int Hour = 0;
	int Minutes = 0;
	TimeFormatDecimal DecimalTime;

	int& operator [](int Number)
	{
		switch (Number % 2)
		{
		case 0:
			return Hour;
			break;

		case 1:
			return Minutes;
			break;

		default:
			break;
		}
	}

	void ConvertToDecimalTime()
	{
		DecimalTime[0] = Hour;
		DecimalTime[1] = (1.0 * Minutes) / 60;
		DecimalTime[2] = DecimalTime[0] + DecimalTime[1];
	}

	void ConvertToTimeFormat()
	{
		Hour = static_cast<int>(DecimalTime[0]);
		Minutes = static_cast<int>((round((DecimalTime[1]) * 60)));
	}

	bool ConvertStringToTimeFormat(char* String)
	{
		if (String == nullptr)
		{
			return false;
		}

		if (String[2] != ':')
		{
			return false;
		}

		if (strlen(String) > 5)
		{
			return false;
		}
		
		// Copying String, as Loop does destruct the string
		char* TempChar = String;

		while (*TempChar != '\0')
		{  // Loop until null terminator
			if (*TempChar == ':')
			{
				++TempChar;
				continue;
			}
			if (!std::isdigit(*TempChar))
			{
				return false;  // If any char is not a digit, return false
			}
			++TempChar;  // Move to next character
		}

		if ((String[3] - '0') > 6 || (String[0] - '0') > 2)
		{
			return false;
		}

		Hour = (10 * (String[0] - '0') + (String[1] - '0'));
		Minutes = (10 * (String[3] - '0') + (String[4] - '0'));
		return true;

	}
};

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



	TextInputBox ArriveTimeBox;
	ArriveTimeBox.Construct(GetScreenWidth() / 2.0f, 180, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("07:30")
		.UpdateFontColor(DARKGRAY);

	TextInputBox DebugTimeBox;
	DebugTimeBox.Construct(GetScreenWidth() / 2.0f, 125, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("15:30")
		.UpdateFontColor(DARKGRAY);


	TextInputBox PauseTimeBox;
	PauseTimeBox.Construct(GetScreenWidth() / 2.0f, 240, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("00:30")
		.UpdateFontColor(DARKGRAY);

	TextInputBox CorrectionTimeBox;
	CorrectionTimeBox.Construct(GetScreenWidth() / 2.0f, 300, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("00:00")
		.UpdateFontColor(DARKGRAY);

	TextInputBox OutputTime;
	OutputTime.Construct(GetScreenWidth() / 2.0f, 300+60, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(false)
		.SetInitialText("Output")
		.UpdateFontSize(22)
		.UpdateFontColor(DARKGRAY);


	TimeFormat CorrectionTime;
	TimeFormat PauseTime;
	TimeFormat StartTime;
	TimeFormat CurrentTime;
	TimeFormat DifferenceTime;
	TimeFormat Konto1Delta;
	TimeFormat Konto2Delta;

	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// Gathering Some Crucial Infos:
		time_t placeholderTime = time(NULL);
		struct tm* localTime = localtime(&placeholderTime);

		if (isDebug)
		{
			CurrentTime.ConvertStringToTimeFormat(DebugTimeBox.StringToHold);
			CurrentTime.ConvertToDecimalTime();
		}
		else
		{
			CurrentTime[0] = localTime->tm_hour;
			CurrentTime[1] = localTime->tm_min;
			CurrentTime.ConvertToDecimalTime();
		}
		
		StartTime.ConvertStringToTimeFormat(ArriveTimeBox.StringToHold);
		StartTime.ConvertToDecimalTime();

		
		PauseTime.ConvertStringToTimeFormat(PauseTimeBox.StringToHold);
		PauseTime.ConvertToDecimalTime();

		
		CorrectionTime.ConvertStringToTimeFormat(CorrectionTimeBox.StringToHold);
		CorrectionTime.ConvertToDecimalTime();

		double workedTime = CurrentTime.DecimalTime.Combined - StartTime.DecimalTime.Combined;
		double totalPause = PauseTime.DecimalTime.Combined - CorrectionTime.DecimalTime.Combined;
		double remaining = std::fmod(8.0 - (workedTime - totalPause), 24.0);

	



		double Temp = std::fmod(8.0 - ((CurrentTime.DecimalTime.Combined - StartTime.DecimalTime.Combined) - (PauseTime.DecimalTime.Combined - CorrectionTime.DecimalTime.Combined)), 24.0);

		DifferenceTime.DecimalTime.Combined = remaining;
		DifferenceTime.DecimalTime.CombinedTimeToComponents();
		DifferenceTime.ConvertToTimeFormat();

		std::string OutputText = std::to_string(DifferenceTime.Hour) + ":" + std::to_string(DifferenceTime.Minutes);
		OutputTime.UpdateTextShown(OutputText);

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		DrawTexture(texture, GetScreenWidth() / 2 - texture.width / 2, GetScreenHeight() / 2 - texture.height / 2, DARKGRAY);

		
		DrawText("Kommen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 200, 18, GREEN);
		DrawText("Pausen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 260, 18, GREEN);
		DrawText("Korrektur Zeit HH:MM :", GetScreenWidth() / 2.0f - 223, 320, 18, GREEN);
		DrawText("Restliche Qual:", GetScreenWidth() / 2.0f - 150, 380, 18, GREEN);


		if (remaining > 0.0)
		{
			Konto1Delta.DecimalTime.Combined = 0.0 - remaining ;
			Konto1Delta.DecimalTime.CombinedTimeToComponents();
			Konto1Delta.ConvertToTimeFormat();
		}
		else
		{
			Konto1Delta.DecimalTime.Combined = std::max(-4.0, std::min((workedTime - totalPause) - 8, 1.0));
			Konto1Delta.DecimalTime.CombinedTimeToComponents();
			Konto1Delta.ConvertToTimeFormat();
		}


		Konto2Delta.DecimalTime.Combined = std::max(0.0, std::min(workedTime - totalPause - 9.0, 24.0));
		Konto2Delta.DecimalTime.CombinedTimeToComponents();
		Konto2Delta.ConvertToTimeFormat();

		DrawText(TextFormat("Konto 1 Delta : %d:%d", Konto1Delta.Hour, Konto1Delta.Minutes), GetScreenWidth() / 2.0f - 150, 450, 18, GREEN);
		DrawText(TextFormat("Konto 2 Delta : %d:%d", Konto2Delta.Hour, Konto2Delta.Minutes), GetScreenWidth() / 2.0f - 150, 470, 18, GREEN);

		if (isDebug)
		{
			DebugTimeBox.Update();
		}
		ArriveTimeBox.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 180 }).UpdateTextPosition().Update();
		PauseTimeBox.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 240 }).UpdateTextPosition().Update();
		CorrectionTimeBox.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 300 }).UpdateTextPosition().Update();
		OutputTime.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 360 }).UpdateTextPosition().Update();


		if (isDebug)
		{
		DrawText(TextFormat("Current Time: %f", CurrentTime.DecimalTime.Combined), GetScreenWidth() / 2.0f - 250, 0, 18, GREEN);
		DrawText(TextFormat("Start Time:  %f", StartTime.DecimalTime.Combined), GetScreenWidth() / 2.0f - 250, 20, 18, GREEN);
		DrawText(TextFormat("Remaining Time: %f", remaining), GetScreenWidth() / 2.0f - 250, 40, 18, GREEN);
		DrawText(TextFormat("Pause Time:  %f", PauseTime.DecimalTime.Combined), GetScreenWidth() / 2.0f - 250, 60, 18, GREEN);
		DrawText(TextFormat("workedTime - totalPause:  %f", workedTime - totalPause), GetScreenWidth() / 2.0f - 250, 80, 18, GREEN);
		}

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}


	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
