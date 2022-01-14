#pragma once

/// <summary>
/// ミリ秒精度タイマー
/// </summary>
class Timer
{
private:
	int nowTime;
	int startTime;
	int endTime;
	int countStartTime;
	int prevTime;
	float updateSpeed = 1.0f;
	bool isStart = false;
	bool isLoop = false;

private:
	static int GetClockNowTime();

public:
	Timer();
	Timer(int start, int end, bool isLoop = false, float speed = 1.0f);

	/// <summary>
	/// タイマーをセット
	/// </summary>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="speed">更新倍率</param>
	void SetTimer(int start, int end, bool isLoop = false, float speed = 1.0f);

	/// <summary>
	/// タイマー更新
	/// </summary>
	void Update();

	/// <summary>
	/// タイマーの更新を開始
	/// </summary>
	void Start();

	/// <summary>
	/// タイマーの更新を一時停止
	/// </summary>
	void Stop();

	/// <summary>
	/// タイマーの現在値を開始値に戻し、更新を一時停止
	/// </summary>
	void Reset();

	/// <summary>
	/// タイマーの現在時間をセット 開始値 <= time <= 終了値にクランプされる。
	/// </summary>
	/// <param name="time"></param>
	void SetNowTime(int time);

	/// <summary>
	/// タイマーが開始しているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsStart()const { return isStart; }

	/// <summary>
	/// タイマーが終了値に達しているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsEnd()const { return nowTime >= endTime; }

	/// <summary>
	/// 開始値を取得
	/// </summary>
	/// <returns></returns>
	int GetStartTime()const { return startTime; }

	/// <summary>
	/// 終了値を取得
	/// </summary>
	/// <returns></returns>
	int GetEndTime()const { return endTime; }

	/// <summary>
	/// 現在値を取得
	/// </summary>
	/// <returns></returns>
	int GetNowTime()const { return nowTime; }
};

