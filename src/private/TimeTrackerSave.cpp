#include "TimeTrackerSave.h"

TimeTrackerSave::~TimeTrackerSave()
{

}

void TimeTrackerSave::SerializeData(TimeFormat CorrectionTime, TimeFormat PauseTime, TimeFormat StartTime)
{
	m_CorrectionTime = CorrectionTime;
	m_PauseTime = PauseTime;
	m_StartTime = StartTime;
}

TimeCalculator TimeTrackerSave::GetSerializedData()
{
	TimeCalculator NewTimeCalc;
	NewTimeCalc.SetCorrectionTime(m_CorrectionTime);
	NewTimeCalc.SetPauseTime(m_PauseTime);
	NewTimeCalc.SetStartTime(m_StartTime);

	NewTimeCalc.UpdateTextBox();

	return NewTimeCalc;
}
