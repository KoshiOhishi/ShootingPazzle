#pragma once
#include <string>
#include "Timer.h"

//参考:https://easings.net/ja

namespace DX12Library {

	enum EaseName {
		EASE_LINE,
		EASE_INSINE,
		EASE_OUTSINE,
		EASE_INOUTSINE,
		EASE_INQUAD,
		EASE_OUTQUAD,
		EASE_INOUTQUAD,
		EASE_INCUBIC,
		EASE_OUTCUBIC,
		EASE_INOUTCUBIC,
		EASE_INQUART,
		EASE_OUTQUART,
		EASE_INOUTQUART,
		EASE_INQUINT,
		EASE_OUTQUINT,
		EASE_INOUTQUINT,
		EASE_INEXPO,
		EASE_OUTEXPO,
		EASE_INOUTEXPO,
		EASE_INCIRC,
		EASE_OUTCIRC,
		EASE_INOUTCIRC,
		EASE_INBACK,
		EASE_OUTBACK,
		EASE_INOUTBACK,
		EASE_INELASTIC,
		EASE_OUTELASTIC,
		EASE_INOUTELASTIC,
		EASE_INBOUNCE,
		EASE_OUTBOUNCE,
		EASE_INOUTBOUNCE,
	};

	class Easing
	{

		static const float PI;

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

}