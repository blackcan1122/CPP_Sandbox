#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif // !RAYGUI_IMPLEMENTATION
#include "GameInstance.h"
#include "Core.h"
#include "StateMachine.h"
#include "GameMode.h"
#include "TimeCalcMode.h"
#include "SandboxMode.h"
#include "TextBox.h"
#include "UIEvent.h"
#include "SaveGameEvent.h"
#include "LoadGameEvent.h"
#include "EventDispatcher.hpp"
#include "Button.h"
#include "PongGameMod.h"
#include "TimeTrackerSave.h"

EventDispatcher GameInstance::UIEventDispatcher;
EventDispatcher GameInstance::SaveStateDispatcher;

// Definition of the static member
GameInstance* GameInstance::Instance = nullptr;

GameInstance::GameInstance(int ScreenWidth, int Screenheight, int TargetFps, bool Fullscreen, bool IsDebug)
	: m_ScreenWidth(ScreenWidth), m_ScreenHeight(Screenheight), m_TargetFps(TargetFps), m_Fullscreen(Fullscreen), m_IsDebug(IsDebug)
{
	std::cout << "GameInstance was Initialized" << std::endl;
}


void GameInstance::InitGameInstance(int ScreenWidth, int Screenheight, int TargetFps, bool Fullscreen, bool isDebug)
{
	if (Instance != nullptr)
	{
		std::cerr << "GameInstance was already initialized" << std::endl;
		return;
	}

	Instance = new GameInstance(ScreenWidth, Screenheight, TargetFps, Fullscreen, isDebug);
	CreateWindow();
	GameLoop();
}

GameInstance* GameInstance::GetInstance()
{
	if (Instance != nullptr)
	{
		return Instance;
	}
	std::cerr << "GameInstance was not Created, please Call InitGameInstance first" << std::endl;
}

EventDispatcher& GameInstance::GetUIEventDispatcher()
{
	return UIEventDispatcher;
}

EventDispatcher& GameInstance::GetSaveStateEventDispatcher()
{
	return SaveStateDispatcher;
}


void GameInstance::CreateWindow()
{
	SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

	InitWindow(Instance->m_ScreenWidth, Instance->m_ScreenHeight, "I DONT EVEN KNOW WTF IM DOING ?!?!?!?!?!?!?");
	SetTargetFPS(Instance->m_TargetFps);
}

void GameInstance::GameLoop()
{
	StateMachine ActiveStateMachine;


	ActiveStateMachine.RegisterState("Menu", []() {return new TimeCalcMode(false); });
	ActiveStateMachine.RegisterState("Sandbox", []() {return new SandboxMode(); });
	ActiveStateMachine.RegisterState("Pong", []() {return new PongGameMod(); });

	//SaveState
	std::shared_ptr<TimeTrackerSave> NewSave = nullptr;
	
	SaveStateDispatcher.AddListener("SaveGameEvent", [&NewSave](std::shared_ptr<Event> Event) -> void
		{ 
			if (std::shared_ptr<SaveGameEvent> CurrentEvent = std::dynamic_pointer_cast<SaveGameEvent>(Event))
			{
				NewSave = CurrentEvent->SaveGame;
			}
			
		});

	SaveStateDispatcher.AddListener("LoadGameEvent", [&NewSave](std::shared_ptr<Event> Event) -> void
		{
			if (NewSave == nullptr)
			{
				return;
			}
			if (std::shared_ptr<LoadGameEvent> CurrentEvent = std::dynamic_pointer_cast<LoadGameEvent>(Event))
			{
				CurrentEvent->TimeCalcGameMode->LoadSaveGame(*NewSave.get());
			}

		});

	// Setting initial Start Mode
	ActiveStateMachine.ChangeState("Menu");

	// Handling UI Event
	// We Use this Event to Switch GameMode which then will update in the GameLoop
	UIEventDispatcher.AddListener("UIEvent", [&ActiveStateMachine](std::shared_ptr<Event> Event) -> void
		{
			auto ButtonClickEvent = std::dynamic_pointer_cast<UIEvent>(Event);
			if (ButtonClickEvent == nullptr)
			{
				return;
			}
			Button* ClickedButton = static_cast<Button*>(ButtonClickEvent.get()->ClickedUIElement);

			ActiveStateMachine.ChangeState(ClickedButton->GetEventPayload());

		});

	int TestSize = MeasureText("Time Calculator", 18);
	int PlaySize = MeasureText("Sandbox", 18);
	int OtherPlaySize = MeasureText("Pong", 18);
	int OptionSize = MeasureText("Option", 18);

	int Width = GetScreenWidth() / 4;


	Button MenuButton;
	Rectangle NewRec = { Width-Width,-50,Width,100 };
	MenuButton.Construct(NewRec, "Time Calculator", LIGHTGRAY, true, 0.2)
		.SetEventPayload("Menu")
		.SetEventDispatcher(std::make_shared<EventDispatcher>(UIEventDispatcher))
		.UpdateTextPosition((Width / 2) - (TestSize / 2), 75)
		.UpdateFontSize(18)
		.OnHover([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width - Width, -25);
				MenuButton->UpdateColor(DARKGRAY);
				MenuButton->UpdateTextColor(RAYWHITE);
			})
		.OnHoverLeave([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width - Width, -50);
				MenuButton->UpdateColor(LIGHTGRAY);
				MenuButton->UpdateTextColor(BLACK);
			});

	Button PlayButton;
	Rectangle NewRecPlay = { Width*2 - Width,-50,Width,100 };
	PlayButton.Construct(NewRecPlay, "Sandbox", LIGHTGRAY, true, 0.2)
		.SetEventPayload("Sandbox")
		.SetEventDispatcher(std::make_shared<EventDispatcher>(UIEventDispatcher))
		.UpdateTextPosition((Width / 2) - (PlaySize / 2), 75)
		.UpdateFontSize(18)
		.OnHover([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width*2 - Width, -25);
				MenuButton->UpdateColor(DARKGRAY);
				MenuButton->UpdateTextColor(RAYWHITE);
			})
		.OnHoverLeave([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width * 2 - Width, -50);
				MenuButton->UpdateColor(LIGHTGRAY);
				MenuButton->UpdateTextColor(BLACK);
			});

	Button OtherGameButton;
	Rectangle NewRecOtherGame = { Width * 3 - Width,-50,Width, 100 };
	OtherGameButton.Construct(NewRecOtherGame, "Pong", LIGHTGRAY, true, 0.2)
		.SetEventPayload("Pong")
		.SetEventDispatcher(std::make_shared<EventDispatcher>(UIEventDispatcher))
		.UpdateTextPosition((Width / 2) - (OtherPlaySize / 2), 75)
		.UpdateFontSize(18)
		.OnHover([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width * 3 - Width, -25);
				MenuButton->UpdateColor(DARKGRAY);
				MenuButton->UpdateTextColor(RAYWHITE);
			})
		.OnHoverLeave([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width * 3 - Width, -50);
				MenuButton->UpdateColor(LIGHTGRAY);
				MenuButton->UpdateTextColor(BLACK);
			});

	Button OptionButton;
	Rectangle NewRecOption = { Width * 4 - Width,-50,Width,100 };
	OptionButton.Construct(NewRecOption, "Option", LIGHTGRAY, true, 0.2)
		.SetEventPayload("Menu")
		.SetEventDispatcher(std::make_shared<EventDispatcher>(UIEventDispatcher))
		.UpdateTextPosition((Width / 2) - (OptionSize / 2), 75)
		.UpdateFontSize(18)
		.OnHover([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width * 4 - Width, -25);
				MenuButton->UpdateColor(DARKGRAY);
				MenuButton->UpdateTextColor(RAYWHITE);
			})
		.OnHoverLeave([Width](Button* MenuButton)
			{
				MenuButton->UpdateButtonPosition(Width * 4 - Width, -50);
				MenuButton->UpdateColor(LIGHTGRAY);
				MenuButton->UpdateTextColor(BLACK);
			});


	// GAMELOOP //
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ActiveStateMachine.UpdateGameMode();

		// GameMode Independend
		MenuButton.Update();
		PlayButton.Update();
		OtherGameButton.Update();
		OptionButton.Update();


		EndDrawing();
	}
}
