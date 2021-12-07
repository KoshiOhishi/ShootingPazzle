#pragma once
#include <vector>
#include "Block.h"
#include "Floor.h"
#include "GameUtility.h"

class EditorStage
{
//エディット用なので公開でOK
public:
	StageVec2 stageSize = { 20,20 };
	std::vector<Block*> blocks;
	Floor floor;
	unsigned short startLaneZ = stageSize.y - 2;

public:
	EditorStage(){}
	~EditorStage();

	/// <summary>
	/// ステージを読み込んで初期化 ファイルが見つからない場合は20x20のブロックなしを生成
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
	/// ブロックを追加 (引数の位置に既にブロックが配置されていたら追加しない)
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <param name="blockType">ブロックの種類</param>
	/// <param name="breakupCount">壊れるまでの衝突回数　0で壊れないブロック</param>
	/// <param name="shapeType">オプションの形状指定</param>
	void AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount, int shapeType = 0);

	/// <summary>
	/// ブロックを削除 (引数の位置になにもない場合は削除しない)
	/// </summary>
	/// <param name="stagePos"></param>
	void DeleteBlock(const StageVec2& stagePos);

	/// <summary>
	/// 既にブロックが配置されているかチェック
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <returns>存在すればブロック配列のインデックス、なければ-1を返す</returns>
	int CheckExistBlock(const StageVec2& stagePos);
};

