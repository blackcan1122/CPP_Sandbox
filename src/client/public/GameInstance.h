#pragma once
#include "Core.h"
#include "EventDispatcher.hpp"

class GameInstance
{

private:
	GameInstance(int ScreenWidth, int Screenheight, int TargetFps, bool Fullscreen, bool IsDebug);
	~GameInstance() = default;

	static GameInstance* Instance;

protected:

	/*
	*****************************
	* Window and Start Parameter
	*****************************
	*/

	int m_ScreenWidth;
	int m_ScreenHeight;
	int m_TargetFps;
	bool m_Fullscreen;
	bool m_IsDebug = false;

	static EventDispatcher UIEventDispatcher;
	static EventDispatcher SaveStateDispatcher;

	/*
	*****************************
	* Methods
	*****************************
	*/

	static void CreateWindow();
	static void GameLoop();

public:

	GameInstance(const GameInstance&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;

	static void InitGameInstance(int ScreenWidth, int Screenheight, int TargetFps, bool Fullscreen, bool IsDebug);
	static GameInstance* GetInstance();


	static EventDispatcher& GetUIEventDispatcher();
	static EventDispatcher& GetSaveStateEventDispatcher();


};