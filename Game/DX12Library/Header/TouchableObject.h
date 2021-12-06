#pragma once
#include "Object3D.h"

class TouchableObject :
    public Object3D
{
public:
	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="fbxModel">FBXモデル</param>
	void SetObjModel(ObjModel* objModel) override;
};

