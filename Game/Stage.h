#pragma once
#include <vector>
#include "Block.h"
#include "Floor.h"

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
	BLOCKTYPE_TRIANGLE
};

class Stage
{
private:
	StageVec2 stageSize = { 20,20 };
	std::vector<Block*> blocks;
	Floor floor;
	unsigned short startLaneZ = stageSize.y - 2;

public:
	Stage(){}
	~Stage();

	/// <summary>
	/// ステージを読み込んで初期化
	/// </smmary>
	/// <param name="filename">ファイルパス</param>
	void LoadStage(std::string filename);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ブロック配置取得
	/// </summary>
	/// <returns></returns>
	std::vector<Block*>& GetBlocks() { return blocks; }

	const Floor& GetFloor()const { return floor; }

	unsigned short GetStartLaneZ() { return startLaneZ; }

	const StageVec2& GetStageSize() { return stageSize; }
};

