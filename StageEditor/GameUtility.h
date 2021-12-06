#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Block.h"
#include "Stage.h"

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

struct StageHeader
{
	unsigned short width = 20;
	unsigned short depth = 20;
	unsigned short startLineZ = depth - 12;
	unsigned short objectCount = 0;
};

struct StageObject
{
	unsigned short stagePosX = 0;
	unsigned short stagePosY = 0;
	char type = 0;
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

	/// <summary>
	/// ワールド座標のxz平面からステージ上の座標に変換
	/// </summary>
	/// <param name="worldX">ワールド座標x</param>
	/// <param name="worldZ">ワールド座標z</param>
	/// <returns></returns>
	static const StageVec2& CalcWorldPos2StagePos(float worldX, float worldZ, const StageVec2& stageSize);

	/// <summary>
	/// ステージ上の座標からワールド座標のxz平面に変換
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <param name="dstWorldX">ワールド座標x</param>
	/// <param name="dstWorldZ">ワールド座標z</param>
	static void CalcStagePos2WorldPos(const StageVec2& stagePos, const StageVec2& stageSize, float* dstWorldX = nullptr, float* dstWorldZ = nullptr);
};