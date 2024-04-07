#include "platform/Platform.h"

//Windows platform layer
#if CPLATFORM_WINDOWS

#include "core/Logger.h"
#include "core/Input.h"
#include "core/Event.h"

#include "containers/DArray.h"

#include <windows.h>
#include <windowsx.h> //param input extraction
#include <stdlib.h>

//for surface creation
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "renderer/vulkan/VulkanTypes.inl"

typedef struct PlatformState
{
    HINSTANCE hInstance;
    HWND hwnd;
    VkSurfaceKHR surface;

    //clock
    f64 clockFrequency;
    LARGE_INTEGER startTime;
} PlatformState;

static PlatformState* pState;

LRESULT CALLBACK Win32ProcessMessage(HWND _hwnd, u32 _msg, WPARAM _wparam, LPARAM _lparam);

b8 PlatformSystemStartup(u64* _memoryRequirement, void* _state, const char* _appName, i32 _x, i32 _y, i32 _width, i32 _height)
{
    *_memoryRequirement = sizeof(PlatformState);
    if(_state == 0)
        return true;
    
    pState = _state;
    pState->hInstance = GetModuleHandleA(0);

    //setup window class
    HICON icon = LoadIcon(pState->hInstance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS; //get double clicks
    wc.lpfnWndProc = Win32ProcessMessage;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = pState->hInstance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);   //NULL; //manage cursor manually
    wc.hbrBackground = NULL;                    //transparent
    wc.lpszClassName = "CranbiWindowClass";

    if(!RegisterClassA(&wc))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    //create the window
    u32 clientX = _x;
    u32 clientY = _y;
    u32 clientWidth = _width;
    u32 clientHeight = _height;

    u32 windowX = clientX;
    u32 windowY = clientY;
    u32 windowWidth = clientWidth;
    u32 windowHeight = clientHeight;

    u32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 windowExStyle = WS_EX_APPWINDOW;

    windowStyle |= WS_MAXIMIZEBOX;
    windowStyle |= WS_MINIMIZEBOX;
    windowStyle |= WS_THICKFRAME;

    //obtain size of the border
    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

    //in this case border rect is negative
    windowX += borderRect.left;
    windowY += borderRect.top;

    //grow by the size of OS border
    windowWidth += borderRect.right - borderRect.left;
    windowHeight += borderRect.bottom - borderRect.top;

    HWND handle = CreateWindowExA(windowExStyle, "CranbiWindowClass", _appName,
    windowStyle, windowX, windowY, windowWidth, windowHeight, 
    0, 0, pState->hInstance, 0);

    if(handle == 0)
    {
        MessageBoxA(NULL, "Window creation failed", "Error", MB_ICONEXCLAMATION | MB_OK);

        LOG_FATAL("Window createion failed");
        return false;
    } 
    else { pState->hwnd = handle; }

    //show the window
    b32 shouldActivate = 1; //TODO: if the window should not accept input make this false
    i32 showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
    //if initially minimized use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    //if initially maximized use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
    ShowWindow(pState->hwnd, showWindowCommandFlags);

    //clock setup
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    pState->clockFrequency = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&pState->startTime);

    return true;
}

void PlatformShutdown(void* _state)
{
    if(pState && pState->hwnd)
    {
        DestroyWindow(pState->hwnd);
        pState = 0;
    }
}

b8 PlatformPumpMessages()
{
    if(pState)
    {
        MSG message;
        while(PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }
    return true;
}

void* PlatformAllocate(u64 _size, b8 _aligned)
{
    return malloc(_size);
}

void PlatformFree(void* _block, b8 _aligned)
{
    free(_block);
}

void* PlatformZeroMem(void* _block, u64 _size)
{
    return memset(_block, 0, _size);
}

void* PlatformCopyMem(void* _dest, const void* _src, u64 _size)
{
    return memcpy(_dest, _src, _size);
}

void* PlatformSetMem(void* _dest, i32 _value, u64 _size)
{
    return memset(_dest, _value, _size);
}

void PlatformConsoleWrite(const char* _msg, u8 _color)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(consoleHandle, levels[_color]);

    OutputDebugStringA(_msg);
    u64 length = strlen(_msg);
    LPDWORD numberWritten = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), _msg, (DWORD)length, numberWritten, 0);
}

void PlatformConsoleWriteError(const char* _msg, u8 _color)
{
    HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(consoleHandle, levels[_color]);

    OutputDebugStringA(_msg);
    u64 length = strlen(_msg);
    LPDWORD numberWritten = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), _msg, (DWORD)length, numberWritten, 0);
}

f64 PlatformGetAbsoluteTime()
{
    if(pState)
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (f64)now.QuadPart * pState->clockFrequency;
    }

    return 0;
}

void PlatformSleep(u64 _ms)
{
    Sleep(_ms);
}

void PlatformGetRequiredExtensionNames(const char*** _namesDArray)
{
    DArrayPush(*_namesDArray, &"VK_KHR_win32_surface");
}

//surface creation for vulkan
b8 PlatformCreateVulkanSurface(struct VulkanContext* _context)
{
    if(!pState)
        return false;

    VkWin32SurfaceCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    createInfo.hinstance = pState->hInstance;
    createInfo.hwnd = pState->hwnd;

    VkResult result = vkCreateWin32SurfaceKHR(_context->instance, &createInfo, _context->allocator, &pState->surface);
    if(result != VK_SUCCESS)
    {
        LOG_FATAL("Vulkan surface creation failed.");
        return false;
    }

    _context->surface = pState->surface;
    return true;
}

LRESULT CALLBACK Win32ProcessMessage(HWND _hwnd, u32 _msg, WPARAM _wparam, LPARAM _lparam)
{
    switch(_msg)
    {
        case WM_ERASEBKGND:
            //notify the OS that erasing will be handled by application to prevent flicker
            return 1;
        case WM_CLOSE:
            EventContext data = {};
            EventFire(EVENT_CODE_APPLICATION_QUIT, 0, data);
            return true;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
        {
            //get updated size of window
            RECT r;
            GetClientRect(_hwnd, &r);
            u32 width = r.right - r.left;
            u32 height = r.bottom - r.top;

            //fire the event, the applciation layer will pick this up but not handle it
            //as it should be visible for other parts of the application
            EventContext context;
            context.data.u16[0] = (u16)width;
            context.data.u16[1] = (u16)height;
            EventFire(EVENT_CODE_RESIZED, 0, context);
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            //key pressed/released
            b8 pressed = (_msg == WM_KEYDOWN || _msg == WM_SYSKEYDOWN);
            Keys key = (u16)_wparam;

            //pass to input subsystem for processing
            InputProcessKey(key, pressed);
        } break;
        case WM_MOUSEMOVE:
        {
            i32 xPosition = GET_X_LPARAM(_lparam);
            i32 yPosition = GET_Y_LPARAM(_lparam);
            
            //pass to input subsystem
            InputProcessMouseMove(xPosition, yPosition);
        } break;
        case WM_MOUSEWHEEL:
        {
            i32 zDelta = GET_WHEEL_DELTA_WPARAM(_wparam);
            if(zDelta != 0)
            {
                //flatten the input to an OS independent (-1, 1)
                zDelta = (zDelta < 0) ? -1 : 1;
                
                //pass to input subsystem
                InputProcessMouseWheel(zDelta);
            }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            b8 pressed = (_msg == WM_LBUTTONDOWN || _msg == WM_MBUTTONDOWN || _msg == WM_RBUTTONDOWN);
            Buttons mouseButton = BUTTON_MAX_BUTTONS;
            switch(_msg)
            {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouseButton = BUTTON_LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouseButton = BUTTON_MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouseButton = BUTTON_RIGHT;
                    break;
            }

            //pass to input system
            if(mouseButton != BUTTON_MAX_BUTTONS)
                InputProcessButton(mouseButton, pressed);
        } break;
    }

    return DefWindowProcA(_hwnd, _msg, _wparam, _lparam);
}

#endif // CPLATFORM_WINDOWS