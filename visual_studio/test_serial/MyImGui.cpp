#include <pch.h>


#include "MyImGui.h"
#include "ThreadPool.h"
#include "MyDefine.h"
#include "GuiInterface.h"





MyImGui* MyImGui::MyImGuis = nullptr;
std::atomic<bool> g_Running = true;
std::atomic<bool> g_IsMoving = false;


MyImGui::MyImGui()
	: ThreadPools(std::make_shared<ThreadPool>(MYThreadCount))
{
	MyImGuis = this;
}
MyImGui::~MyImGui()
{

}


void MyImGui::Instance()
{
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);

	
	//Create Center
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &rect);

	int screenWidth = rect.right;
	int screenHeight = rect.bottom;

	int windowsizeX = 1000;
	int windowsizeY = 800;

	int x = (screenWidth - windowsizeX) / 2;
	int y = (screenHeight - windowsizeY) / 2;

	//hwnd = ::CreateWindowW(wc.lpszClassName, ProgramName, WS_POPUP, x, y, windowsizeX, windowsizeY, nullptr, nullptr, wc.hInstance, nullptr);
	hwnd = ::CreateWindowW(wc.lpszClassName, ProgramName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, windowsizeX, windowsizeY, nullptr, nullptr, wc.hInstance, nullptr);


	//window Rounding
	//HRGN hRgn = CreateRoundRectRgn(0, 0, windowsizeX + 1, windowsizeY + 1, 15, 15);
	//SetWindowRgn(hwnd, hRgn, TRUE);




	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
	}

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = nullptr;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.137f, 0.165f, 0.216f, 1.0f);

	//Font
	//Pretendard_semibold = io.Fonts->AddFontFromFileTTF("Pretendard-SemiBold.otf", 33.0f);

	io.Fonts->Build();

	HWND console = GetConsoleWindow();
	if (console)
		MoveWindow(console, 100, 100, 400, 200, TRUE);


	bool done = false;
	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		if (GetClientRect(hwnd, &rect))
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}

		if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;


		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Interfaces->Instance(io);

		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		HRESULT hr = g_pSwapChain->Present(1, 0);
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

}



bool MyImGui::CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}


void MyImGui::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam);
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	}
	case WM_MOVE:
	{
		g_IsMoving = false; // 창이 이동하면 강제로 렌더링 다시 시작
		break;
	}
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		g_Running = false;
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


void MyImGui::CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}


void MyImGui::CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}



void MyImGui::ChangeWindowSize(const int _SizeX , const int _SizeY , bool Popup)
{
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &MyImGui::MyImGuis->GetRECT());
	int screenWidth = MyImGui::MyImGuis->GetRECT().right;
	int screenHeight = MyImGui::MyImGuis->GetRECT().bottom;

	int x = (screenWidth - _SizeX) / 2;
	int y = (screenHeight - _SizeY) / 2;

	LONG newStyle;
	if (Popup)
		newStyle = WS_POPUP;
	else
		newStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
#ifdef _WIN64
	SetWindowLongPtr(MyImGui::MyImGuis->GetWindowHandle(), GWL_STYLE, newStyle);
#else
	SetWindowLong(MyImGui::MyImGuis->GetWindowHandle(), GWL_STYLE, newStyle);
#endif
	SetWindowRgn(MyImGui::MyImGuis->GetWindowHandle(), nullptr, TRUE);
	SetWindowPos(MyImGui::MyImGuis->GetWindowHandle(), nullptr, x, y, _SizeX, _SizeY, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_SHOWWINDOW);
}