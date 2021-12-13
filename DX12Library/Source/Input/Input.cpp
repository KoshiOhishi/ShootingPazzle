#include "Input.h"
#include <cassert>
#include "DX12Util.h"
#include <Windows.h>
#include "Matrix4.h"
#include "Quaternion.h"
#include "DebugText.h"

//Input
Microsoft::WRL::ComPtr<IDirectInput8> Input::dinput;

//Keyboard
Microsoft::WRL::ComPtr<IDirectInputDevice8> Keyboard::devkeyboard;
BYTE Keyboard::key[256];
BYTE Keyboard::prevKey[256];

//Mouse
Microsoft::WRL::ComPtr<IDirectInputDevice8> Mouse::devMouse;
DIMOUSESTATE2 Mouse::mouseState;
DIMOUSESTATE2 Mouse::prevMouseState;
Ray Mouse::ray;
Camera* Mouse::pCamera;

void Input::Initialize(HWND hwnd)
{
	HRESULT result;

	//DirectInput �I�u�W�F�N�g�̐���
	dinput = nullptr;
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	Keyboard::Initialize(hwnd, dinput.Get());
	Mouse::Initialize(hwnd, dinput.Get());
}

void Input::Update()
{
	Keyboard::Update();
	Mouse::Update();
}

void Keyboard::Initialize(HWND hwnd, IDirectInput8* dinput)
{
	HRESULT result;

	//�L�[�{�[�h�f�o�C�X�̐���
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	if (FAILED(result)) {
		assert(0);
	}

	//���̓f�[�^�`���̃Z�b�g
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard); //�W���`��
	if (FAILED(result)) {
		assert(0);
	}

	//�r�����䃌�x���̃Z�b�g
	result = devkeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
	}
}

void Keyboard::Update()
{
	HRESULT result;
	//�L�[�{�[�h���̎擾�J�n
	result = devkeyboard->Acquire();

	//�O��̃L�[���͏����R�s�[
	for (int i = 0; i < 256; i++)
	{
		prevKey[i] = key[i];
	}

	//�S�L�[�̓��͏�Ԃ��擾����
	result = devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Keyboard::IsKeyTrigger(int keynum)
{
	if (keynum < 0x00) return false;
	if (keynum > 0xff) return false;

	if (!prevKey[keynum] && key[keynum])
		return true;

	return false;
}

bool Keyboard::IsKeyPush(int keynum)
{
	if (keynum < 0x00) return false;
	if (keynum > 0xff) return false;

	if (key[keynum])
		return true;

	return false;
}

Vector3 Mouse::CalcPosScreenToWorld(float fZ)
{
	//�t�s����Z�o
	Matrix4 invView = DirectX::XMMatrixInverse(nullptr, pCamera->GetViewMatrix());
	Matrix4 invPrj = DirectX::XMMatrixInverse(nullptr, pCamera->GetProjectionMatrix());
	Matrix4 invVp = DirectX::XMMatrixIdentity();
	invVp.r[0].m128_f32[0] = (float)DX12Util::GetWindowWidth() * 0.5f;
	invVp.r[1].m128_f32[1] = (float)-DX12Util::GetWindowHeight() * 0.5f;
	invVp.r[3].m128_f32[0] = (float)DX12Util::GetWindowWidth() * 0.5f;
	invVp.r[3].m128_f32[1] = (float)DX12Util::GetWindowHeight() * 0.5f;
	invVp = DirectX::XMMatrixInverse(nullptr, invVp);

	//�X�N���[�����W�ɋt�s����|����
	Vector3 pos = { GetMousePos().x, GetMousePos().y, fZ};
	pos = Transform(pos, invVp);
	pos = Transform(pos, invPrj);
	//�����ŃJ������w�l(�j�A�[)�ŏ��Z
	pos /= Camera::CAMERA_NEAR;
	pos = Transform(pos, invView);

	return pos;
}

void Mouse::Initialize(HWND hwnd, IDirectInput8* dinput)
{
	HRESULT result;

	// �}�E�X�f�o�C�X�̐���	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	if (FAILED(result)) {
		assert(0);
	}

	// ���̓f�[�^�`���̃Z�b�g
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // �W���`��
	if (FAILED(result)) {
		assert(0);
	}

	// �r�����䃌�x���̃Z�b�g
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
	}
}

void Mouse::Update()
{
	HRESULT result;

	result = devMouse->Acquire();	// �}�E�X����J�n

// �O��̃}�E�X���͏����R�s�[
	prevMouseState = mouseState;

	// �}�E�X�̓��͏�Ԃ��擾����
	result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);

	//���C�X�V
	if (pCamera) {
		//�}�E�X�̃X�N���[�����W�����[���h���W�ɕϊ�
		//��ԋ߂��_
		Vector3 nearPos = CalcPosScreenToWorld(0);
		//��ԉ����_
		Vector3 farPos = CalcPosScreenToWorld(1);

		//�}�E�X�̈ʒu�͈�ԋ߂��_
		ray.start = nearPos;

		//���C�̃x�N�g�����v�Z
		ray.dir = farPos - nearPos;
		ray.dir = ray.dir.Normalize();
	}
}

bool Mouse::IsMouseButtonTrigger(int mouseButtonNum)
{
	if (!prevMouseState.rgbButtons[mouseButtonNum] && mouseState.rgbButtons[mouseButtonNum]) {
		return true;
	}

	return false;
}

bool Mouse::IsMouseButtonRelease(int mouseButtonNum)
{
	if (prevMouseState.rgbButtons[mouseButtonNum] && !mouseState.rgbButtons[mouseButtonNum]) {
		return true;
	}

	return false;
}

bool Mouse::IsMouseButtonPush(int mouseButtonNum)
{
	if (mouseState.rgbButtons[mouseButtonNum]) {
		return true;
	}

	return false;
}

Vector2 Mouse::GetMouseMove()
{
	return Vector2((float)mouseState.lX, mouseState.lY);
}

float Mouse::GetMouseWheelMove()
{
	return (float)mouseState.lZ;
}

Vector2 Mouse::GetMousePos(bool isRelative)
{
	POINT p{};
	GetCursorPos(&p);

	//���Έʒu�擾�̏ꍇ�̓E�B���h�E�̈ʒu�����Z
	if (isRelative) {
		WINDOWINFO windowInfo{};
		GetWindowInfo(DX12Util::GetHwnd(), &windowInfo);

		LONG offsetX = 7, offsetY = 30;

		p.x -= (windowInfo.rcWindow.left + offsetX);
		p.y -= (windowInfo.rcWindow.top + offsetY);
	}

	return Vector2(p.x, p.y);
}

void Mouse::SetMousePos(Vector2 pos, bool isRelative)
{
	int posX = (int)pos.x;
	int posY = (int)pos.y;

	//���Έʒu�擾�̏ꍇ�̓E�B���h�E�̈ʒu�����Z
	if (isRelative) {
		WINDOWINFO windowInfo{};
		GetWindowInfo(DX12Util::GetHwnd(), &windowInfo);

		LONG offsetX = 7, offsetY = 30;

		posX += (windowInfo.rcWindow.left + offsetX);
		posY += (windowInfo.rcWindow.top + offsetY);
	}

	SetCursorPos(posX, posY);
}
