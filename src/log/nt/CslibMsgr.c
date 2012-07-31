#include <windows.h>

__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hInstance,
                                          DWORD     dwReason,
                                          LPVOID    lpReserved)
{
    return 1;
}
