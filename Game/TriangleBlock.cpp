#include "TriangleBlock.h"
#include "GameUtility.h"
#include "GameSound.h"
#include "FPSManager.h"

ObjModel TriangleBlock::modelTriangle[4];
Particle3D TriangleBlock::particle[5];

void TriangleBlock::StaticInitialize()
{
	//モデル生成
	modelTriangle[0].CreateFromOBJ(MODEL_DIR + "TriangleBlock/TriangleBlock.obj");
	modelTriangle[1].CreateFromOBJ(MODEL_DIR + "TriangleBlock_Breakable_1/TriangleBlock_Breakable_1.obj");
	modelTriangle[2].CreateFromOBJ(MODEL_DIR + "TriangleBlock_Breakable_2/TriangleBlock_Breakable_2.obj");
	modelTriangle[3].CreateFromOBJ(MODEL_DIR + "TriangleBlock_Breakable_3/TriangleBlock_Breakable_3.obj");

	//パーティクル
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].LoadTexture(TEX_DIR_UTIL + L"Particle/Triangle.png");
		particle[i].Initialize();
		particle[i].SetColor(GameUtility::COLOR_VALUE[i]);
		particle[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}
}

void TriangleBlock::GenerateParticle(int num, const Vector3& pos, int colorIndex)
{
	Vector3 generatePos = pos;
	for (int i = 0; i < num; i++) {
		//上方向にランダムで飛ばす
		float x = (float)((rand() % 200 - 100) * 0.01f);
		float y = (float)((rand() % 100) * 0.01f);
		float z = (float)((rand() % 200 - 100) * 0.01f);
		Vector3 vel = Vector3(x, y, z).Normalize() * 0.75f * FPSManager::GetMulAdjust60FPS();
		Vector3 acc = Vector3(0, -0.01f, 0) * FPSManager::GetMulAdjust60FPS();;
		float startScale = 6.0f;
		float endScale = 0;

		particle[colorIndex].Add(1000, generatePos, vel, acc, startScale, endScale);
	}
}

void TriangleBlock::DrawParticle()
{
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Update();
		particle[i].Draw();
	}
}

void TriangleBlock::Initialize(const StageVec2& pos, float sphereRadius)
{
	//オブジェクト生成
	object.Initialize();

	/*上から見た元の形
	＿＿＿
	|　　／
	|　／
	|／

	*/

	//直方体の周りにつけるカプセル判定 (三角形なので3つ)
	capsule.clear();
	for (int i = 0; i < 3; i++) {
		capsule.push_back({});
		capsule[i].radius = sphereRadius;
	}

	//オブジェクト位置セット
	SetStagePos(pos);

	//コリジョンセット
	UpdateCollision();

	//色変更タイマー初期化
	changeColorTimer.SetTimer(0, 1000);
}

void TriangleBlock::SetTriangleType(int shapeType)
{
	this->triangleType = shapeType;

	//タイプによって回転
	switch (this->triangleType) {
	case TRIANGLETYPE_NO_LEFTTOP:
		object.SetRotation({ 0, 180, 0 });
		objectName = "TriangleBlock_No_LeftTop";
		break;
	case TRIANGLETYPE_NO_RIGHTTOP:
		object.SetRotation({ 0, 270, 0 });
		objectName = "TriangleBlock_No_RightTop";
		break;
	case TRIANGLETYPE_NO_LEFTBOTTOM:
		object.SetRotation({ 0, 90, 0 });
		objectName = "TriangleBlock_No_LeftBottom";
		break;
	case TRIANGLETYPE_NO_RIGHTBOTTOM:
		//そのまま
		objectName = "TriangleBlock_No_RightBottom";
		break;
	}

	UpdateCollision();
}

void TriangleBlock::Update()
{
	UpdateColor();
	object.Update(pIOD[breakupCount]);
	UpdateCollision();
}

void TriangleBlock::UpdateCollision()
{
	const Vector3& position = object.GetPosition();
	Vector3 topLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//左奥
	Vector3 topRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//右奥
	Vector3 bottomLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//左手前
	Vector3 bottomRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//右手前

	//カプセルの値更新
	switch (this->triangleType) {
	case TRIANGLETYPE_NO_LEFTTOP:
		capsule[0].start = topRight;
		capsule[0].end = bottomRight;

		capsule[1].start = bottomRight;
		capsule[1].end = bottomLeft;

		capsule[2].start = bottomLeft;
		capsule[2].end = topRight;
		break;

	case TRIANGLETYPE_NO_RIGHTTOP:
		capsule[0].start = topLeft;
		capsule[0].end = bottomRight;

		capsule[1].start = bottomRight;
		capsule[1].end = bottomLeft;

		capsule[2].start = bottomLeft;
		capsule[2].end = topLeft;
		break;

	case TRIANGLETYPE_NO_LEFTBOTTOM:
		capsule[0].start = topLeft;
		capsule[0].end = topRight;

		capsule[1].start = topRight;
		capsule[1].end = bottomRight;

		capsule[2].start = bottomRight;
		capsule[2].end = topLeft;
		break;

	case TRIANGLETYPE_NO_RIGHTBOTTOM:
		capsule[0].start = topLeft;
		capsule[0].end = topRight;

		capsule[1].start = topRight;
		capsule[1].end = bottomLeft;

		capsule[2].start = bottomLeft;
		capsule[2].end = topLeft;
		break;
	}
}

void TriangleBlock::DecrementBreakupCount()
{
	if (isBreakable == false || breakupCount == 0) {
		return;
	}

	breakupCount--;
	if (breakupCount <= 0) {
		Breakup();
	}
	else {
		//パーティクルを発生させる
		GenerateParticle(3, object.GetPosition(), blockColor);

		//効果音再生
		GameSound::Play(L"Crack", object.GetPosition());
	}
}

void TriangleBlock::Breakup()
{
	//パーティクルを発生させる
	GenerateParticle(10, object.GetPosition(), blockColor);

	//効果音再生
	GameSound::Play(L"BreakBlock", object.GetPosition());

#ifdef BUILD_GAME
	//残り目標ブロック数を減らす
	pStage->DecrementTargetBlockCount();
#endif // BUILD_GAME
}
