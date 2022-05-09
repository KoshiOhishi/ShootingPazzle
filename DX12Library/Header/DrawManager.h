#pragma once
#include <vector>

namespace DX12Library {

	enum DrawManagerObjectType {
		DRAW_MANAGER_OBJECT_TYPE_OBJECT3D,
		DRAW_MANAGER_OBJECT_TYPE_LINE3D,
		DRAW_MANAGER_OBJECT_TYPE_SPRITE,
		DRAW_MANAGER_OBJECT_TYPE_RENDERTEXT,
		DRAW_MANAGER_OBJECT_TYPE_PARTICLE3D,
		DRAW_MANAGER_OBJECT_TYPE_PARTICLE2D,
	};

	class DrawManager
	{
	public:
		//描画オブジェクト構造体
		struct DrawObject
		{
			int objectType;
			void* pObject;
		};

	private:
		static std::vector<DrawObject> drawListBG;
		static std::vector<DrawObject> drawList;

	public:
		static void AddDrawListBG(int objectType, void* drawObject);
		static void AddDrawList(int objectType, void* drawObject);
		static void DrawAll(bool isBG = false);
		static void DrawAllShadow();
	};
}
