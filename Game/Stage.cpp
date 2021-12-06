#include "Stage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "GameUtility.h"

Stage::~Stage()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
}

void Stage::LoadStage(std::string filename)
{
	//‚Æ‚è‚ ‚¦‚¸‚Ìˆ—
	width = 20;
	depth = 20;

	TriangleBlock* block = new TriangleBlock;
	block->Initialize(0, 0, ONE_CELL_LENGTH / 2);
	block->SetShapeType(SHAPETYPE_NO_RIGHTBOTTOM);
	blocks.emplace_back(block);

	floor.CreateModel(width * ONE_CELL_LENGTH, depth * ONE_CELL_LENGTH);
	floor.Initialize();
}

void Stage::Update()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Update();
	}
	floor.Update();
}

void Stage::Draw()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Draw();
	}
	floor.Draw();
}
