#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Block.h"

#define PI 3.14159265359f
//1マスの長さ
#define ONE_CELL_LENGTH (float)5.0f

//ゲームフェーズ
enum Phase
{
	PHASE_SETPOS,
	PHASE_SETANGLE,
	PHASE_AFTERSHOOT
};

class GameUtility
{

private:
	//現在のフェーズ
	static int nowPhase;

public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	static void SetNowPhase(int phase) { nowPhase = phase; }
	static int GetNowPhase() { return nowPhase; }
};