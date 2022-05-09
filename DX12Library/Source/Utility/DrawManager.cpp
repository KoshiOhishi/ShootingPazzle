#include "DrawManager.h"
#include "Sprite.h"
#include "RenderText.h"
#include "Particle3D.h"
#include "Particle2D.h"
#include "Object3D.h"
#include "Line3D.h"

using namespace DX12Library;

std::vector<DrawManager::DrawObject> DrawManager::drawListBG;
std::vector<DrawManager::DrawObject> DrawManager::drawList;

void DrawManager::AddDrawListBG(int objectType, void* drawObject)
{
	DrawObject l_drawObject = { objectType, drawObject };
	drawListBG.emplace_back(l_drawObject);
}

void DrawManager::AddDrawList(int objectType, void* drawObject)
{
	DrawObject l_drawObject = { objectType, drawObject };
	drawList.emplace_back(l_drawObject);
}

void DrawManager::DrawAll(bool isBG)
{
	std::vector<DrawManager::DrawObject>* l_drawList = nullptr;
	if (isBG) { l_drawList = &drawListBG; }
	else { l_drawList = &drawList; }
	int prevDrawType = -1;

	if (isBG == false) {
		Line3D::BeginDraw();
		Line3D::DrawAll();
		Line3D::Reset();
		prevDrawType = DRAW_MANAGER_OBJECT_TYPE_LINE3D;
	}

	for (auto& v : *l_drawList) {
		if (v.objectType == DRAW_MANAGER_OBJECT_TYPE_OBJECT3D) {
			//BeginDraw���ĂԂ��ǂ���
			if (prevDrawType != v.objectType) {
				Object3D::BeginDraw();
			}
			//Object3D*�ɃL���X�g
			Object3D* p = static_cast<Object3D*>(v.pObject);
			p->DrawPrivate();
		}
		else if (v.objectType == DRAW_MANAGER_OBJECT_TYPE_SPRITE) {
			//BeginDraw���ĂԂ��ǂ���
			if (prevDrawType != v.objectType) {
				Sprite::BeginDraw();
			}
			//Sprite*�ɃL���X�g
			Sprite* p = static_cast<Sprite*>(v.pObject);
			p->DrawPrivate();
		}
		else if (v.objectType == DRAW_MANAGER_OBJECT_TYPE_RENDERTEXT) {
			//BeginDraw���ĂԂ��ǂ���
			if (prevDrawType != v.objectType) {
				RenderText::BeginDraw();
			}
			//RenderText*�ɃL���X�g
			RenderText* p = static_cast<RenderText*>(v.pObject);
			p->DrawStringPrivate();
		}
		else if (v.objectType == DRAW_MANAGER_OBJECT_TYPE_PARTICLE3D) {
			//BeginDraw���ĂԂ��ǂ���
			if (prevDrawType != v.objectType) {
				Particle3D::BeginDraw();
			}
			//Particle3D*�ɃL���X�g
			Particle3D* p = static_cast<Particle3D*>(v.pObject);
			p->DrawPrivate();
		}
		else if (v.objectType == DRAW_MANAGER_OBJECT_TYPE_PARTICLE2D) {
			//BeginDraw���ĂԂ��ǂ���
			if (prevDrawType != v.objectType) {
				Particle2D::BeginDraw();
			}
			//Particle2D*�ɃL���X�g
			Particle2D* p = static_cast<Particle2D*>(v.pObject);
			p->DrawPrivate();
		}

		prevDrawType = v.objectType;
	}

	l_drawList->clear();
}

void DrawManager::DrawAllShadow()
{
	//�[�x�l���e�N�X�`���ɏ�������
	Object3D::BeginDrawShadow();
	for (auto& v : drawList) {
		if (v.objectType == DRAW_MANAGER_OBJECT_TYPE_OBJECT3D) {
			//Object3D*�ɃL���X�g
			Object3D* p = static_cast<Object3D*>(v.pObject);
			p->DrawShadow();
		}
	}
}
