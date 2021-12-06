#include "Block.h"
#include "GameUtility.h"

void Block::SetBlockPos(float x, float z)
{
	object.SetPosition(x, ONE_CELL_LENGTH / 2, z);
}