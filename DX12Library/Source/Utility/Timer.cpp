#include "Timer.h"
#include <time.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

Timer::Timer()
{
}

Timer::Timer(int start, int end, bool isLoop, float speed)
{
	startTime = start;
	nowTime = startTime;
	endTime = end;
	updateSpeed = speed;
	this->isLoop = isLoop;
	isStart = false;
}

void Timer::SetTimer(int start, int end, bool isLoop, float speed)
{
	startTime = start;
	nowTime = startTime;
	endTime = end;
	updateSpeed = speed;
	if (updateSpeed < 0) {
		updateSpeed = 1.0f;
	}

	this->isLoop = isLoop;
	isStart = false;
	if (start > end) {
		updateSpeed *= -1.0f;
	}
	else if (start == end) {
		updateSpeed = 0;
	}
}

void Timer::Update()
{
	int addTime = (GetClockNowTime() - prevTime) * updateSpeed;
	if (isStart == true) {
		nowTime += addTime;
	}

	prevTime = GetClockNowTime();

	if ((updateSpeed >= 0 && nowTime >= endTime) ||
		(updateSpeed < 0 && nowTime <= endTime))
	{
		if (isLoop) {
			nowTime -= (endTime - startTime);
		}
		else {
			nowTime = endTime;
			isStart = false;
		}
	}
}

void Timer::Start()
{
	isStart = true;
	prevTime = GetClockNowTime();
}

void Timer::Stop()
{
	isStart = false;
}

void Timer::Reset()
{
	isStart = false;
	nowTime = startTime;
}

int Timer::GetClockNowTime()
{
	int t = timeGetTime();
	return t;
}
