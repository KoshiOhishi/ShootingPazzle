#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Block.h"

#define PI 3.14159265359f
//1�}�X�̒���
#define ONE_CELL_LENGTH (float)5.0f

//�Q�[���t�F�[�Y
enum Phase
{
	PHASE_SETPOS,
	PHASE_SETANGLE,
	PHASE_AFTERSHOOT
};

class GameUtility
{

private:
	//���݂̃t�F�[�Y
	static int nowPhase;

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	static void SetNowPhase(int phase) { nowPhase = phase; }
	static int GetNowPhase() { return nowPhase; }
};