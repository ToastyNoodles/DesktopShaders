#include <glad/glad.h>
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <gl/GL.h>
#include "shader.h"

HWND hWorkerW = NULL;
HWND hShellView = NULL;

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    char buffer[256];
    GetClassName(hwnd, buffer, sizeof(buffer));
    if (strcmp(buffer, "SHELLDLL_DefView") == 0)
    {
        char title[256];
        GetWindowText(hwnd, title, sizeof(title));
        if (strlen(title) == 0)
        {
            hShellView = hwnd;
        }
    }
    else
    {
        if (hShellView && strcmp(buffer, "WorkerW") == 0)
        {
            hWorkerW = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char buffer[256];
    GetClassName(hwnd, buffer, sizeof(buffer));
    if (hShellView && strcmp(buffer, "WorkerW") == 0)
    {
        hWorkerW = hwnd;
        return FALSE;
    }
    EnumChildWindows(hwnd, EnumChildProc, 0);
    return TRUE;
}

int main()
{
    HWND progman = FindWindow("Progman", NULL);
    if (!progman)
    {
        MessageBox(NULL, "Failed to find progman window", "Error", MB_OK);
        return -1;
    }

    SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);
    EnumWindows(EnumWindowsProc, 0);
    if (!hWorkerW)
    {
        MessageBox(NULL, "Failed to find workerW!", "Error", MB_OK);
        return -1;
    }  

    WNDCLASS windowClass = {0};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = DefWindowProc;
    windowClass.hInstance = GetModuleHandle(0);
    windowClass.lpszClassName = "DesktopShadersWindow";

    if (!RegisterClass(&windowClass))
    {
        MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
        return -1;
    }

    HWND hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, windowClass.lpszClassName, "Desktop Shaders", WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, GetModuleHandle(0), 0);
    if (!hwnd)
    {
        MessageBox(NULL, "Failed to create window!", "Error", MB_OK);
        return -1;
    }

    printf("\n\nHWND: %p\nWorkerW: %p\nProgman: %p\n", hwnd, hWorkerW, progman);
    SetParent(hwnd, hWorkerW);
    SetWindowLongPtr(hwnd, GWL_STYLE, WS_CHILDWINDOW | WS_VISIBLE);

    HWND desktopWindow = GetDesktopWindow();
    HDC desktopDC = GetDC(desktopWindow);
    COLORREF desktopColor = GetSysColor(COLOR_SCROLLBAR);
    SetLayeredWindowAttributes(hwnd, desktopColor, 0, LWA_COLORKEY);
    ReleaseDC(desktopWindow, desktopDC);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (!SetPixelFormat(hdc, pixelFormat, &pfd))
    {
        MessageBox(NULL, "Failed to set pixel format!", "Error", MB_OK);
        return -1;
    }

    HGLRC glContext = wglCreateContext(hdc);
    if (!wglMakeCurrent(hdc, glContext))
    {
        MessageBox(NULL, "Failed to make context current!", "Error", MB_OK);
        return -1;
    }

    if (!gladLoadGL())
    {
        MessageBox(NULL, "Failed to initialize glad!", "Error", MB_OK);
        return -1;
    }

    uint32_t shader = CreateShader("res/shaders/color.vert", "res/shaders/color.frag");
    uint32_t vao, vbo;
    float vertices[] =
    {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SwapBuffers(hdc);
    }

    return 0;
}