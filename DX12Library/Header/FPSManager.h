#pragma once
#include <Windows.h>
#include <timeapi.h>
#include <chrono>
#pragma comment(lib,"winmm.lib")

class FPSManager
{
private:
	static double FRAME_TIME;
	static std::chrono::steady_clock::time_point start;
	static std::chrono::steady_clock::time_point end;

	static double nowFPS;
	static bool isAdjust;
	static bool isPrintFPS;

public:
	/// <summary>
	/// FPSをセット(0以下で制限しない)
	/// </summary>
	/// <param name="fps">動作させるfps。0以下で制限なし</param>
	static void SetFPS(double fps = -1, bool isPrintFPS = false);

	/// <summary>
	/// プログラムループの終わりに記述する fpsの調整を行う
	/// </summary>
	static void Update();

	/// <summary>
	/// 現在のfpsを取得
	/// </summary>
	/// <returns></returns>
	static double GetFPS() { return nowFPS; }

	/// <summary>
	/// ある値を60fpsで動作させるために乗算させる数
	/// </summary>
	/// <returns></returns>
	static double GetMulAdjust60FPS() { return 60.0 / nowFPS; }
};

