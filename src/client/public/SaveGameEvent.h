#pragma once
#include "Core.h"
#include "Event.hpp"
#include "TimeTrackerSave.h"


class SaveGameEvent : public Event
{
public:
	virtual ~SaveGameEvent() override;

	virtual std::string GetName() const override;

	std::shared_ptr<TimeTrackerSave> SaveGame = nullptr;
};