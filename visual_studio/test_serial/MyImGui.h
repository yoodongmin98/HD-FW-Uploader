#pragma once

#include "pch.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <d3d11.h>
#include <tchar.h>




// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



class GuiInterface;
class ThreadPool;
class MyImGui
{
public:

    static MyImGui* MyImGuis;
    MyImGui();
    ~MyImGui();
    void Instance();

    void ChangeWindowSize(const int _SizeX, const int _SizeY, bool Popup = false);
    std::shared_ptr<ThreadPool> GetThreadPool()
    {
        return ThreadPools;
    }
    const float GetWindowSize_X()
    {
        return width;
    }
    const float GetWindowSize_Y()
    {
        return height;
    }
    HWND& GetWindowHandle()
    {
        return hwnd;
    }
    RECT& GetRECT()
    {
        return rect;
    }

protected:
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

private:
    HWND hwnd;
    RECT rect;
    const WCHAR* ProgramName = L"[Program Name]";
    float width = 0.0f;
    float height = 0.0f;

    std::shared_ptr<ThreadPool> ThreadPools = nullptr;
    std::shared_ptr<GuiInterface> Interfaces = nullptr;


    //font
 /*   ImFont* Pretendard_semibold;
    ImFont* Pretendard_Regular;
    ImFont* Pretendard_small;
    ImFont* Pretendard_smalllog;
    ImFont* Pretendard_13;*/
};