#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#pragma comment(lib, "d3d12.lib")

class ImguiHelper
{
	using XMFLOAT2 = DirectX::XMFLOAT2;

	using string = std::string;
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static ComPtr<ID3D12DescriptorHeap> heapForImgui;
	static string windowName;

public:

	static void Initialize();
	static void Finalize();
	
	static void BeginCommand(string windowName);
	static void EndCommand();
	static void Draw();

	static void AllNewFrame();
	static void SetWindowSize(XMFLOAT2 size, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	static void SetWindowSize(float width, float height, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	static void SetWindowPos(XMFLOAT2 pos, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	static void SetWindowPos(float posX, float posY, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	
	static ComPtr<ID3D12DescriptorHeap> GetHeapForImgui() { return heapForImgui; }
	
};