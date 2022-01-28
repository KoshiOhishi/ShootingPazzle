#pragma once
#include <Windows.h>
#include <timeapi.h>
#include <chrono>
#pragma comment(lib,"winmm.lib")

class FPSManager
{
private:
	static double FRAME_TIME;
	static std::chrono::steady_clock::time_point start;
	static std::chrono::steady_clock::time_point end;

	static double nowFPS;
	static bool isAdjust;
	static bool isPrintFPS;

public:
	/// <summary>
	/// FPS���Z�b�g(0�ȉ��Ő������Ȃ�)
	/// </summary>
	/// <param name="fps">���삳����fps�B0�ȉ��Ő����Ȃ�</param>
	static void SetFPS(double fps = -1, bool isPrintFPS = false);

	/// <summary>
	/// �v���O�������[�v�̏I���ɋL�q���� fps�̒������s��
	/// </summary>
	static void Update();

	/// <summary>
	/// ���݂�fps���擾
	/// </summary>
	/// <returns></returns>
	static double GetFPS() { return nowFPS; }

	/// <summary>
	/// ����l��60fps�œ��삳���邽�߂ɏ�Z�����鐔
	/// </summary>
	/// <returns></returns>
	static double GetMulAdjust60FPS() { return 60.0 / nowFPS; }
};

