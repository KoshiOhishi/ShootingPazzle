#include "TouchableObject.h"
#include "MeshCollider.h"

using namespace DX12Library;

void TouchableObject::SetObjModel(ObjModel* objModel)
{
	Object3D::SetObjModel(objModel);

	//コライダーの追加
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	collider->ConstructTriangles(objModel);
}
