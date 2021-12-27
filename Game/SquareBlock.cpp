#include "SquareBlock.h"
#include "GameUtility.h"

InstancingObjectDraw SquareBlock::instancingObjectDraw;
ObjModel SquareBlock::modelBox;

void SquareBlock::CreateModel()
{
	//モデル生成
	modelBox.CreateFromOBJ("SquareBlock");
}

void SquareBlock::StaticInitialize()
{
	CreateModel();
	instancingObjectDraw.Initialize();
	instancingObjectDraw.SetObjModel(&modelBox);
}

void SquareBlock::Initialize(const StageVec2& pos, float sphereRadius)
{
	//オブジェクト生成
	object.Initialize();

	//直方体の周りにつけるカプセル判定 (三角形なので3つ)
	capsule.clear();
	for (int i = 0; i < 4; i++) {
		capsule.push_back({});
		capsule[i].radius = sphereRadius;
	}

	SetStagePos(pos);

	UpdateCollision();
}

void SquareBlock::Update()
{
	UpdateColor();
	object.Update(instancingObjectDraw);
	UpdateCollision();
}

void SquareBlock::Draw()
{
	instancingObjectDraw.Update();
	instancingObjectDraw.Draw();
}

void SquareBlock::EndDraw()
{
	instancingObjectDraw.EndDraw();
}

void SquareBlock::UpdateCollision()
{
	const Vector3& position = object.GetPosition();
	Vector3 topLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//左奥
	Vector3 topRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//右奥
	Vector3 bottomLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//左手前
	Vector3 bottomRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//右手前

	//カプセルの値更新
	capsule[0].start = topLeft;
	capsule[0].end = topRight;

	capsule[1].start = topRight;
	capsule[1].end = bottomRight;

	capsule[2].start = bottomRight;
	capsule[2].end = bottomLeft;

	capsule[3].start = bottomLeft;
	capsule[3].end = topLeft;
}

void SquareBlock::DecrementBreakupCount()
{
	if (isBreakable == false) {
		return;
	}

	breakupCount--;
	if (breakupCount <= 0) {
		Breakup();
	}
}

void SquareBlock::Breakup()
{
	//ここに爆発エフェクト入れる

	//残り目標ブロック数を減らす
	pStage->DecrementTargetBlockCount();
}
