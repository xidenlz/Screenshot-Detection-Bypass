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
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static BOOL SaveBitmap(HBITMAP hBmp, HDC hDC, const char *path)
{
    BITMAP             bmp;
    BITMAPINFOHEADER   bih;
    BITMAPFILEHEADER   bfh;
    DWORD              imgSize;
    BYTE              *pixels;
    FILE              *f;

    GetObject(hBmp, sizeof(bmp), &bmp);

    memset(&bih, 0, sizeof(bih));
    bih.biSize        = sizeof(bih);
    bih.biWidth       = bmp.bmWidth;
    bih.biHeight      = bmp.bmHeight;
    bih.biPlanes      = 1;
    bih.biBitCount    = 32;
    bih.biCompression = BI_RGB;

    imgSize = (DWORD)bmp.bmWidth * bmp.bmHeight * 4;
    pixels  = malloc(imgSize);
    if (!pixels) 
        return FALSE;


    GetDIBits(hDC, hBmp, 0, bmp.bmHeight, pixels, (BITMAPINFO *)&bih, DIB_RGB_COLORS);

    memset(&bfh, 0, sizeof(bfh));
    bfh.bfType    = 0x4D42; 
    bfh.bfOffBits = sizeof(bfh) + sizeof(bih);
    bfh.bfSize    = bfh.bfOffBits + imgSize;

    f = fopen(path, "wb");
    if (!f) 
    { 
        free(pixels); 
        return FALSE;
    }
    fwrite(&bfh, sizeof(bfh), 1, f);
    fwrite(&bih, sizeof(bih), 1, f);
    fwrite(pixels, imgSize, 1, f);
    fclose(f);
    free(pixels);
    return TRUE;
}


static VOID TakeScreenshot(VOID)
{
    static int n = 0;
    int w, h;
    HDC hScreen, hMem;
    HBITMAP hBmp, hOld;
    char path[MAX_PATH];

    w = GetSystemMetrics(SM_CXSCREEN);
    h = GetSystemMetrics(SM_CYSCREEN);

    hScreen = GetDC(NULL);
    hMem    = CreateCompatibleDC(hScreen);
    hBmp    = CreateCompatibleBitmap(hScreen, w, h);
    hOld    = (HBITMAP)SelectObject(hMem, hBmp);

    BitBlt(hMem, 0, 0, w, h, hScreen, 0, 0, SRCCOPY); 

    SelectObject(hMem, hOld); 

    sprintf(path, "screenshot_%d.bmp", n++);
    printf(SaveBitmap(hBmp, hScreen, path) ? "[+] captured -> %s\n" : "[-] capture failed (%s)\n", path);

    DeleteObject(hBmp);
    DeleteDC(hMem);
    ReleaseDC(NULL, hScreen);
}

int main(void)
{
    printf("> BitBlt hook target <\n");
    printf("PID: %lu\n", GetCurrentProcessId());
    printf("F9: capture\n");
    printf("ESC: quit\n\n");

    for (;;)
    {
        if (GetAsyncKeyState(VK_F9) & 1) 
            TakeScreenshot();
        if (GetAsyncKeyState(VK_ESCAPE) & 1) 
            break;
        Sleep(10);  
    }

    printf("[*] bye\n");
    return 0;
}
