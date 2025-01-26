#pragma once
#include "GameMode.h"
#include "TimeCalculator.h"
#include "raylib.h"
#include "TimeTrackerSave.h"


class TimeCalcMode : public GameMode
{
public:
	TimeCalcMode(bool bIsDebug);
	TimeCalcMode(bool bIsDebug, TimeTrackerSave UseExistingTimeCalc);
	~TimeCalcMode() override;

	void LoadSaveGame(TimeTrackerSave UseSave);

	TimeCalculator ActiveTimeCalc;
	Image image;
	Image imageLogo;
	Texture2D texture;

	TimeTrackerSave GetSerializedData();
	
	virtual void Update() override;
};

