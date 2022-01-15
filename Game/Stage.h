#pragma once
#include <vector>
#include "BaseBlock.h"
#include "BaseFloor.h"
#include "Timer.h"

class Stage
{
	friend class BaseBlock;

private:
	//球半径
	const float sphereRadius = ONE_CELL_LENGTH / 2;
	//ステージの大きさ マス単位
	StageVec2 stageSize = { 20,20 };
	//ブロックコンテナ
	std::vector<BaseBlock*> blocks;
	//床ブロックコンテナ
	std::vector<BaseFloor*> floors;
	//マウスカーソルと判定を取る用の板コリジョン
	Plane floorCollision;
	//球のスタート奥行
	unsigned short startLaneZ = stageSize.y - 2;
	//残り目標破壊ブロック数
	int targetBlockCount = 0;
	//出現エフェクトタイマーのポインタ
	Timer* pFirstEffectTimer;
	//クリアエフェクトタイマーのポインタ
	Timer* pClearEffectTimer;

	InstancingObjectDraw iodSquareBlock[4];
	InstancingObjectDraw iodTriangleBlock[4];
	InstancingObjectDraw iodNormalFloor;
	InstancingObjectDraw iodSwitchFloor[2];
	InstancingObjectDraw iodTurnFloor[4];
	InstancingObjectDraw iodBreakFloor;

public:
	Stage() {}
	~Stage();

	void Initialize();

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
	/// 出現エフェクトの更新
	/// </summary>
	void UpdateFirstEffect();

	/// <summary>
	/// クリアエフェクト更新
	/// </summary>
	void UpdateClearEffect();

	/// <summary>
	/// 床ブロック空きマスに穴オブジェクトを挿入
	/// </summary>
	void InsertHole();

	/// <summary>
	/// 床ブロックが既に配置されているかを返す
	/// </summary>
	/// <param name="stagePos">ステージ基準座標</param>
	/// <returns>既に配置されていればfloorsのインデックス、なければ-1を返す</returns>
	int CheckExistFloor(const StageVec2& stagePos);

	/// <summary>
	/// ブロックを追加 (引数の位置に既にブロックが配置されていたら追加しない)
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <param name="blockType">ブロックの種類</param>
	/// <param name="breakupCount">壊れるまでの衝突回数　0で壊れないブロック</param>
	/// <param name="blockColor">ブロックの色指定</param>
	void AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount, int blockColor);

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

	/// <summary>
	/// 床ブロックを追加 (引数の位置に既にブロックが配置されていたら追加しない)
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <param name="floorType">床ブロックの種類</param>
	void AddFloor(const StageVec2& stagePos, int floorType);

	/// <summary>
	/// 床ブロックを削除 (引数の位置になにもない場合は削除しない)
	/// </summary>
	/// <param name="stagePos"></param>
	void DeleteFloor(const StageVec2& stagePos);

	/// <summary>
	/// 残り目標破壊ブロック数を減らす
	/// </summary>
	void DecrementTargetBlockCount() { targetBlockCount--; }

	/// <summary>
	/// ブロック配置取得
	/// </summary>
	/// <returns></returns>
	std::vector<BaseBlock*>& GetBlocks() { return blocks; }

	/// <summary>
	/// 床ブロック配置取得
	/// </summary>
	/// <returns></returns>
	std::vector<BaseFloor*>& GetFloors() { return floors; }

	/// <summary>
	/// マウスと床の当たり判定に使用する板コリジョン取得
	/// </summary>
	/// <returns></returns>
	const Plane& GetFloorCollision()const { return floorCollision; }

	/// <summary>
	/// スタートレーンのz座標取得
	/// </summary>
	/// <returns></returns>
	unsigned short GetStartLaneZ() { return startLaneZ; }

	/// <summary>
	/// ステージサイズを取得 単位はマス
	/// </summary>
	/// <returns></returns>
	const StageVec2& GetStageSize() { return stageSize; }

	/// <summary>
	/// 残り目標破壊ブロック数取得
	/// </summary>
	/// <returns>残り目標破壊ブロック数</returns>
	const int GetTargetBlockCount() { return targetBlockCount; }

	/// <summary>
	/// ブロック・床全てに適用されるマスター座標
	/// </summary>
	/// <param name="pos"></param>
	void SetMasterPosition(const Vector3& pos);

	void SetPFirstEffectTimer(Timer* pFirstEffectTimer) { this->pFirstEffectTimer = pFirstEffectTimer; }
	void SetPClearEffectTimer(Timer* pClearEffectTimer) { this->pClearEffectTimer = pClearEffectTimer; }
};