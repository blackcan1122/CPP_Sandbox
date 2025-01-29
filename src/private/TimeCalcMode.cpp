#include "TimeCalcMode.h"
#include "Trex.h"
#include "Logo.h"
#include "GameInstance.h"
#include "EventDispatcher.hpp"
#include "SaveGameEvent.h"
#include "LoadGameEvent.h"


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

	std::shared_ptr<LoadGameEvent> LoadEvent = std::make_shared<LoadGameEvent>();
	LoadEvent->TimeCalcGameMode = this;

	GameInstance::GetSaveStateEventDispatcher().Dispatch(LoadEvent);

	ActiveTimeCalc.SetDebugMode(bIsDebug);

}

TimeCalcMode::TimeCalcMode(bool bIsDebug, TimeTrackerSave UseExistingTimeCalc)
{
	ActiveTimeCalc = UseExistingTimeCalc.GetSerializedData();
}


TimeTrackerSave TimeCalcMode::GetSerializedData()
{
	TimeTrackerSave NewSave;
	NewSave.SerializeData(ActiveTimeCalc.GetCorrectionTime(), ActiveTimeCalc.GetPauseTime(), ActiveTimeCalc.GetStartTime());
	return NewSave;
}

void TimeCalcMode::Update()
{
	ClearBackground(BLACK);

	// Setup the back buffer for drawing (clear color and depth buffers)

	float ScaleFactor = 1.f;

	DrawTextureEx(
		texture,
		Vector2{
			((float)GetScreenWidth() / 2) - ((texture.width * ScaleFactor) / 2),  // Center horizontally
			((float)GetScreenHeight() / 2) - ((texture.height * ScaleFactor) / 2) // Center vertically
		},
		0.0f,   // No rotation
		ScaleFactor,  // Scale texture to 25% of its original size
		DARKGRAY   // Default tint color
	);

	ActiveTimeCalc.UpdateCalculator();


	DrawText("Kommen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 200, 18, GREEN);
	DrawText("Pausen Zeit HH:MM :", GetScreenWidth() / 2.0f - 200, 260, 18, GREEN);
	DrawText("Korrektur Zeit HH:MM :", GetScreenWidth() / 2.0f - 223, 320, 18, GREEN);
	DrawText("Restliche Qual:", GetScreenWidth() / 2.0f - 150, 380, 18, GREEN);
	DrawText("Voraussichtliche Geh-Zeit:", GetScreenWidth() / 2.0f - 250, 440, 18, GREEN);

}

TimeCalcMode::~TimeCalcMode()
{
	UnloadTexture(texture);
	std::cout << "Destructor of TimeCalcMode is called" << std::endl;

	std::shared_ptr<SaveGameEvent> SaveEvent = std::make_shared<SaveGameEvent>();
	SaveEvent->SaveGame = std::make_shared<TimeTrackerSave>(GetSerializedData());

	GameInstance::GetSaveStateEventDispatcher().Dispatch(SaveEvent);

}

void TimeCalcMode::LoadSaveGame(TimeTrackerSave UseSave)
{
	ActiveTimeCalc = UseSave.GetSerializedData();
	ActiveTimeCalc.UpdateTextBox();
}
