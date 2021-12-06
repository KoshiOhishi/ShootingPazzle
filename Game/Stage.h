#pragma once
#include <vector>
#include "Block.h"
#include "Floor.h"

class Stage
{
private:
	int width;
	int depth;
	std::vector<Block*> blocks;
	Floor floor;

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
};

