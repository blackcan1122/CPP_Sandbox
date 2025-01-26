#pragma once
#include "Core.h"
#include "GameModeState.h"
#include "TimeCalculator.h"

class TimeTrackerSave : public GameModeState
{
public:

	TimeTrackerSave() = default;
	virtual ~TimeTrackerSave() override;

	void SerializeData(TimeFormat CorrectionTime, TimeFormat PauseTime, TimeFormat StartTime);
	TimeCalculator GetSerializedData();

private:
	TimeFormat m_CorrectionTime;
	TimeFormat m_PauseTime;
	TimeFormat m_StartTime;

};