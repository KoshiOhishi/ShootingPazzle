#pragma once
#include <string>
#include "Timer.h"

//参考:https://easings.net/ja

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
	static const double c1;
	static const double c2;
	static const double c3;
	static const double c4;
	static const double c5;
	static const double n1;
	static const double d1;

	/// <summary>
	/// 軽量化pow 自然数乗のときはこっちを使うこと
	/// </summary>
	/// <param name="x">値</param>
	/// <param name="t">累乗数</param>
	/// <returns>結果</returns>
	static double Pow(double x, int t);

public:
	/// <summary>
	/// イージングをかけた値を取得
	/// </summary>
	/// <param name="type">イージングの種類</param>
	/// <param name="start">t=0の時の値</param>
	/// <param name="end">t=1の時の値</param>
	/// <param name="timer">タイマー ※開始-終了を0より大きくすること</param>
	/// <param name="startTime">t=0の時の時間 それより前は0固定</param>
	/// <param name="endTime">t=1の時の時間　それ以降は1固定</param>
	/// <returns>イージングをかけた値</returns>
	static double GetEaseValue(int type, double start, double end, const Timer& timer, double startTime = -1, double endTime = -1);

	/// <summary>
	/// イージングをかけた値を取得
	/// </summary>
	/// <param name="type">イージングの種類</param>
	/// <param name="start">t=0の時の値</param>
	/// <param name="end">t=1の時の値</param>
	/// <param name="t">0～1の間の数値</param>
	/// <returns>イージングをかけた値</returns>
	static double GetEaseValue(int type, double start, double end, double t);

	static const std::string& GetFuncName(int type);
};

