#pragma once
#include "GameMode.h"
class TimeCalcMode : public GameMode
{
	TimeCalcMode(bool bIsDebug);
	~TimeCalcMode() override;

	virtual void Update() override;
};

