#include "ImguiHelper.h"
#include "DX12Util.h"

ImguiHelper::ComPtr<ID3D12DescriptorHeap> ImguiHelper::heapForImgui;
ImguiHelper::string ImguiHelper::windowName;

void ImguiHelper::Initialize()
{
	//imgui用のヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DX12Util::GetDevice()->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(heapForImgui.ReleaseAndGetAddressOf()));
	
	if (ImGui::CreateContext() == nullptr) {
		assert(0);
	}
	
	bool blnResult = ImGui_ImplWin32_Init(DX12Util::GetHwnd());
	if (!blnResult)
	{
		assert(0);
	}

	blnResult = ImGui_ImplDX12_Init(
		DX12Util::GetDevice(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		heapForImgui.Get(),
		heapForImgui->GetCPUDescriptorHandleForHeapStart(),
		heapForImgui->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImguiHelper::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


void ImguiHelper::BeginCommand(const string& windowName)
{
	AllNewFrame();
	ImGui::Begin(windowName.c_str());
	ImguiHelper::windowName = windowName;
}

void ImguiHelper::Draw()
{
	ImGui::Render();
	DX12Util::GetCmdList()->SetDescriptorHeaps(1, heapForImgui.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DX12Util::GetCmdList());
}

void ImguiHelper::EndCommand()
{
	ImGui::End();
}

void ImguiHelper::AllNewFrame()
{
	//imgui描画前処理
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImguiHelper::SetWindowSize(const Vector2& size, ImGuiCond cond)
{
	ImGui::SetWindowSize(ImVec2(size.x, size.y), cond);
}

void ImguiHelper::SetWindowSize(float width, float height, ImGuiCond cond)
{
	ImGui::SetWindowSize(ImVec2(width, height), cond);
}

void ImguiHelper::SetWindowPos(const Vector2& pos, ImGuiCond cond)
{
	ImGui::SetWindowPos(windowName.c_str(), ImVec2(pos.x, pos.y), cond);
}

void ImguiHelper::SetWindowPos(float posX, float posY, ImGuiCond cond)
{
	ImGui::SetWindowPos(windowName.c_str(), ImVec2(posX, posY), cond);
}
