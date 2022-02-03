#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Vector4.h"

//円周率
const float PI = 3.14159265359f;
//1マスの長さ
const float ONE_CELL_LENGTH = 5.0f;
//モデルのディレクトリ
const std::string MODEL_DIR = "Resources/Model/";
//2Dテクスチャのディレクトリ
const std::wstring TEX_DIR_UTIL = L"Resources/2DTexture/Utility/";
const std::wstring TEX_DIR_TITLE = L"Resources/2DTexture/Title/";
const std::wstring TEX_DIR_STAGESELECT = L"Resources/2DTexture/StageSelect/";
const std::wstring TEX_DIR_GAMEPLAY = L"Resources/2DTexture/GamePlay/";
const std::string TEX_DIR_GAMEPLAY_S = "Resources/2DTexture/GamePlay/";

const std::string STAGE_DIR = "Resources/StageData/";

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
	//定数
public:
	static const Vector4 COLOR_VALUE[5];
	static const Vector4 COLOR_VALUE_BREAKABLE[5];

private:
	//現在のフェーズ
	static int nowPhase;
	//ステージサイズを格納するポインタ
	static StageVec2* pStageSize;
	//ステージの現在の色
	static int stageColor;
	//現在のステージパス
	static std::string nowStagePath;
	//ゲームが一時停止状態か
	static bool isPause;

public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

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

	/// <summary>
	/// 現在のゲームフェーズをセット
	/// </summary>
	/// <param name="phase">PHASE_...</param>
	static void SetNowPhase(int phase) { nowPhase = phase; }

	/// <summary>
	/// 現在プレイ中のステージサイズ変数のポインタをセットする
	/// </summary>
	/// <param name="pStageSize"></param>
	static void SetPStageSize(StageVec2* pStageSize) { GameUtility::pStageSize = pStageSize; }

	/// <summary>
	/// ステージの設定色をセットする 同じ色のブロックが通り抜けられるようになる
	/// </summary>
	/// <param name="stageColor"></param>
	static void SetStageColor(int stageColor) { GameUtility::stageColor = stageColor; }

	/// <summary>
	/// 現在プレイ中のステージのパスをセット
	/// </summary>
	/// <param name="path"></param>
	static void SetNowStagePath(const std::string& path) { GameUtility::nowStagePath = path; }

	/// <summary>
	/// ポーズ中かのフラグセット
	/// </summary>
	/// <param name="isPause"></param>
	static void SetIsPause(bool isPause) { GameUtility::isPause = isPause; }


	/// <summary>
	/// 現在のフェーズ取得
	/// </summary>
	/// <returns></returns>
	static const int GetNowPhase() { return nowPhase; }

	/// <summary>
	/// ステージの設定色取得
	/// </summary>
	/// <returns></returns>
	static const int GetStageColor() { return stageColor; }

	/// <summary>
	/// 現在プレイ中のステージのパスを取得
	/// </summary>
	/// <returns></returns>
	static const std::string& GetNowStagePath() { return nowStagePath; }

	/// <summary>
	/// ポーズ中かどうか取得
	/// </summary>
	/// <returns></returns>
	static bool GetIsPause() { return isPause; }
};