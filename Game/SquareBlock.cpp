#include "SquareBlock.h"
#include "GameUtility.h"
#include "GameSound.h"
#include "FPSManager.h"

ObjModel SquareBlock::modelBox[4];
Particle3D SquareBlock::particle[5];

void SquareBlock::StaticInitialize()
{
	//���f������
	modelBox[0].CreateFromOBJ(modelDir + "SquareBlock/SquareBlock.obj");
	modelBox[1].CreateFromOBJ(modelDir + "SquareBlock_Breakable_1/SquareBlock_Breakable_1.obj");
	modelBox[2].CreateFromOBJ(modelDir + "SquareBlock_Breakable_2/SquareBlock_Breakable_2.obj");
	modelBox[3].CreateFromOBJ(modelDir + "SquareBlock_Breakable_3/SquareBlock_Breakable_3.obj");

	//�p�[�e�B�N��
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].LoadTexture(L"Resources/Particle/Square.png");
		particle[i].Initialize();
		particle[i].SetColor(GameUtility::COLOR_VALUE[i]);
		particle[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}
}

void SquareBlock::GenerateParticle(int num, const Vector3& pos, int colorIndex)
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

void SquareBlock::DrawParticle()
{
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Update();
		particle[i].Draw();
	}
}

void SquareBlock::Initialize(const StageVec2& pos, float sphereRadius)
{
	//�I�u�W�F�N�g����
	object.Initialize();

	//�����̂̎���ɂ���J�v�Z������ (�O�p�`�Ȃ̂�3��)
	capsule.clear();
	for (int i = 0; i < 4; i++) {
		capsule.push_back({});
		capsule[i].radius = sphereRadius;
	}

	//�I�u�W�F�N�g�ʒu�Z�b�g
	SetStagePos(pos);

	//�R���W�����Z�b�g
	UpdateCollision();

	//�F�ύX�^�C�}�[������
	changeColorTimer.SetTimer(0, 1000);

	objectName = "SquareBlock";
}

void SquareBlock::Update()
{
	UpdateColor();
	object.Update(pIOD[breakupCount]);
	UpdateCollision();
}

void SquareBlock::UpdateCollision()
{
	const Vector3& position = object.GetPosition();
	Vector3 topLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//����
	Vector3 topRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//�E��
	Vector3 bottomLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//����O
	Vector3 bottomRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//�E��O

	//�J�v�Z���̒l�X�V
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

void SquareBlock::Breakup()
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