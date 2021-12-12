#include "NormalFloor.h"
#include "GameUtility.h"

ObjModel NormalFloor::modelBox;

void NormalFloor::CreateModel()
{
    //���f������
    modelBox.CreateFromOBJ("NormalFloor");
}

void NormalFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();
	object.SetObjModel(&modelBox);

	//���������Ȃ��̂Ń��f�����܂ŐF�ς���
	object.SetColor(0.5f, 0.5f, 0.5f, 1);

	SetStagePos(pos);

	UpdateCollision();
}

void NormalFloor::Update()
{
	object.Update();
	UpdateCollision();
}

void NormalFloor::Draw()
{
	object.Draw();
}

void NormalFloor::UpdateCollision()
{
}