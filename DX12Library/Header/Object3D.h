#pragma once
#include "FbxLoader.h"
#include "FbxModel.h"
#include "ObjModel.h"
#include "Camera.h"
#include "Light.h"
#include "DX12Util.h"
#include "CollisionInfo.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

class BaseCollider;

//�I�u�W�F�N�g�̎��
enum ObjectType
{
	OBJECTTYPE_OBJ,
	OBJECTTYPE_FBX,
	OBJECTTYPE_INSTANCING_OBJ,
	OBJECTTYPE_INSTANCING_FBX,
};

//�r���{�[�h�^�C�v
enum BillboardType {
	NONE,
	Billboard,
	BillboardY,
};

class Object3D
{
protected: //�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMMATRIX = DirectX::XMMATRIX;
	//std::���ȗ�
	using string = std::string;

	using PipelineData = DX12Util::PipelineData;

public: //�萔
	//�{�[���̍ő吔
	static const int MAX_BONES = 32;

public: //�T�u�N���X

	//�C���X�^���V���O�`��p�\����(���W�ϊ��s��p)
	struct InstanceData
	{
		Vector4 color;		//�F
		DirectX::XMMATRIX world;		//���[���h�s��
	};

	struct ConstBufferDataShare
	{
		DirectX::XMMATRIX viewproj;	//�r���[�v���W�F�N�V�����s��
		DirectX::XMMATRIX lightCamera;
		UINT isDrawShadowToMyself;
		Vector3 cameraPos;	//�J�������W(���[���h���W)
	};

	//�萔�o�b�t�@�p�f�[�^�\����(���W�ϊ�)
	struct ConstBufferDataTransform
	{
		InstanceData data;
	};

	//�萔�o�b�t�@�p�f�[�^�\����(�X�L�j���O)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

	//�萔�o�b�t�@�p�f�[�^�\����(�V���h�E�}�b�v)
	struct ConstBufferDataShadow
	{
		XMMATRIX lightCamera;
		XMMATRIX world;
	};

public: //�ÓI�����o�֐�

	/// <summary>
	/// �f�X�N���v�^�q�[�v����
	/// </summary>
	static void StaticInitialize();
	
	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̐���
	/// </summary>
	/// <param name="objectType">��������p�C�v���C�����g�p�����I�u�W�F�N�g�̎��</param>
	/// <param name="pipelineData">�p�C�v���C���ݒ�ϐ�</param>
	static void CreateGraphicsPipeline(int objectType, PipelineData& pipelineData);

	/// <summary>
	/// OBJ�V���h�E�p�p�C�v���C������
	/// </summary>
	static void CreateShadowObjGraphicsPipeline();

	/// <summary>
	/// FBX�V���h�E�p�p�C�v���C������
	/// </summary>
	static void CreateShadowFbxGraphicsPipeline();

	//getter
	static ID3D12DescriptorHeap* GetDescHeap() { return Object3D::descHeapSRV.Get(); }

	//setter
	static void SetDevice(ID3D12Device* device) { Object3D::device = device; }
	static void SetCamera(Camera* camera) { Object3D::camera = camera; }
	static void SetLight(Light* light) { Object3D::light = light; }

protected: //�ÓI�����o�ϐ�
	static std::vector<Object3D*> drawList;

	//�f�o�C�X
	static ID3D12Device* device;
	//�J����
	static Camera* camera;
	//���C�g
	static Light* light;
	//SRV�p�f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�[�x�e�N�X�`��SRV�p�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeapDepthSRV;
	//FBX���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> fbxRootsignature;
	//FBX�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> fbxPipelinestate;
	//OBJ�V���h�E���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> shadowFbxRootsignature;
	//OBJ�V���h�E�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> shadowFbxPipelinestate;
	//OBJ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> objRootsignature;
	//OBJ�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> objPipelinestate;
	//OBJ�V���h�E���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> shadowObjRootsignature;
	//OBJ�V���h�E�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> shadowObjPipelinestate;

	static int prevDrawObjectType;

	static int loadCount;

	static XMMATRIX matOrthographicLH;

public:

	static void DrawAll();
	static void WriteDepthTex();

	static int GetLoadCount() { return Object3D::loadCount; }
	static void IncrementLoadCount() { Object3D::loadCount++; }
	static void SetMatrixOrthographicLH(float viewWigth, float viewHeight, float nearZ, float farZ) {
		matOrthographicLH = DirectX::XMMatrixOrthographicLH(viewWigth, viewHeight, nearZ, farZ);
	}

private:
	/// <summary>
	/// �`��
	/// </summary>
	virtual void DrawPrivate();

	/// <summary>
	/// �V���h�E�`��
	/// </summary>
	virtual void DrawShadow();

	/// <summary>
	/// �I�u�W�F�N�g�`��O����
	/// </summary>
	static void BeginDraw();

	/// <summary>
	/// �V���h�E�`��O����
	/// </summary>
	static void BeginDrawShadow();

public: //�����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Object3D() = default;

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~Object3D();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`�惊�X�g�ɓo�^ (�o�^���ꂽ���ɍŌ�ɂ܂Ƃ߂ĕ`�悷��)
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐�
	/// </summary>
	/// <param name="info">�Փˏ��</param>
	virtual void OnCollision(const CollisionInfo& info){}

	/// <summary>
	/// �A�j���[�V�����J�n
	/// </summary>
	void PlayAnimation();

#pragma region �֗��֐�
	//�����o�֐�
	public:
	/// <summary>
	/// �X�P�[�����w�蕪���Z����
	/// </summary>
	/// <param name="scaleX">�X�P�[��X</param>
	/// <param name="scaleY">�X�P�[��Y</param>
	/// <param name="scaleZ">�X�P�[��Z</param>
	void AddScale(const float scaleX, const float scaleY, const float scaleZ)
	{
		this->scale.x += scaleX; this->scale.y += scaleY; this->scale.z += scaleZ;
	}

	/// <summary>
	/// ��]�p���w�蕪���Z����
	/// </summary>
	/// <param name="rotationX">��]�pX</param>
	/// <param name="rotationY">��]�pY</param>
	/// <param name="rotationZ">��]�pZ</param>
	void AddRotation(const float rotationX, const float rotationY, const float rotationZ)
	{
		this->rotation.x += rotationX; this->rotation.y += rotationY; this->rotation.z += rotationZ;
	}

	/// <summary>
	/// ���W���w�蕪���Z����
	/// </summary>
	/// <param name="positionX">X���W</param>
	/// <param name="positionY">Y���W</param>
	/// <param name="positionZ">Z���W</param>
	void AddPosition(const float positionX, const float positionY, const float positionZ)
	{
		this->position.x += positionX; this->position.y += positionY; this->position.z += positionZ;
	}

#pragma endregion

#pragma region Getter
	//�����o�֐�
public:
	/// <summary>
	/// �X�P�[���̎擾
	/// </summary>
	/// <returns>���W</returns>
	const Vector3& GetScale() const { return scale; }

	/// <summary>
	/// ��]�p�̎擾
	/// </summary>
	/// <returns>���W</returns>
	const Vector3& GetRotation() const { return rotation; }


	/// <summary>
	/// ���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	const Vector3& GetPosition() const { return position; }

	/// <summary>
	/// �F(RGBA)���擾����
	/// </summary>
	/// <returns>�F(RGBA)</returns>
	const Vector4& GetColor() const { return color; }

	/// <summary>
	/// �F(RGBA)��0�`255�̐��l�Ŏ擾����
	/// </summary>
	/// <returns>�F(RGBA)�A0�`255�͈̔�</returns>
	const Vector4 GetColorAs0To255() const {
		Vector4 c = { color.x * 255,color.y * 255 ,color.z * 255 ,color.w * 255 };
		return c;
	}

	/// <summary>
	/// ���[���h�s��̎擾
	/// </summary>
	/// <returns>���[���h�s��</returns>
	const XMMATRIX& GetMatWorld() const { return matWorld; }

	/// <summary>
	/// obj���f�����擾
	/// </summary>
	/// <returns>obj���f��</returns>
	inline ObjModel* GetObjModel() const { return objModel; }

	/// <summary>
	/// �N���X���擾
	/// </summary>
	/// <returns></returns>
	const char* GetObjectName() const { return name; }

	/// <summary>
	/// �R���C�_�[�擾
	/// </summary>
	/// <returns></returns>
	const BaseCollider* GetCollider() const { return collider; }

#pragma endregion

#pragma region Setter
	//�����o�֐�
public:
	/// <summary>
	/// �X�P�[�����Z�b�g
	/// </summary>
	/// <param name="scale">�X�P�[��</param>
	void SetScale(const Vector3& scale) { this->scale = scale; }

	/// <summary>
	/// �X�P�[�����Z�b�g
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetScale(const float x, const float y, const float z) { this->scale = { x,y,z }; }

	/// <summary>
	/// ��]�p���Z�b�g
	/// </summary>
	/// <param name="rotation">��]�p</param>
	void SetRotation(const Vector3& rotation) { this->rotation = rotation; }

	/// <summary>
	/// ��]�p���Z�b�g
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetRotation(const float x, const float y, const float z) { this->rotation = { x,y,z }; }


	/// <summary>
	/// ���W���Z�b�g
	/// </summary>
	/// <param name="position">���W</param>
	void SetPosition(const Vector3& position) { this->position = position; }

	/// <summary>
	/// ���W���Z�b�g
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetPosition(const float x, const float y, const float z) { this->position = { x,y,z }; }

	/// <summary>
	/// �F(RGBA)���Z�b�g(0�`1�w��o�[�W����)
	/// </summary>
	/// <param name="R">��</param>
	/// <param name="G">��</param>
	/// <param name="B">��</param>
	/// <param name="A">�A���t�@�l</param>
	void SetColor(float R, float G, float B, float A) {
		SetColor({ R,G,B,A });
	}

	/// <summary>
	/// �F(RGBA)���Z�b�g
	/// </summary>
	/// <param name="color">�F(RGBA)</param>
	void SetColor(const Vector4& color) { this->color = color; }

	/// <summary>
	/// �F(RGBA)��0�`255�͈̔͂ŃZ�b�g
	/// </summary>
	/// <param name="R">��</param>
	/// <param name="G">��</param>
	/// <param name="B">��</param>
	/// <param name="A">�A���t�@�l</param>
	void SetColorAs0To255(const float R, const float G, const float B, const float A = 255)
	{
		SetColor({ R / 255,G / 255,B / 255,A / 255 });
	}

	/// <summary>
	/// �F(RGBA)��0�`255�͈̔͂ŃZ�b�g
	/// </summary>
	/// <param name="color">�F(RGBA)</param>
	void SetColorAs0To255(const Vector4& color)
	{
		SetColor({ color.x / 255, color.y / 255, color.z / 255, color.w / 255 });
	}

	/// <summary>
	/// �R���C�_�[�̃Z�b�g
	/// </summary>
	/// <param name="collider">�R���C�_�[</param>
	void SetCollider(BaseCollider* collider);

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="fbxModel">FBX���f��</param>
	virtual void SetFbxModel(FbxModel* fbxModel);

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="objModel">OBJ���f��</param>
	virtual void SetObjModel(ObjModel* objModel);

	/// <summary>
	/// �����̉e�𑼂̃I�u�W�F�N�g�ɕ`�悷�邩�Z�b�g
	/// </summary>
	/// <param name="flag"></param>
	virtual void SetDrawShadowToOther(bool flag) { isDrawShadowToOther = flag; }

	/// <summary>
	/// ���̃I�u�W�F�N�g�e�������ɕ`�悷�邩�Z�b�g
	/// </summary>
	/// <param name="flag"></param>
	virtual void SetDrawShadowToMyself(bool flag) { isDrawShadowToMyself = flag; }
#pragma endregion

protected: //�����o�ϐ�
	//�萔�o�b�t�@(���L�f�[�^)
	ComPtr<ID3D12Resource> constBuffShare;

	//�萔�o�b�t�@(���W�ϊ��s��p)
	ComPtr<ID3D12Resource> constBuffTransform;

	//�萔�o�b�t�@(�X�L��)
	ComPtr<ID3D12Resource> constBuffSkin;

	//�萔�o�b�t�@(�V���h�E)
	ComPtr<ID3D12Resource> constBuffShadow;

	//���[�J���X�P�[��
	Vector3 scale = { 1, 1, 1 };
	//X,Y,Z�����̃��[�J����]�p
	Vector3 rotation = { 0, 0, 0 };
	//���[�J�����W
	Vector3 position = { 0, 0, 0 };
	//���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;
	//FBX���f��
	FbxModel* fbxModel = nullptr;
	//OBJ���f��
	ObjModel* objModel = nullptr;

	//�F
	Vector4 color = { 1,1,1,1 };

	//�r���{�[�h�^�C�v
	BillboardType billboardType = BillboardType::NONE;

	//1�t���[���̎���
	FbxTime frameTime;
	//�A�j���[�V�����J�n����
	FbxTime startTime;
	//�A�j���[�V�����I������
	FbxTime endTime;
	//���ݎ��ԁi�A�j���[�V�����j
	FbxTime currentTime;
	//�A�j���[�V�����Đ���
	bool isPlay = false;

	//�I�u�W�F�N�g�^�C�v
	int objectType = -1;

	bool isDrawShadowToOther = true;
	bool isDrawShadowToMyself = true;

	//�N���X���i�f�o�b�O�p�j
	const char* name = nullptr;
	//�R���C�_�[
	BaseCollider* collider = nullptr;

};

//�I�u�W�F�N�g�̍X�V���ꊇ�ōs�����߂̃N���X
class Object3DManager
{
private:
	template <typename T>
	using vector = std::vector<T>;

private:
	static vector<Object3D*> pObject3DList;

public:
	static void Initialize();
	static void AddObject(Object3D* p);
	static void UpdateAllObject();
	static void DeleteObject(Object3D* p);
	static void DeleteAllObject();

};