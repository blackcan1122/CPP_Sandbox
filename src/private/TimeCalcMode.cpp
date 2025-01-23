#include "TimeCalcMode.h"
#include "trex.h"
#include "Logo.h"


TimeCalcMode::TimeCalcMode(bool bIsDebug)
{
	SetTargetFPS(30);
	image = LoadImageFromMemory(".jpg", trex_jpg, trex_jpg_len);     // Loaded in CPU memory (RAM)
	imageLogo = LoadImageFromMemory(".png", Logo_png, Logo_png_len);     // Loaded in CPU memory (RAM)
	texture = LoadTextureFromImage(image);          // Image converted to texture, GPU memory (VRAM)
	SetWindowSize(600, 600);
	ClearWindowState(FLAG_WINDOW_RESIZABLE);

	SetWindowIcon(imageLogo);
	UnloadImage(imageLogo);
	UnloadImage(image);   // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM


}

void TimeCalcMode::Update()
{
	ClearBackground(BLACK);

	// Setup the back buffer for drawing (clear color and depth buffers)

	DrawTexture(texture, GetScreenWidth() / 2 - texture.width / 2, GetScreenHeight() / 2 - texture.height / 2, DARKGRAY);

	ActiveTimeCalc.UpdateCalculator();


	DrawText("Kommen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 200, 18, GREEN);
	DrawText("Pausen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 260, 18, GREEN);
	DrawText("Korrektur Zeit HH:MM :", GetScreenWidth() / 2.0f - 223, 320, 18, GREEN);
	DrawText("Restliche Qual:", GetScreenWidth() / 2.0f - 150, 380, 18, GREEN);

}

TimeCalcMode::~TimeCalcMode()
{
	UnloadTexture(texture);
	std::cout << "Destructor is called" << std::endl;
}
