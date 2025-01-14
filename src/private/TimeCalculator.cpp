#include "TimeCalculator.h"
#include "time.h"
#include <functional>

TimeCalculator::TimeCalculator()
{
	InitMember();
}

void TimeCalculator::SetDebugMode(bool Debug)
{
	bIsDebug = Debug;
}

void TimeCalculator::UpdateCalculator()
{
	GetCurrentTime();
	CalculateTime();
	HandleRendering();
}

void TimeCalculator::InitMember()
{
	ArriveTimeBox.Construct(GetScreenWidth() / 2.0f, 180, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("07:30")
		.UpdateFontColor(DARKGRAY);

	DebugTimeBox.Construct(GetScreenWidth() / 2.0f, 125, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("15:30")
		.UpdateFontColor(DARKGRAY);


	PauseTimeBox.Construct(GetScreenWidth() / 2.0f, 240, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("00:30")
		.UpdateFontColor(DARKGRAY);

	CorrectionTimeBox.Construct(GetScreenWidth() / 2.0f, 300, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(true)
		.UpdateFontSize(22)
		.SetInitialText("00:00")
		.UpdateFontColor(DARKGRAY);

	OutputTime.Construct(GetScreenWidth() / 2.0f, 300 + 60, 225, 50, RAYWHITE)
		.UseBorder(false)
		.CanBeEdited(false)
		.SetInitialText("Output")
		.UpdateFontSize(22)
		.UpdateFontColor(DARKGRAY);

}

bool TimeCalculator::IsDebug()
{
	return bIsDebug;
}





void TimeCalculator::CalculateTime()
{
	StartTime.ConvertStringToTimeFormat(ArriveTimeBox.StringToHold);
	StartTime.ConvertToDecimalTime();


	PauseTime.ConvertStringToTimeFormat(PauseTimeBox.StringToHold);
	PauseTime.ConvertToDecimalTime();


	CorrectionTime.ConvertStringToTimeFormat(CorrectionTimeBox.StringToHold);
	CorrectionTime.ConvertToDecimalTime();

	double workedTime = CurrentTime.DecimalTime.Combined - StartTime.DecimalTime.Combined;
	double totalPause = PauseTime.DecimalTime.Combined - CorrectionTime.DecimalTime.Combined;
	double remaining = std::fmod(8.0 - (workedTime - totalPause), 24.0);





	double Temp = std::fmod(8.0 - ((CurrentTime.DecimalTime.Combined - StartTime.DecimalTime.Combined) - (PauseTime.DecimalTime.Combined - CorrectionTime.DecimalTime.Combined)), 24.0);

	DifferenceTime.DecimalTime.Combined = remaining;
	DifferenceTime.DecimalTime.CombinedTimeToComponents();
	DifferenceTime.ConvertToTimeFormat();

	std::string OutputText = std::to_string(DifferenceTime.Hour) + ":" + std::to_string(DifferenceTime.Minutes);
	OutputTime.UpdateTextShown(OutputText);

	if (remaining > 0.0)
	{
		Konto1Delta.DecimalTime.Combined = 0.0 - remaining;
		Konto1Delta.DecimalTime.CombinedTimeToComponents();
		Konto1Delta.ConvertToTimeFormat();
	}
	else
	{
		Konto1Delta.DecimalTime.Combined = std::max(-4.0, std::min((workedTime - totalPause) - 8, 1.0));
		Konto1Delta.DecimalTime.CombinedTimeToComponents();
		Konto1Delta.ConvertToTimeFormat();
	}


	Konto2Delta.DecimalTime.Combined = std::max(0.0, std::min(workedTime - totalPause - 9.0, 24.0));
	Konto2Delta.DecimalTime.CombinedTimeToComponents();
	Konto2Delta.ConvertToTimeFormat();



	if (IsDebug())
	{
		DrawText(TextFormat("Current Time: %f", CurrentTime.DecimalTime.Combined), GetScreenWidth() / 2.0f - 250, 0, 18, GREEN);
		DrawText(TextFormat("Start Time:  %f", StartTime.DecimalTime.Combined), GetScreenWidth() / 2.0f - 250, 20, 18, GREEN);
		DrawText(TextFormat("Remaining Time: %f", remaining), GetScreenWidth() / 2.0f - 250, 40, 18, GREEN);
		DrawText(TextFormat("Pause Time:  %f", PauseTime.DecimalTime.Combined), GetScreenWidth() / 2.0f - 250, 60, 18, GREEN);
		DrawText(TextFormat("workedTime - totalPause:  %f", workedTime - totalPause), GetScreenWidth() / 2.0f - 250, 80, 18, GREEN);
	}

}

void TimeCalculator::HandleRendering()
{
	DrawText(TextFormat("Konto 1 Delta : %d:%d", Konto1Delta.Hour, Konto1Delta.Minutes), GetScreenWidth() / 2.0f - 150, 450, 18, GREEN);
	DrawText(TextFormat("Konto 2 Delta : %d:%d", Konto2Delta.Hour, Konto2Delta.Minutes), GetScreenWidth() / 2.0f - 150, 470, 18, GREEN);

	if (IsDebug())
	{
		DebugTimeBox.Update();
	}

	ArriveTimeBox.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 180 }).UpdateTextPosition().Update();
	PauseTimeBox.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 240 }).UpdateTextPosition().Update();
	CorrectionTimeBox.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 300 }).UpdateTextPosition().Update();
	OutputTime.UpdatePosition(Vector2() = { GetScreenWidth() / 2.0f, 360 }).UpdateTextPosition().Update();


}

void TimeCalculator::GetCurrentTime()
{
	// Gathering Some Crucial Infos:
	time_t placeholderTime = time(NULL);
	struct tm* localTime = localtime(&placeholderTime);

	if (IsDebug())
	{
		CurrentTime.ConvertStringToTimeFormat(DebugTimeBox.StringToHold);
		CurrentTime.ConvertToDecimalTime();
	}
	else
	{
		CurrentTime[0] = localTime->tm_hour;
		CurrentTime[1] = localTime->tm_min;
		CurrentTime.ConvertToDecimalTime();
	}
}
