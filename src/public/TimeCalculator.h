#pragma once
#include "raylib.h"
#include "TextBox.h"

struct TimeFormatDecimal
{
	double Hour = 0;
	double Minutes = 0;
	double Combined = 0;

	double& operator [](int Number)
	{
		switch (Number % 3)
		{
		case 0:
			return Hour;
			break;

		case 1:
			return Minutes;
			break;
		case 2:
			return Combined;
			break;
		default:
			break;
		}
	}

	void CombinedTimeToComponents()
	{
		if (Combined > 0.0)
		{
			Hour = floor(Combined);
			Minutes = Combined - floor(Combined);
		}
		else
		{
			Hour = ceil(Combined);
			Minutes = Combined - ceil(Combined);
		}

	}

};


struct TimeFormat
{
	int Hour = 0;
	int Minutes = 0;
	TimeFormatDecimal DecimalTime;

	int& operator [](int Number)
	{
		switch (Number % 2)
		{
		case 0:
			return Hour;
			break;

		case 1:
			return Minutes;
			break;

		default:
			break;
		}
	}

	void ConvertToDecimalTime()
	{
		DecimalTime[0] = Hour;
		DecimalTime[1] = (1.0 * Minutes) / 60;
		DecimalTime[2] = DecimalTime[0] + DecimalTime[1];
	}

	void ConvertToTimeFormat()
	{
		Hour = static_cast<int>(DecimalTime[0]);
		Minutes = static_cast<int>((round((DecimalTime[1]) * 60)));
	}

	bool ConvertStringToTimeFormat(char* String)
	{
		if (String == nullptr)
		{
			return false;
		}

		if (String[2] != ':')
		{
			return false;
		}

		if (strlen(String) > 5)
		{
			return false;
		}

		// Copying String, as Loop does destruct the string
		char* TempChar = String;

		while (*TempChar != '\0')
		{  // Loop until null terminator
			if (*TempChar == ':')
			{
				++TempChar;
				continue;
			}
			if (!std::isdigit(*TempChar))
			{
				return false;  // If any char is not a digit, return false
			}
			++TempChar;  // Move to next character
		}

		if ((String[3] - '0') > 6 || (String[0] - '0') > 2)
		{
			return false;
		}

		Hour = (10 * (String[0] - '0') + (String[1] - '0'));
		Minutes = (10 * (String[3] - '0') + (String[4] - '0'));
		return true;

	}

	std::string ConvertToString()
	{
		std::string NewString;
		if (Hour <= 9)
		{
			NewString += "0" + std::to_string(Hour) + ":";
		}
		else
		{
			NewString += std::to_string(Hour) + ":";
		}
		if (Minutes <= 9)
		{
			NewString += "0" + std::to_string(Minutes);
		}
		else
		{
			NewString += std::to_string(Minutes);
		}

		return NewString;
	}
};


class TimeCalculator
{
public:

	TimeCalculator();
	void SetDebugMode(bool Debug);
	void UpdateCalculator();

	TimeFormat GetCorrectionTime();
	TimeFormat GetPauseTime();
	TimeFormat GetStartTime();

	void SetCorrectionTime(TimeFormat NewCorrectionTime);
	void SetStartTime(TimeFormat NewStartTime);
	void SetPauseTime(TimeFormat NewPauseTime);
	void UpdateTextBox();



private:

	void InitMember();
	bool IsDebug();
	void CalculateTime();
	void HandleRendering();
	void GetCurrentTime();

	TimeFormat CorrectionTime;
	TimeFormat PauseTime;
	TimeFormat StartTime;
	TimeFormat CurrentTime;
	TimeFormat DifferenceTime;
	TimeFormat EstimatedLeaveTime;
	TimeFormat Konto1Delta;
	TimeFormat Konto2Delta;


	TextInputBox EstimatedLeaveTimeBox;
	TextInputBox ArriveTimeBox;
	TextInputBox DebugTimeBox;
	TextInputBox PauseTimeBox;
	TextInputBox CorrectionTimeBox;
	TextInputBox OutputTime;

	bool bIsDebug = false;


};