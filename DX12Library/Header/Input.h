#pragma once
#define DIRECTINPUT_VERSION	0x0800	//DirectInput�̃o�[�W�����w��

#include <Windows.h>
#include <wrl.h>
#include "Vector2.h"
#include "Vector3.h"
#include <dinput.h>
#include "CollisionPrimitive.h"
#include "Camera.h"

enum MouseButton
{
	LEFT,
	RIGHT,
	MIDDLE
};

class Input
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static ComPtr<IDirectInput8> dinput;

public:
	static void Initialize(HWND hwnd);
	static void Update();
};

class Keyboard
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static BYTE key[256];
	static BYTE prevKey[256];
	static ComPtr<IDirectInputDevice8> devkeyboard;

public:
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="hwnd"></param>
	static void Initialize(HWND hwnd, IDirectInput8* dinput);

	/// <summary>
	/// �X�V
	/// </summary>
	static void Update();

	/// <summary>
	/// �L�[�����󋵎擾(�g���K�[)
	/// </summary>
	/// <param name="keynum">�L�[�ԍ�(DIK_)</param>
	/// <returns>�L�[������</returns>
	static bool IsKeyTrigger(int keynum);

	/// <summary>
	/// �L�[�����󋵎擾
	/// </summary>
	/// <param name="keynum">�L�[�ԍ�(DIK_)</param>
	/// <returns>�L�[������</returns>
	static bool IsKeyPush(int keynum);

};

class Mouse
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static DIMOUSESTATE2 mouseState;
	static DIMOUSESTATE2 prevMouseState;
	static ComPtr<IDirectInputDevice8> devMouse;
	static Ray ray;
	static Camera* pCamera;

private:
	/// <summary>
	/// �X�N���[�����W���烏�[���h���W�ɕϊ�����
	/// </summary>
	/// <param name="fZ"></param>
	/// <returns></returns>
	static const Vector3& CalcPosScreenToWorld(float fZ);

public:
	//�����o�֐��̐錾

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="hwnd"></param>
	static void Initialize(HWND hwnd, IDirectInput8* dinput);

	/// <summary>
	/// �X�V
	/// </summary>
	static void Update();

	/// <summary>
	/// �}�E�X�{�^�������󋵎擾(�g���K�[)
	/// </summary>
	/// <param name="mouseButtonNum">�}�E�X�{�^���ԍ�(MouseButton::)</param>
	/// <returns>�}�E�X�{�^��������</returns>
	static bool IsMouseButtonTrigger(int mouseButtonNum);

	/// <summary>
	/// �}�E�X�{�^�������󋵎擾(�����ꂽ�u��)
	/// </summary>
	/// <param name="mouseButtonNum"></param>
	/// <returns>�}�E�X�{�^��������</returns>
	static bool IsMouseButtonRelease(int mouseButtonNum);

	/// <summary>
	/// �}�E�X�{�^�������󋵎擾
	/// </summary>
	/// <param name="mouseButtonNum">�}�E�X�{�^���ԍ�(MouseButton::)</param>
	/// <returns>�}�E�X�{�^��������</returns>
	static bool IsMouseButtonPush(int mouseButtonNum);

	/// <summary>
	/// �}�E�X1�t���[���ړ���
	/// </summary>
	/// <returns>�ړ���</returns>
	static const Vector2& GetMouseMove();

	/// <summary>
	/// �}�E�X1�t���[���z�C�[���ړ���
	/// </summary>
	/// <returns>�z�C�[���ړ���</returns>
	static float GetMouseWheelMove();

	/// <summary>
	/// �}�E�X���W�擾
	/// </summary>
	/// <param name="isRelative">�E�B���h�E�����0�Ƃ��邩</param>
	/// <returns>���W</returns>
	static const Vector2& GetMousePos(bool isRelative = true);

	/// <summary>
	/// �}�E�X���W�Z�b�g
	/// </summary>
	static void SetMousePos(const Vector2& pos, bool isRelative = true);

	/// <summary>
	/// �J�������Z�b�g
	/// </summary>
	/// <param name="pCamera">�J�����|�C���^</param>
	static void SetCamera(Camera* pCamera) { Mouse::pCamera = pCamera; }

	/// <summary>
	/// �}�E�X��3D�I�u�W�F�N�g�̓����蔻��Ɏg�p���郌�C��Ԃ�
	/// </summary>
	/// <returns>���C</returns>
	static const Ray& GetMouseRay() { return ray; }
};
