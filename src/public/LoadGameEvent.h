#pragma once
#include "Core.h"
#include "Event.hpp"
#include "TimeCalcMode.h"
#include "TimeTrackerSave.h"

class LoadGameEvent : public Event
{
public:
	virtual ~LoadGameEvent() override;

	virtual std::string GetName() const override;

	TimeCalcMode* TimeCalcGameMode = nullptr;

};