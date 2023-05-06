#pragma once
#include "../utils/globals.h"
#include <d3d11.h>

namespace overlay {
	static int window_state = 0;

	const int kWidth = GetSystemMetrics(SM_CXSCREEN);
	const int kHeight = GetSystemMetrics(SM_CYSCREEN);

	static MSG msg;

	static HWND window = nullptr;
	static WNDCLASSEX window_class = { };
	static POINTS position = { };

	static DXGI_SWAP_CHAIN_DESC sd{};
	static ID3D11Device* device = nullptr;
	static ID3D11DeviceContext* device_context = nullptr;
	static IDXGISwapChain* swap_chain = nullptr;
	static ID3D11RenderTargetView* render_target_view = nullptr;

	void CreateHWindow(const char* window_name);
	void DestroyHWindow();

	bool CreateDevice();
	void DestroyDevice();

	void CreateImGui();
	void DestroyImGui();

	void CreateRenderTarget();
	void DestroyRenderTarget();

	void Render();

	void ChangeClickability(bool canclick, HWND ownd);

	void CreateImGuiStyles();

	void SaveUserSettings();
}

