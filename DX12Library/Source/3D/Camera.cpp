#include "Camera.h"
#include "DX12Util.h"
#include "Quaternion.h"

using namespace DirectX;

const float Camera::CAMERA_NEAR = 0.1f;
const float Camera::CAMERA_FAR = 1000.0f;
const float Camera::CAMERA_VIEWING_ANGLE = 60.0f;

void Camera::Initialize()
{
	//初期値代入
	matProjection = DirectX::XMMatrixIdentity();
	matView = DirectX::XMMatrixIdentity();
	matRotation = DirectX::XMMatrixIdentity();
	matBillboard = DirectX::XMMatrixIdentity();
	matBillboardY = DirectX::XMMatrixIdentity();

	eye = { 0,0,-100 };
	target = { 0, 0, 0 };
	up = { 0, 1, 0 };

	//射影行列の作成
	//透視投影
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(CAMERA_VIEWING_ANGLE),
		(float)DX12Util::GetWindowWidth() / DX12Util::GetWindowHeight(),
		CAMERA_NEAR,
		CAMERA_FAR
	);

	UpdateViewMatrix();
}

void Camera::Update()
{
	//値の変更があった時だけ行列更新
	if (dirty)
	{
		UpdateViewMatrix();
		dirty = false;
	}
}

void Camera::AddEye(const float eyeX, const float eyeY, const float eyeZ)
{
	this->eye.x += eyeX;
	this->eye.y += eyeY;
	this->eye.z += eyeZ;
	dirty = true;
}

void Camera::AddTarget(const float targetX, const float targetY, const float targetZ)
{
	this->target.x += targetX;
	this->target.y += targetY;
	this->target.z += targetZ;
	dirty = true;
}

void Camera::AddUp(const float upX, const float upY, const float upZ)
{
	this->up.x += upX;
	this->up.y += upY;
	this->up.z += upZ;
	dirty = true;
}

void Camera::MoveCamera(const Vector3& vector, bool isAutoDirAxisX, bool isAutoDirAxisY, bool isAutoDirAxisZ)
{
	//カメラの回転量をラジアンにする
	Vector3 rad = {
		(rotation.x) * 3.14159265359f / 180,
		(rotation.y) * 3.14159265359f / 180,
		(rotation.z) * 3.14159265359f / 180 };

	Quaternion qx, qy, qz, qq;

	//移動方向ベクトルの回転
	//X軸基準回転
	if (isAutoDirAxisX) {
		qx = Quaternion::CreateQuaternion({ 1,0,0 }, rad.x);
	}
	else {
		qx = Quaternion::CreateQuaternion(0, 0, 0, 1);
	}
	qq = Quaternion::CreateQuaternion({ vector.x, vector.y, vector.z }, qx);

	//Y軸基準回転
	if (isAutoDirAxisY) {
		qy = Quaternion::CreateQuaternion({ 0,1,0 }, rad.y);
	}
	else {
		qy = Quaternion::CreateQuaternion(0, 0, 0, 1);
	}
	qq = Quaternion::CreateQuaternion({ qq.x, qq.y, qq.z }, qy);

	//Z軸基準回転
	if (isAutoDirAxisZ) {
		qz = Quaternion::CreateQuaternion({ 0,0,1 }, rad.z);
	}
	else {
		qz = Quaternion::CreateQuaternion(0, 0, 0, 1);
	}
	qq = Quaternion::CreateQuaternion({ qq.x, qq.y, qq.z }, qz);

	//Vector3に変換
	Vector3 dir = { qq.x, qq.y, qq.z };

	//移動
	AddEye(dir.x, dir.y, dir.z);
	AddTarget(dir.x, dir.y, dir.z);
}

void Camera::MoveCamera(const float x, const float y, const float z, bool isAutoDirAxisX, bool isAutoDirAxisY, bool isAutoDirAxisZ)
{
	Vector3 vector = { x,y,z };
	MoveCamera(vector, isAutoDirAxisX, isAutoDirAxisY, isAutoDirAxisZ);
}

void Camera::RotateCamera(const Vector3& axis, const float digrees)
{
	//ラジアンに変換
	float rad = digrees * 3.14159265359f / 180;

	//回転用クォータニオン生成
	Quaternion q(Vector3(axis.x, axis.y, axis.z), rad);

	//回転行列を生成
	XMMATRIX matRot = Quaternion::Rotate(q);

	matRotation *= matRot;

	dirty = true;
}

void Camera::UpdateViewMatrix()
{
	//視点座標
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	//注視点座標
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	//(仮の)上方向
	XMVECTOR upVector = XMLoadFloat3(&up);

	matRotation = XMMatrixIdentity();

	RotateCamera({ 0,0,1 }, rotation.z);
	RotateCamera({ 1,0,0 }, rotation.x);
	RotateCamera({ 0,1,0 }, rotation.y);

	//転置により逆行列(逆回転)を計算
	matView = XMMatrixTranspose(matRotation);


	//平行移動の逆を求める
	//視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//カメラの位置からワールド原点へのベクトル(カメラ座標系)
	XMVECTOR tX = XMVector3Dot(matRotation.r[0], reverseEyePosition);	//X成分
	XMVECTOR tY = XMVector3Dot(matRotation.r[1], reverseEyePosition);	//Y成分
	XMVECTOR tZ = XMVector3Dot(matRotation.r[2], reverseEyePosition);	//Z成分
	//一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//ビュー行列に平行移動成分を設定
	matView.r[3] = translation;

#pragma region 全方向ビルボード行列の計算
	//ビルボード行列
	matBillboard.r[0] = matRotation.r[0];
	matBillboard.r[1] = matRotation.r[1];
	matBillboard.r[2] = matRotation.r[2];
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion

#pragma region Y軸回りビルボード行列の計算
	//カメラX軸、Y軸、Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//X軸は共通
	ybillCameraAxisX = matRotation.r[0];
	//Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);
	//Z軸はX軸⇒Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	//Y軸回りビルボード行列
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma endregion
}

const XMMATRIX& Camera::GetViewProjection()const
{
	XMMATRIX tmp = matView * matProjection;
	return tmp;
}

const Vector3 Camera::GetUpVec() const
{
	Vector3 tmp = { 0,1,0 };

	Quaternion q1(matRotation);
	Quaternion q2(tmp, q1);

	return Vector3{ q2.x,q2.y,q2.z };
}

const float Camera::GetDistance()const
{
	XMVECTOR sub = XMLoadFloat3(&target) - XMLoadFloat3(&eye);
	XMVECTOR t = XMVector3Length(sub);
	return t.m128_f32[0];
}

const Vector3 Camera::GetCameraDir()const
{
	Vector3 tmp = { 0,0,1 };
	
	Quaternion q1(matRotation);
	Quaternion q2(tmp, q1);

	return Vector3{ q2.x,q2.y,q2.z };
}

void Camera::SetEye(const Vector3& eye)
{
	this->eye = eye;
	dirty = true;
}
void Camera::SetEye(const float x, const float y, const float z)
{
	this->eye.x = x;
	this->eye.y = y;
	this->eye.z = z;
	dirty = true;
}
//注視点座標をセット
void Camera::SetTarget(const Vector3& target)
{
	this->target = target;
	dirty = true;
}
void Camera::SetTarget(const float x, const float y, const float z)
{
	this->target.x = x;
	this->target.y = y;
	this->target.z = z;
	dirty = true;
}
//上方向ベクトルをセット
void Camera::SetUpVec(const Vector3& up)
{
	this->up = up;
	dirty = true;
}
void Camera::SetUpVec(const float x, const float y, const float z)
{
	this->up.x = x;
	this->up.y = y;
	this->up.z = z;
	dirty = true;
}
//視点座標、注視点座標、上方向ベクトルをセット
void Camera::SetEyeTargetUpVec(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	this->eye = eye;
	this->target = target;
	this->up = up;
	dirty = true;
}
void Camera::SetEyeTargetUpVec(const float eyeX, const float eyeY, const float eyeZ,
	const float targetX, const float targetY, const float targetZ,
	const float upX, const float upY, const float upZ)
{
	this->SetEye(eyeX, eyeY, eyeZ);
	this->SetTarget(targetX, targetY, targetZ);
	this->SetUpVec(upX, upY, upZ);
	dirty = true;
}

void Camera::SetPositionAndDistance(const Vector3& pos, const float distance)
{
	SetEye(pos);
	SetDistance(distance);
	dirty = true;
}

void Camera::SetPositionAndDistance(const float posX, const float posY, const float posZ, const float distance)
{
	SetEye(posX, posY, posZ);
	SetDistance(distance);
	dirty = true;
}

void Camera::SetPosition(const Vector3& pos)
{
	SetEye(pos);
	dirty = true;
}

void Camera::SetPosition(const float x, const float y, const float z)
{
	SetEye({ x,y,z });
	dirty = true;
}

void Camera::SetDistance(const float distance)
{
	//視点座標からdistanceだけ前に注視点座標を配置
	Vector3 newTarget = { eye.x, eye.y, eye.z + distance };
	SetTarget(newTarget);
	dirty = true;
}

void Camera::SetRotation(const Vector3& vector)
{
	rotation.x = vector.x;
	rotation.y = vector.y;
	rotation.z = vector.z;

	dirty = true;
}

void Camera::SetRotation(const float pitch, const float yaw, const float roll)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;

	dirty = true;
}
