#include "GameUtility.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"

//���݂̃t�F�[�Y
int GameUtility::nowPhase;


void GameUtility::StaticInitialize()
{
	MyBullet::CreateModel();
	TriangleBlock::CreateModel();
	SquareBlock::CreateModel();
}