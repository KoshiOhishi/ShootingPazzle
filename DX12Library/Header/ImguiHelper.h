#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include "Vector2.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"

#pragma comment(lib, "d3d12.lib")

enum DrawPhase {
	DRAW_PHASE_NONE,
	DRAW_PHASE_BEGIN,
	DRAW_PHASE_DRAWABLE,
};

class ImguiHelper
{
	using string = std::string;
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static ComPtr<ID3D12DescriptorHeap> heapForImgui;
	static string windowName;
	static int drawPhase;

public:

	static void Initialize();
	static void Finalize();
	
	static void BeginCommand(const string& windowName);
	static void EndCommand();
	static void Draw();

	static void AllNewFrame();
	static void SetWindowSize(const Vector2& size, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	static void SetWindowSize(float width, float height, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	static void SetWindowPos(const Vector2& pos, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	static void SetWindowPos(float posX, float posY, ImGuiCond cond = ImGuiCond_::ImGuiCond_Once);
	
	static ComPtr<ID3D12DescriptorHeap> GetHeapForImgui() { return heapForImgui; }
	
};