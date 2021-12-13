#include "TouchableObject.h"
#include "MeshCollider.h"

void TouchableObject::SetObjModel(ObjModel* objModel)
{
	Object3D::SetObjModel(objModel);

	//�R���C�_�[�̒ǉ�
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	collider->ConstructTriangles(objModel);
}
