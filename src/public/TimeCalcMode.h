#pragma once
#include "GameMode.h"
#include "TimeCalculator.h"
#include "raylib.h"


class TimeCalcMode : public GameMode
{
public:
	TimeCalcMode(bool bIsDebug);
	~TimeCalcMode() override;

	TimeCalculator ActiveTimeCalc;
	Image image;
	Image imageLogo;
	Texture2D texture;

	virtual void Update() override;
};

