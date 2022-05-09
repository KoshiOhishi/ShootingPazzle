#include "Timer.h"
#include <time.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

using namespace DX12Library;

std::vector<Timer*> Timer::timers;

Timer::Timer(int start, int end, bool isLoop, float speed)
{
	startTime = start;
	nowTime = startTime;
	endTime = end;
	updateSpeed = speed;
	this->isLoop = isLoop;
	isStart = false;

	//—v‘f‚ª‚È‚©‚Á‚½‚ç’Ç‰Á
	if (std::find(timers.begin(), timers.end(), this) == timers.end()) {
		timers.emplace_back(this);
	}
}

DX12Library::Timer::~Timer()
{
	auto itr = std::find(timers.begin(), timers.end(), this);
	//—v‘f‚ª‚ ‚Á‚½‚çíœ
	if (itr != timers.end()) {
		timers.erase(itr);
	}
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

	//—v‘f‚ª‚È‚©‚Á‚½‚ç’Ç‰Á
	if (std::find(timers.begin(), timers.end(), this) == timers.end()) {
		timers.emplace_back(this);
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

void Timer::SetNowTime(int time)
{
	if (time < startTime) { time = startTime; }
	if (time > endTime) { time = endTime; }

	nowTime = time;
}

void Timer::UpdateAll()
{
	for (auto& v : timers) {
		v->Update();
	}
}

int Timer::GetClockNowTime()
{
	int t = timeGetTime();
	return t;
}
