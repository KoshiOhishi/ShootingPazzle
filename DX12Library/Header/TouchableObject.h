#pragma once
#include "Object3D.h"

namespace DX12Library {

	class TouchableObject :
		public Object3D
	{
	public:
		/// <summary>
		/// ���f���̃Z�b�g
		/// </summary>
		/// <param name="fbxModel">FBX���f��</param>
		void SetObjModel(ObjModel* objModel) override;
	};
}
