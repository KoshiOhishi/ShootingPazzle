#include "FPSManager.h"
#include "DebugText.h"

double FPSManager::FRAME_TIME = 0.0f;
std::chrono::steady_clock::time_point FPSManager::start;
std::chrono::steady_clock::time_point FPSManager::end;

double FPSManager::nowFPS = 60;
bool FPSManager::isAdjust = false;
bool FPSManager::isPrintFPS = false;

void FPSManager::SetFPS(double fps, bool isPrintFPS)
{
	if (fps > 0) {
		FRAME_TIME = 1.0 / fps;
		isAdjust = true;
	}
	else {
		isAdjust = false;
	}
	FPSManager::isPrintFPS = isPrintFPS;
	start = std::chrono::steady_clock::now();
}

void FPSManager::Update()
{
	static int count = 0;
	static double totalDeltaTime = 0;

	auto end = std::chrono::steady_clock::now();
	double deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0f;

	totalDeltaTime += deltaTime;
	count++;

	if (isAdjust) {

		if (deltaTime < FRAME_TIME)
		{
			DWORD sleepTime = static_cast<DWORD>((FRAME_TIME - deltaTime) * 1000);
			totalDeltaTime += FRAME_TIME - deltaTime;
			timeBeginPeriod(1);
			Sleep(sleepTime);
			timeEndPeriod(1);
		}
	}


	if (totalDeltaTime >= 1.0) { // Œo‰ßŠÔ‚ª0‚æ‚è‘å‚«‚¢(‚±‚¤‚µ‚È‚¢‚Æ‰º‚ÌŒvZ‚Åƒ[ƒœZ‚É‚È‚é‚Æv‚í‚ê)
		nowFPS = (double)count / totalDeltaTime; // •½‹Ïfps‚ğŒvZ
		count = 0;
		totalDeltaTime = 0;
	}
	//o—Í
	if (isPrintFPS) {
		DebugText::Print("FPS:" + std::to_string(FPSManager::GetFPS()), 0, 0);
	}

	start = std::chrono::steady_clock::now();
}
