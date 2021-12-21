#include "SquareBlock.h"
#include "GameUtility.h"

ObjModel SquareBlock::modelBox;

void SquareBlock::CreateModel()
{
	//���f������
	modelBox.CreateBox(ONE_CELL_LENGTH, ONE_CELL_LENGTH, ONE_CELL_LENGTH);
}

SquareBlock::~SquareBlock()
{
}

void SquareBlock::Initialize(const StageVec2& pos, float sphereRadius)
{
	//�I�u�W�F�N�g����
	object.Initialize();
	object.SetObjModel(&modelBox);

	//�����̂̎���ɂ���J�v�Z������ (�O�p�`�Ȃ̂�3��)
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
	object.Update();
	UpdateCollision();
}

void SquareBlock::Draw()
{
	object.Draw();
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
	//�����ɔ����G�t�F�N�g�����

	//�c��ڕW�u���b�N�������炷
	pStage->DecrementTargetBlockCount();
}
