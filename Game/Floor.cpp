#include "Floor.h"

void Floor::CreateModel(float width, float depth)
{
	model.CreateSquare(width, depth, { 0.7f, 0.7f, 0.7f });
}

void Floor::Initialize()
{
	Object3D::Initialize();
	Object3D::SetObjModel(&model);
	//���Ȃ̂�90�x��]
	Object3D::SetRotation(90, 0, 0);

	//�����蔻�菉����
	plane.distance = 0;
	plane.normal = { 0,1,0 };	//�@�������͏�
}
