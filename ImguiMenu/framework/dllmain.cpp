#include "headers/includes.h"
#include <Windows.h>
#include <gl/GL.h>
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"

// -------------------------------------------------------
// Minecraft Java Edition uses LWJGL / OpenGL
// We hook wglSwapBuffers from opengl32.dll
// -------------------------------------------------------

typedef BOOL(WINAPI* wglSwapBuffers_t)(HDC);
static wglSwapBuffers_t oWglSwapBuffers = nullptr;

static bool  g_imgui_initialized = false;
static HWND  g_hwnd              = nullptr;
static WNDPROC g_oWndProc        = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ---- WndProc hook: forward input events to ImGui ----
LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    return CallWindowProcA(g_oWndProc, hWnd, uMsg, wParam, lParam);
}

// ---- Hooked wglSwapBuffers ----
BOOL WINAPI hkWglSwapBuffers(HDC hDC)
{
    if (!g_imgui_initialized)
    {
        g_hwnd = WindowFromDC(hDC);

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        io.IniFilename  = nullptr; // disable imgui.ini

        ImGui_ImplWin32_Init(g_hwnd);
        ImGui_ImplOpenGL3_Init("#version 130");

        // Hook WndProc so keyboard/mouse reach ImGui
        g_oWndProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtrA(g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hWndProc)));

        g_imgui_initialized = true;
    }

    // --- Render ImGui frame ---
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    gui->render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return oWglSwapBuffers(hDC);
}

// ---- Install JMP hook on wglSwapBuffers ----
static void HookWglSwapBuffers()
{
    HMODULE hGL = GetModuleHandleA("opengl32.dll");
    if (!hGL)
        hGL = LoadLibraryA("opengl32.dll");
    if (!hGL)
        return;

    void* pTarget = reinterpret_cast<void*>(GetProcAddress(hGL, "wglSwapBuffers"));
    if (!pTarget)
        return;

    // Save original bytes for trampoline (first 14 bytes)
    static BYTE original_bytes[14];
    memcpy(original_bytes, pTarget, 14);

    // Save original function pointer via trampoline stub in allocated memory
    BYTE* trampoline = reinterpret_cast<BYTE*>(
        VirtualAlloc(nullptr, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!trampoline)
        return;

    // Copy original 14 bytes + JMP back to pTarget+14
    memcpy(trampoline, original_bytes, 14);
    BYTE jmp_back[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    *reinterpret_cast<void**>(jmp_back + 6) =
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pTarget) + 14);
    memcpy(trampoline + 14, jmp_back, 14);

    oWglSwapBuffers = reinterpret_cast<wglSwapBuffers_t>(trampoline);

    // Patch target with JMP to our hook
    BYTE jmp_hook[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    *reinterpret_cast<void**>(jmp_hook + 6) = reinterpret_cast<void*>(hkWglSwapBuffers);

    DWORD oldProtect;
    VirtualProtect(pTarget, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(pTarget, jmp_hook, 14);
    VirtualProtect(pTarget, 14, oldProtect, &oldProtect);
}

// ---- Main thread ----
DWORD WINAPI MainThread(LPVOID hModule)
{
    // Wait until Minecraft's OpenGL context is ready
    while (!GetModuleHandleA("opengl32.dll"))
        Sleep(200);

    Sleep(1000); // give LWJGL time to init its context

    HookWglSwapBuffers();

    // Keep thread alive
    while (true)
        Sleep(500);

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
