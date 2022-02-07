#include "BreakFloor.h"
#include "GameUtility.h"
#include "GameSound.h"
#include "FPSManager.h"

ObjModel BreakFloor::modelBox;
Particle3D BreakFloor::particle;

void BreakFloor::StaticInitialize()
{
	//���f������
	modelBox.CreateFromOBJ(MODEL_DIR + "BreakFloor/BreakFloor.obj");

	//�p�[�e�B�N��
	particle.LoadTexture(TEX_DIR_UTIL + L"/Particle/Break.png");
	particle.Initialize();
	particle.SetColor({ 0.71f,0.47f, 0.2f, 1 });
	particle.SetBlendMode(PARTICLE_BLENDMODE_ADD);
}

void BreakFloor::GenerateParticle(const Vector3& pos)
{
	Vector3 generatePos = pos;
	for (int i = 0; i < 5; i++) {
		//������Ƀ����_���Ŕ�΂��i�򋗗��͍T���߁j
		float x = (float)((rand() % 200 - 100) * 0.01f);
		float y = (float)((rand() % 100) * 0.01f);
		float z = (float)((rand() % 200 - 100) * 0.01f);
		Vector3 vel = Vector3(x, y, z).Normalize() * 0.25f * FPSManager::GetMulAdjust60FPS();
		Vector3 acc =  Vector3(0, -0.01f, 0) * FPSManager::GetMulAdjust60FPS();;
		float startScale = 4.0f;
		float endScale = 0;

		particle.Add(1000, generatePos, vel, acc, startScale, endScale);
	}
}

void BreakFloor::DrawParticle()
{
	particle.Update();
	particle.Draw();
}

void BreakFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();

	isBreak = false;
}

void BreakFloor::Update()
{
	//���������̏ォ�痣�ꂽ��j��
	{
		if (onFloor == false && prevOnFloor == true) {
			Break();
		}
		prevOnFloor = onFloor;
	}

	if (isBreak == false) {
		object.Update(*pIOD);
	}
	UpdateCollision();
}

void BreakFloor::UpdateCollision()
{
}

int BreakFloor::GetFloorType()
{
	if (isBreak) {
		//�j���͌��Ƃ݂Ȃ�
		return FLOORTYPE_HOLE;
	}
	else {
		return FLOORTYPE_BREAK;
	}
}

void BreakFloor::Break()
{
#ifdef BUILD_GAME
	//�p�[�e�B�N���𔭐�������
	GenerateParticle(object.GetPosition());

	//���ʉ��Đ�
	GameSound::Play(L"BreakFloor", object.GetPosition());

#endif // BUILD_GAME

	isBreak = true;
}
