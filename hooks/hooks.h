#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace hooks
{
    // Hook typedefs
    using present_fn   = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
    using resize_fn    = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    using wndproc_fn   = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);

    // Original function pointers
    inline present_fn  o_present   = nullptr;
    inline resize_fn   o_resize    = nullptr;
    inline wndproc_fn  o_wndproc   = nullptr;

    // DX11 objects
    inline ID3D11Device*           device    = nullptr;
    inline ID3D11DeviceContext*    context   = nullptr;
    inline ID3D11RenderTargetView* rtv       = nullptr;
    inline HWND                    hwnd      = nullptr;

    inline bool imgui_initialized = false;

    // Hook functions
    HRESULT WINAPI hk_present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
    HRESULT WINAPI hk_resize_buffers(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags);
    LRESULT WINAPI hk_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void init();
    void destroy();

    // VMT hook helpers
    void* hook_vtable(void** vtable, int index, void* hook);
}
