#include "SquareBlock.h"
#include "GameUtility.h"

InstancingObjectDraw SquareBlock::instancingObjectDraw[4];
ObjModel SquareBlock::modelBox[4];

void SquareBlock::CreateModel()
{
	//モデル生成
	modelBox[0].CreateFromOBJ("SquareBlock");
	modelBox[1].CreateFromOBJ("SquareBlock_Breakable_1");
	modelBox[2].CreateFromOBJ("SquareBlock_Breakable_2");
	modelBox[3].CreateFromOBJ("SquareBlock_Breakable_3");
}

void SquareBlock::StaticInitialize()
{
	CreateModel();
	for (int i = 0; i < _countof(modelBox); i++) {
		instancingObjectDraw[i].Initialize();
		instancingObjectDraw[i].SetObjModel(&modelBox[i]);
	}
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

	objectName = "SquareBlock";
}

void SquareBlock::Update()
{
	UpdateColor();
	object.Update(instancingObjectDraw[breakupCount]);
	UpdateCollision();
}

void SquareBlock::Draw()
{
	for (int i = 0; i < _countof(modelBox); i++) {
		instancingObjectDraw[i].Update();
		instancingObjectDraw[i].Draw();
	}
}

void SquareBlock::EndDraw()
{
	for (int i = 0; i < _countof(modelBox); i++) {
		instancingObjectDraw[i].EndDraw();
	}
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
