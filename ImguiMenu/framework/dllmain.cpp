#include "headers/includes.h"
#include "headers/blur.h"
#include <d3d11.h>
#include <dxgi.h>
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain*, UINT, UINT);
static Present_t oPresent = nullptr;

static bool g_imgui_initialized = false;
static HWND g_hwnd = nullptr;
static WNDPROC g_oWndProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    return CallWindowProcA(g_oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!g_imgui_initialized)
    {
        // Get device and context from swapchain
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&var->device)))
        {
            var->device->GetImmediateContext(&var->context);

            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_hwnd = sd.OutputWindow;

            // Init ImGui
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

            ImGui_ImplWin32_Init(g_hwnd);
            ImGui_ImplDX11_Init(var->device, var->context);

            // Hook WndProc
            g_oWndProc = (WNDPROC)SetWindowLongPtrA(g_hwnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);

            g_imgui_initialized = true;
        }
    }

    if (g_imgui_initialized)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        gui->render();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void* GetPresentVTable()
{
    // Create a dummy DX11 device + swapchain to read the vtable
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow();
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* pDevice = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
    D3D_FEATURE_LEVEL featureLevel;

    if (FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &pSwapChain, &pDevice, &featureLevel, nullptr)))
        return nullptr;

    // vtable[8] = Present
    void** pVTable = *reinterpret_cast<void***>(pSwapChain);
    void* pPresent = pVTable[8];

    pSwapChain->Release();
    pDevice->Release();

    return pPresent;
}

void ApplyVMTHook()
{
    void* pPresent = GetPresentVTable();
    if (!pPresent)
        return;

    // Simple VMT hook via VirtualProtect
    DWORD oldProtect;
    void** pVTable = *reinterpret_cast<void***>(pPresent);

    // Use trampoline-style pointer swap via MH_CreateHook-like manual approach
    // We patch the function pointer directly using WriteProcessMemory approach
    oPresent = reinterpret_cast<Present_t>(pPresent);

    // Detour via inline patch (JMP hook)
    BYTE jmp_patch[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // JMP QWORD PTR [RIP+0]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // absolute address
    };
    *reinterpret_cast<void**>(jmp_patch + 6) = (void*)hkPresent;

    VirtualProtect(pPresent, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(pPresent, jmp_patch, 14);
    VirtualProtect(pPresent, 14, oldProtect, &oldProtect);
}

DWORD WINAPI MainThread(LPVOID hModule)
{
    // Wait for a valid foreground window
    while (!GetForegroundWindow())
        Sleep(100);

    ApplyVMTHook();

    // Keep thread alive
    while (true)
        Sleep(100);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }
    return TRUE;
}
