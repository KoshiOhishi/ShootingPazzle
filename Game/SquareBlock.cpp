#include "SquareBlock.h"
#include "GameUtility.h"

ObjModel SquareBlock::modelBox[4];

void SquareBlock::CreateModel()
{
	//���f������
	modelBox[0].CreateFromOBJ(modelDir + "SquareBlock/SquareBlock.obj");
	modelBox[1].CreateFromOBJ(modelDir + "SquareBlock_Breakable_1/SquareBlock_Breakable_1.obj");
	modelBox[2].CreateFromOBJ(modelDir + "SquareBlock_Breakable_2/SquareBlock_Breakable_2.obj");
	modelBox[3].CreateFromOBJ(modelDir + "SquareBlock_Breakable_3/SquareBlock_Breakable_3.obj");
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

	SetStagePos(pos);

	UpdateCollision();

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
}

void SquareBlock::Breakup()
{
	//�����ɔ����G�t�F�N�g�����

	//�c��ڕW�u���b�N�������炷
	pStage->DecrementTargetBlockCount();
}
