#pragma once
#include "Timer.h"
#include <string>

class Easing
{
#define PI 3.14159265359

#define EASE_LINE -1
#define EASE_INSIDE 0
#define EASE_OUTSINE 1
#define EASE_INOUTSINE 2
#define EASE_INQUAD 3
#define EASE_OUTQUAD 4
#define EASE_INOUTQUAD 5
#define EASE_INCUBIC 6
#define EASE_OUTCUBIC 7
#define EASE_INOUTCUBIC 8
#define EASE_INQUART 9
#define EASE_OUTQUART 10
#define EASE_INOUTQUART 11
#define EASE_INQUINT 12
#define EASE_OUTQUINT 13
#define EASE_INOUTQUINT 14
#define EASE_INEXPO 15
#define EASE_OUTEXPO 16
#define EASE_INOUTEXPO 17
#define EASE_INCIRC 18
#define EASE_OUTCIRC 19
#define EASE_INOUTCIRC 20
#define EASE_INBACK 21
#define EASE_OUTBACK 22
#define EASE_INOUTBACK 23
#define EASE_INELASTIC 24
#define EASE_OUTELASTIC 25
#define EASE_INOUTELASTIC 26
#define EASE_INBOUNCE 27
#define EASE_OUTBOUNCE 28
#define EASE_INOUTBOUNCE 29

private:
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;
	const double c3 = c1 + 1.0;
	const double c4 = (2.0 * PI) / 3.0;
	const double c5 = (2.0 * PI) / 4.5;
	const double n1 = 7.5625;
	const double d1 = 2.75;

	Timer paramTimer;
	int type = 0;
	double start = 0;
	double end = 0;
	double value = 0;

	/// <summary>
	/// 軽量化pow 自然数乗のときはこっちを使うこと
	/// </summary>
	/// <param name="x">値</param>
	/// <param name="t">累乗数</param>
	/// <returns>結果</returns>
	double Pow(double x, int t);

public:

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 値の設定
	/// </summary>
	/// <param name="start">値の開始値</param>
	/// <param name="end">値の終了値</param>
	/// <param name="type">easings.netにある関数名</param>
	/// <param name="takesTime">開始から終了までにかかる時間(ms)</param>
	void Set(double start, double end, int type = EASE_INSIDE, double takesTime = 1000);

	/// <summary>
	/// 関数の開始
	/// </summary>
	void Start();

	/// <summary>
	/// 関数の一時停止
	/// </summary>
	void Stop();

	/// <summary>
	/// 関数のリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 開始したかどうかを返す
	/// </summary>
	/// <returns>開始したかどうか</returns>
	bool IsStart() { return paramTimer.GetIsStart(); }

	/// <summary>
	/// 関数によって算出された値を取得
	/// </summary>
	/// <returns>値</returns>
	double GetValue() { return value; }

	static std::string GetFuncName(int type);
};

