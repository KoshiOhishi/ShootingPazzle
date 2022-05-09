#pragma once
#include "Object3D.h"

namespace DX12Library {

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
}
