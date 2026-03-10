#include "hooks.h"
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "../ImguiMenu/framework/headers/includes.h"
#include "../ImguiMenu/framework/gui.cpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{
    // Simple VMT hook — replaces vtable entry and returns original pointer
    void* hook_vtable(void** vtable, int index, void* hook)
    {
        DWORD old_protect;
        VirtualProtect(&vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect);
        void* original = vtable[index];
        vtable[index] = hook;
        VirtualProtect(&vtable[index], sizeof(void*), old_protect, &old_protect);
        return original;
    }

    // Called every frame by DX11
    HRESULT WINAPI hk_present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags)
    {
        if (!imgui_initialized)
        {
            // Get DX11 device & context
            if (SUCCEEDED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device))))
            {
                device->GetImmediateContext(&context);

                // Get window handle from swap chain
                DXGI_SWAP_CHAIN_DESC sd;
                swap_chain->GetDesc(&sd);
                hwnd = sd.OutputWindow;

                // Create render target view
                ID3D11Texture2D* back_buffer = nullptr;
                swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
                device->CreateRenderTargetView(back_buffer, nullptr, &rtv);
                back_buffer->Release();

                // Init ImGui
                ImGui::CreateContext();
                ImGuiIO& io = ImGui::GetIO();
                io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

                ImGui_ImplWin32_Init(hwnd);
                ImGui_ImplDX11_Init(device, context);

                // Hook WndProc
                o_wndproc = reinterpret_cast<wndproc_fn>(SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hk_wndproc)));

                imgui_initialized = true;
            }
        }

        // Render our menu every frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Call our GUI render
        static c_gui gui_instance;
        gui_instance.render();

        ImGui::EndFrame();
        ImGui::Render();

        context->OMSetRenderTargets(1, &rtv, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        return o_present(swap_chain, sync_interval, flags);
    }

    // Called when game resizes window — need to recreate RTV
    HRESULT WINAPI hk_resize_buffers(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags)
    {
        if (rtv) { rtv->Release(); rtv = nullptr; }

        HRESULT hr = o_resize(swap_chain, buffer_count, width, height, new_format, flags);

        ID3D11Texture2D* back_buffer = nullptr;
        swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
        device->CreateRenderTargetView(back_buffer, nullptr, &rtv);
        back_buffer->Release();

        return hr;
    }

    // WndProc hook — pass input to ImGui
    LRESULT WINAPI hk_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
        return CallWindowProcW(o_wndproc, hWnd, msg, wParam, lParam);
    }

    void init()
    {
        // Create a dummy DX11 device & swap chain to get the vtable addresses
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount        = 1;
        sd.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow       = GetForegroundWindow();
        sd.SampleDesc.Count   = 1;
        sd.Windowed           = TRUE;
        sd.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

        ID3D11Device*       dummy_device    = nullptr;
        ID3D11DeviceContext* dummy_context  = nullptr;
        IDXGISwapChain*     dummy_swapchain = nullptr;

        D3D_FEATURE_LEVEL feature_level;
        D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            nullptr, 0, D3D11_SDK_VERSION,
            &sd, &dummy_swapchain,
            &dummy_device, &feature_level, &dummy_context
        );

        if (dummy_swapchain)
        {
            // Get vtable of IDXGISwapChain
            void** vtable = *reinterpret_cast<void***>(dummy_swapchain);

            // Present = index 8, ResizeBuffers = index 13
            o_present = reinterpret_cast<present_fn>(hook_vtable(vtable, 8, reinterpret_cast<void*>(hk_present)));
            o_resize  = reinterpret_cast<resize_fn>(hook_vtable(vtable, 13, reinterpret_cast<void*>(hk_resize_buffers)));

            dummy_swapchain->Release();
            dummy_device->Release();
            dummy_context->Release();
        }
    }

    void destroy()
    {
        // Restore WndProc
        if (hwnd && o_wndproc)
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(o_wndproc));

        // Shutdown ImGui
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        // Release DX objects
        if (rtv)     { rtv->Release();     rtv     = nullptr; }
        if (context) { context->Release(); context = nullptr; }
        if (device)  { device->Release();  device  = nullptr; }
    }
}
