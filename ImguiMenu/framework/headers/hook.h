#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

// Forward declarations for hook functions
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
void ApplyVMTHook();
void* GetPresentVTable();

// WndProc hook
extern WNDPROC g_oWndProc;
extern HWND g_hwnd;
extern bool g_imgui_initialized;
