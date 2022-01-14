#include "TriangleBlock.h"
#include "GameUtility.h"

ObjModel TriangleBlock::modelTriangle[4];

void TriangleBlock::CreateModel()
{
	//���f������
	modelTriangle[0].CreateFromOBJ(modelDir + "TriangleBlock/TriangleBlock.obj");
	modelTriangle[1].CreateFromOBJ(modelDir + "TriangleBlock_Breakable_1/TriangleBlock_Breakable_1.obj");
	modelTriangle[2].CreateFromOBJ(modelDir + "TriangleBlock_Breakable_2/TriangleBlock_Breakable_2.obj");
	modelTriangle[3].CreateFromOBJ(modelDir + "TriangleBlock_Breakable_3/TriangleBlock_Breakable_3.obj");
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

	SetStagePos(pos);

	UpdateCollision();
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
}

void TriangleBlock::Breakup()
{
	//�����ɔ����G�t�F�N�g�����

	//�c��ڕW�u���b�N�������炷
	pStage->DecrementTargetBlockCount();
}
