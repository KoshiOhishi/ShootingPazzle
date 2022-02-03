#include "TriangleBlock.h"
#include "GameUtility.h"
#include "GameSound.h"
#include "FPSManager.h"

ObjModel TriangleBlock::modelTriangle[4];
Particle3D TriangleBlock::particle[5];

void TriangleBlock::StaticInitialize()
{
	//���f������
	modelTriangle[0].CreateFromOBJ(MODEL_DIR + "TriangleBlock/TriangleBlock.obj");
	modelTriangle[1].CreateFromOBJ(MODEL_DIR + "TriangleBlock_Breakable_1/TriangleBlock_Breakable_1.obj");
	modelTriangle[2].CreateFromOBJ(MODEL_DIR + "TriangleBlock_Breakable_2/TriangleBlock_Breakable_2.obj");
	modelTriangle[3].CreateFromOBJ(MODEL_DIR + "TriangleBlock_Breakable_3/TriangleBlock_Breakable_3.obj");

	//�p�[�e�B�N��
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
		//������Ƀ����_���Ŕ�΂�
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
	//�I�u�W�F�N�g����
	object.Initialize();

	/*�ォ�猩�����̌`
	�Q�Q�Q
	|�@�@�^
	|�@�^
	|�^

	*/

	//�����̂̎���ɂ���J�v�Z������ (�O�p�`�Ȃ̂�3��)
	capsule.clear();
	for (int i = 0; i < 3; i++) {
		capsule.push_back({});
		capsule[i].radius = sphereRadius;
	}

	//�I�u�W�F�N�g�ʒu�Z�b�g
	SetStagePos(pos);

	//�R���W�����Z�b�g
	UpdateCollision();

	//�F�ύX�^�C�}�[������
	changeColorTimer.SetTimer(0, 1000);
}

void TriangleBlock::SetTriangleType(int shapeType)
{
	this->triangleType = shapeType;

	//�^�C�v�ɂ���ĉ�]
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
		//���̂܂�
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
	Vector3 topLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//����
	Vector3 topRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//�E��
	Vector3 bottomLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//����O
	Vector3 bottomRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//�E��O

	//�J�v�Z���̒l�X�V
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
		//�p�[�e�B�N���𔭐�������
		GenerateParticle(3, object.GetPosition(), blockColor);

		//���ʉ��Đ�
		GameSound::Play(L"Crack", object.GetPosition());
	}
}

void TriangleBlock::Breakup()
{
	//�p�[�e�B�N���𔭐�������
	GenerateParticle(10, object.GetPosition(), blockColor);

	//���ʉ��Đ�
	GameSound::Play(L"BreakBlock", object.GetPosition());

#ifdef BUILD_GAME
	//�c��ڕW�u���b�N�������炷
	pStage->DecrementTargetBlockCount();
#endif // BUILD_GAME
}
