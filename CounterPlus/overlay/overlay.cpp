#include "overlay.h"
#include <d3d11.h>
#include <dwmapi.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h"
#include "../features/features.h"
#include "../config/config.h"

HWND game_window = FindWindowA(nullptr, "Counter-Strike: Global Offensive - Direct3D 9");

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
);

extern LRESULT CALLBACK MainWndProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
		return true;
	}

	switch (message) {
		case WM_ACTIVATE:
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_SIZE:
			if (overlay::device != nullptr && w_param != SIZE_MINIMIZED) {
				overlay::DestroyRenderTarget();
				overlay::swap_chain->ResizeBuffers(0, (UINT)LOWORD(l_param), (UINT)HIWORD(l_param), DXGI_FORMAT_UNKNOWN, 0);
				overlay::CreateRenderTarget();
			}
			return 0;

		case WM_SYSCOMMAND:
			if ((w_param & 0xfff0) == SC_KEYMENU) {
				return 0;
			}
			break;

		default:
			return DefWindowProc(window, message, w_param, l_param);
	}

	return DefWindowProc(window, message, w_param, l_param);
}

void overlay::SaveUserSettings() {
	config::ConfigPreset cfg{
		globals::triggerbot::enable,
		globals::triggerbot::humanizer,
		globals::triggerbot::humanize_min,
		globals::triggerbot::humanize_max,
		globals::triggerbot::pre_delay,
		globals::triggerbot::hold_time,
		globals::triggerbot::post_delay,
		globals::esp::enable,
		globals::esp::enemy_color[0],
		globals::esp::enemy_color[1],
		globals::esp::enemy_color[2],
		globals::esp::team_color[0],
		globals::esp::team_color[1],
		globals::esp::team_color[2],
		globals::bhop::enable};

	SaveConfig(cfg);
}

void overlay::CreateHWindow(const char *window_name) {
	window_class.cbSize = sizeof(WNDCLASSEXW);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = MainWndProc;
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hIcon = LoadIcon(0, IDI_APPLICATION);
	window_class.hIconSm = LoadIcon(0, IDI_APPLICATION);
	window_class.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	window_class.hInstance = NULL;
	window_class.lpszClassName = "CounterPlus Stats Class";

	RegisterClassEx(&window_class);

	window = CreateWindowExA(
		WS_EX_TRANSPARENT | WS_EX_LAYERED,
		window_class.lpszClassName,
		"CounterPlus Stats",
		WS_POPUP | WS_VISIBLE,
		0,
		0,
		kWidth,
		kHeight,
		nullptr,
		nullptr,
		window_class.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	MARGINS margins = { 0, 0, kWidth, kHeight };

	DwmExtendFrameIntoClientArea(window, &margins);
	SetWindowLongA(window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
}

bool overlay::CreateDevice() {
	sd.BufferDesc.Width = 0U;
	sd.BufferDesc.Height = 0U;
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.SampleDesc.Quality = 0U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	const D3D_FEATURE_LEVEL levels[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};

	D3D_FEATURE_LEVEL level{};

	if (FAILED(D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0U,
			levels,
			2U,
			D3D11_SDK_VERSION,
			&sd,
			&swap_chain,
			&device,
			&level,
			&device_context))) 
	{
		DestroyWindow(window);
		UnregisterClass(window_class.lpszClassName, window_class.hInstance);

		return false;
	}

	CreateRenderTarget();

	return true;
}

void overlay::CreateImGui() {
	ImGui::CreateContext();
	CreateImGuiStyles();
	ImGuiIO &io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18.f);
	io.IniFilename = NULL;
	config::LoadConfig();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);
}

static void HelpMarker(const char *desc) {
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void overlay::Render() {
	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if (msg.message == WM_QUIT) {
			globals::is_running = false;
		}
	}

	if (GetAsyncKeyState(VK_DELETE) & 0x01) {
		globals::show_menu = !globals::show_menu;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (globals::show_menu) {
		if (window_state != 1) {
			ChangeClickability(true, window);
		}

		// begin ImGui render
		ImGui::Begin(
			"CounterPlus",
			nullptr, 
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize
		);

		// triggerbot section
		ImGui::SeparatorText("Triggerbot");
		ImGui::Checkbox("Enable##enable_triggerbot", &globals::triggerbot::enable);

		if (globals::triggerbot::enable) {
			ImGui::Checkbox("Humanizer", &globals::triggerbot::humanizer);
			ImGui::SameLine();
			HelpMarker("Humanizer adds a random amount of time (in ms) to:\nDelay Before Firing, Trigger Hold Time, and Delay After Firing\n\nMin: Base amount (gets added to total)\nMax: Random number 0 through Max\n\nExample humanizer that is a min of 5 and a TOTAL max of 35:\nMin = 5, Max = 30");

			if (globals::triggerbot::humanizer) {
				ImGui::SliderInt("Humanizer Min", &globals::triggerbot::humanize_min, 0, 200);
				ImGui::SliderInt("Humanizer Max", &globals::triggerbot::humanize_max, 0, 200);
			}

			ImGui::SliderInt("Delay Before Firing", &globals::triggerbot::pre_delay, 0, 200);
			ImGui::SliderInt("Trigger Hold Time", &globals::triggerbot::hold_time, 0, 500);
			ImGui::SliderInt("Delay After Firing", &globals::triggerbot::post_delay, 0, 1000);
		}

		// esp section
		ImGui::SeparatorText("ESP##enable_esp");
		ImGui::Checkbox("Enable", &globals::esp::enable);
		if (globals::esp::enable) {
			ImGui::ColorEdit3("Enemy Color", (float *)&globals::esp::enemy_color);
			ImGui::ColorEdit3("Team Color", (float *)&globals::esp::team_color);
		}

		// bunnyhop section
		ImGui::SeparatorText("Bunnyhop");
		ImGui::Checkbox("Enable##enable_bhop", &globals::bhop::enable);

		// config section
		ImGui::SeparatorText("Config");
		if (ImGui::Button("Save Config")) {
			SaveUserSettings();
		}

		ImGui::SameLine();
		if (ImGui::Button("Load Config")) {
			config::LoadConfig();
		}

		ImGui::SameLine();
		ImGui::Text("Show/Hide Menu: DELETE");

		ImGui::End();
	}
	else {
		if (window_state != 0) {
			ChangeClickability(false, window);
		}
	}

	if (!globals::show_menu) {
		if (game_window == GetForegroundWindow()) {
			ShowWindow(window, SW_RESTORE);
			SetWindowPos(game_window, window, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			SendMessage(window, WM_PAINT, 0, 0);
		}
		else {
			ShowWindow(window, SW_HIDE);
		}
	}

	if (globals::esp::enable) {
		features::esp::DrawEsp();
	}

	ImGui::Render();

	constexpr float color[4]{0.f, 0.f, 0.f, 0.f};
	device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
	device_context->ClearRenderTargetView(render_target_view, color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swap_chain->Present(0, 0);
}

void overlay::DestroyImGui() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void overlay::DestroyDevice() {

	if (swap_chain) {
		swap_chain->Release();
	}

	if (device_context) {
		device_context->Release();
	}

	if (device) {
		device->Release();
	}

	if (render_target_view) {
		render_target_view->Release();
	}
}

void overlay::DestroyRenderTarget() {
	if (render_target_view) {
		render_target_view->Release();
		render_target_view = nullptr;
	}
}

void overlay::CreateRenderTarget() {
	ID3D11Texture2D *back_buffer;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

	if (!back_buffer) {
		return;
	}

	device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
	back_buffer->Release();
}

void overlay::DestroyHWindow() {
	DestroyWindow(window);
	UnregisterClass(window_class.lpszClassName, window_class.hInstance);
}

void overlay::ChangeClickability(bool can_click, HWND hWnd) {
	long style = GetWindowLong(hWnd, GWL_EXSTYLE);

	if (can_click) {
		style &= ~WS_EX_LAYERED;
		SetWindowLong(hWnd, GWL_EXSTYLE, style);
		SetForegroundWindow(hWnd);
		window_state = 1;
	}
	else
	{
		style |= WS_EX_LAYERED;
		SetWindowLong(hWnd, GWL_EXSTYLE, style);
		window_state = 0;
	}
}

void overlay::CreateImGuiStyles() {
	// Rounded Visual Studio style by RedNicStone from ImThemes
	ImGuiStyle &style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(8.0f, 6.0f);
	style.FrameRounding = 5.5f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 11.0f;
	style.ScrollbarRounding = 2.5f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 2.0f;
	style.TabRounding = 3.5f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921568870544434f, 0.5921568870544434f, 0.5921568870544434f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
}