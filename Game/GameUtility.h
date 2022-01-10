#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#define PI 3.14159265359f
//1マスの長さ
#define ONE_CELL_LENGTH (float)5.0f

const std::string modelDir = "Resources/Model/";

/// <summary>
/// ステージ座標 左上が原点
/// </summary>
struct StageVec2 {
	unsigned short x = 0;
	unsigned short y = 0;
};

/// <summary>
/// ブロックタイプ
/// </summary>
enum BlockType {
	BLOCKTYPE_SQUARE,
	BLOCKTYPE_TRIANGLE_NO_LEFTTOP,		//左上の角がない三角形
	BLOCKTYPE_TRIANGLE_NO_RIGHTTOP,		//右上の角がない三角形
	BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM,	//左下の角がない三角形
	BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM,	//右下の角がない三角形
};

enum FloorType {
	FLOORTYPE_NORMAL,
	FLOORTYPE_TURN_LEFT,
	FLOORTYPE_TURN_RIGHT,
	FLOORTYPE_TURN_UP,
	FLOORTYPE_TURN_DOWN,
	FLOORTYPE_BREAK,
	FLOORTYPE_SWITCH_NONE,
	FLOORTYPE_SWITCH_RED,
	FLOORTYPE_SWITCH_BLUE,
	FLOORTYPE_SWITCH_YELLOW,
	FLOORTYPE_SWITCH_GREEN,
};

//ゲームフェーズ
enum Phase
{
	PHASE_STAGESELECT_SELECT,
	PHASE_GAME_FIRSTEFFECT,
	PHASE_GAME_SETPOS,
	PHASE_GAME_SETANGLE,
	PHASE_GAME_AFTERSHOOT,
	PHASE_GAME_CLEAR
};

//ステージの色
enum StageColor
{
	STAGE_COLOR_NONE,
	STAGE_COLOR_RED,
	STAGE_COLOR_BLUE,
	STAGE_COLOR_YELLOW,
	STAGE_COLOR_GREEN,
};

struct StageHeader
{
	unsigned short width = 20;
	unsigned short depth = 20;
	unsigned short startLineZ = depth - 12;
	unsigned short blockCount = 0;
	unsigned short floorCount = 0;
};

struct StageBlock
{
	unsigned short stagePosX = 0;
	unsigned short stagePosY = 0;
	char type = 0;
	unsigned short breakupCount = 0;	//ブロックが何回の衝突で壊れるか (読み込み時0だと壊れないブロックになる)
	char blockColor = 0;
};

struct StageFloor
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
	//ステージサイズを格納するポインタ
	static StageVec2* pStageSize;
	//ステージの現在の色
	static int stageColor;
	//現在のステージパス
	static std::string nowStagePath;

public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	static void SetNowPhase(int phase) { nowPhase = phase; }
	static void SetPStageSize(StageVec2* pStageSize) { GameUtility::pStageSize = pStageSize; }
	static void SetStageColor(int stageColor) { GameUtility::stageColor = stageColor; }
	static void SetNowStagePath(const std::string& path) { GameUtility::nowStagePath = path; }

	static const int GetNowPhase() { return nowPhase; }
	static const int GetStageColor() { return stageColor; }
	static const std::string& GetNowStagePath() { return nowStagePath; }

	/// <summary>
	/// ワールド座標のxz平面からステージ上の座標に変換
	/// </summary>
	/// <param name="worldX">ワールド座標x</param>
	/// <param name="worldZ">ワールド座標z</param>
	/// <returns></returns>
	static const StageVec2& CalcWorldPos2StagePos(float worldX, float worldZ);

	/// <summary>
	/// ステージ上の座標からワールド座標のxz平面に変換
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <param name="dstWorldX">ワールド座標x</param>
	/// <param name="dstWorldZ">ワールド座標z</param>
	static void CalcStagePos2WorldPos(const StageVec2& stagePos, float* dstWorldX = nullptr, float* dstWorldZ = nullptr);
};