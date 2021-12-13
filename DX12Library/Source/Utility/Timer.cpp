#include "Timer.h"
#include <time.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

Timer::Timer()
{
}

Timer::Timer(int start, int end, float speed)
{
	startTime = start;
	nowTime = startTime;
	endTime = end;
	updateSpeed = speed;
	isStart = false;
}

void Timer::SetTimer(int start, int end, float speed)
{
	startTime = start;
	nowTime = startTime;
	endTime = end;
	updateSpeed = speed;
	isStart = false;
}

void Timer::Update()
{

	if (isStart == true)
		nowTime += (GetClockNowTime() - prevTime) * updateSpeed;

	prevTime = GetClockNowTime();

	if (nowTime >= endTime)
	{
		nowTime = endTime;
		isStart = false;
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
