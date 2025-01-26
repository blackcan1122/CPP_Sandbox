#pragma once
#include "Core.h"
#include "Event.hpp"
#include "TextBox.h"

class UIEvent : public Event
{
public:
	virtual std::string GetName() const override;

	BaseUI* ClickedUIElement = nullptr;

};

