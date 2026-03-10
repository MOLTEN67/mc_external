#include <Windows.h>
#include <thread>
#include "hooks/hooks.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
    // Wait for game to fully load
    Sleep(1000);

    // Initialize hooks
    hooks::init();

    // Keep DLL alive
    while (!GetAsyncKeyState(VK_END))
        Sleep(100);

    // Cleanup
    hooks::destroy();

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }
    return TRUE;
}
