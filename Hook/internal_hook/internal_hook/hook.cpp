#include "hook.hpp"

/*
    MIT License

    Copyright (c) 2024 xidenlz


    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

void Hooks::EnableConsole()
{
    if (!AllocConsole())
        return;

    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$",  "r", stdin);

    SetConsoleTitleW(L"Hooks log");
}

void Hooks::Alert(BOOL Result)
{
    std::printf("BitBlt called: %d\n", Result);
}

/* @ Brief
*  our hook runs instead of the real BitBlt
*  we hide the overlay from the capture first, let the original do its thing,
*  then bring it back
*/
BOOL WINAPI Hooks::hBitBlt(HDC hDest, int xDest, int yDest, int W, int H,  HDC hSrc, int xSrc, int ySrc, DWORD rop)
{
    if (WindowFromDC(hDest) != nullptr)
        return oBitBlt(hDest, xDest, yDest, W, H, hSrc, xSrc, ySrc, rop);

    // overlay won't show up in the screenshot there's better alter btw
    // but in general here you would hide your overlay
    SetWindowDisplayAffinity(g_hOverlay, 0x00000011);

    BOOL Result = oBitBlt(hDest, xDest, yDest, W, H, hSrc, xSrc, ySrc, rop);
    Hooks::Alert(Result);

    g_ScreenshotTaken = true;

    if (g_hOverlay)
    {
        InvalidateRect(g_hOverlay, nullptr, TRUE);
        SetTimer(g_hOverlay, g_Reset, 2000, nullptr);
        SetWindowDisplayAffinity(g_hOverlay, 0x00000000);         
    }

    return Result;
}



/* @ Brief
*  oBitBlt ends up pointing at the trampoline MinHook builds that's how we
*  still call the real function without recursing into ourselves
*/
void Hooks::InstallHook()
{
    if (MH_Initialize() != MH_OK)
        std::printf("MH_Initialize failed.\n");

    pTarget = GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "BitBlt");

    if (MH_CreateHook(pTarget, reinterpret_cast<LPVOID>(&Hooks::hBitBlt), reinterpret_cast<LPVOID*>(&oBitBlt)) != MH_OK)
        std::printf("MH_CreateHook failed.\n");

    if (MH_EnableHook(pTarget) != MH_OK)
        std::printf("MH_EnableHook failed.\n");

    std::printf("Hook installed on target 0x%p\n", (void*)pTarget);
}



/*
* @ Overlay
*/
LRESULT CALLBACK Overlay::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        HBRUSH magenta = CreateSolidBrush(RGB(255, 0, 255));
        FillRect((HDC)wParam, &rc, magenta);
        DeleteObject(magenta);
        return 1;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);

        SetBkMode(hdc, TRANSPARENT); 

        const char* text  = g_ScreenshotTaken ? "SCREENSHOT TAKEN" : "Overlay Running";
        COLORREF    color = g_ScreenshotTaken ? RGB(255, 0, 0)      : RGB(0, 255, 0);

        SetTextColor(hdc, color);
        DrawTextA(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_TIMER:
    {
        if (wParam == g_Reset)
        {
            KillTimer(hwnd, g_Reset);
            g_ScreenshotTaken = false;
            InvalidateRect(hwnd, nullptr, TRUE);
        }
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* 
* @ Overlay 
*/
DWORD WINAPI Overlay::Thread(LPVOID)
{
    WNDCLASSEXA wc{};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = Overlay::WndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = "BitBlt Example";
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 255)); 

    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        "BitBlt Example", "",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr,
        wc.hInstance,
        nullptr);

    if (!hwnd)
        return 1;

    g_hOverlay = hwnd;
    SetLayeredWindowAttributes(hwnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}
