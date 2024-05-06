#include "main.h"
#include "sound.h"
#include "input.h"
#include <Windows.h>
#include <xaudio2.h>
#include <math.h>
#include "platform.h"
#include "win32_sound.h"

struct Win32BackBuffer
{
    BITMAPINFO Info;
    void* Memory;
    i32 Width;
    i32 Height;
    i32 BytesPerPixel;
    i32 RowStride;
};

struct WindowDimension
{
    i32 Width;
    i32 Height;
};

struct Win32GameCode
{
    HMODULE GameCodeDLL;
    GameUpdateAndRender_t* GameUpdateAndRender;
    GameStartUp_t* GameStartUp;
    FILETIME LastWriteTime;
    bool IsValid;
};

global_var bool running;
global_var Win32BackBuffer g_BackBuffer;
global_var bool audioBusy = false;

internal WindowDimension Win32GetWindowDimension(HWND window)
{
    RECT clientRect;
    GetClientRect(window, &clientRect);
    i32 width = clientRect.right - clientRect.left;
    i32 height = clientRect.bottom - clientRect.top;
    return { width, height };
}

inline FILETIME Win32GetLastWriteTime(char* filename)
{
    FILETIME lastWriteTime = {};
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
    {
        lastWriteTime = data.ftLastWriteTime;
    }
    
    return lastWriteTime;
}

internal void Win32UnloadGameCode(Win32GameCode* gameCode)
{
    if (gameCode->GameCodeDLL)
    {
        FreeLibrary(gameCode->GameCodeDLL);
        gameCode->GameCodeDLL = 0;
    }
    gameCode->GameStartUp = GameStartUpStub;
    gameCode->GameUpdateAndRender = GameUpdateAndRenderStub;
    gameCode->IsValid = false;
}

internal Win32GameCode Win32LoadGameCode(char* filename)
{
    Win32GameCode result = {};
    
    result.LastWriteTime = Win32GetLastWriteTime(filename);
    result.GameCodeDLL = LoadLibraryA(filename);
    if (result.GameCodeDLL)
    {
        result.GameStartUp = (GameStartUp_t*)GetProcAddress(result.GameCodeDLL, "GameStartUp");
        result.GameUpdateAndRender = (GameUpdateAndRender_t*)GetProcAddress(result.GameCodeDLL, "GameUpdateAndRender");
        result.IsValid = result.GameStartUp && result.GameUpdateAndRender;
    }
    
    if (!result.IsValid)
    {
        result.GameUpdateAndRender = GameUpdateAndRenderStub;
        result.GameStartUp = GameStartUpStub;
    }
    return result;
}

internal void Win32ResizeBackBuffer(Win32BackBuffer* buffer, i32 width, i32 height)
{
    if (buffer->Memory)
    {
        VirtualFree(buffer->Memory, 0, MEM_RELEASE);
    }

    buffer->Width = width;
    buffer->Height = height;
    buffer->BytesPerPixel = 4;
    buffer->RowStride = buffer->Width * buffer->BytesPerPixel;

    buffer->Info.bmiHeader.biSize = sizeof(buffer->Info.bmiHeader);
    buffer->Info.bmiHeader.biWidth = buffer->Width;
    buffer->Info.bmiHeader.biHeight = -buffer->Height;
    buffer->Info.bmiHeader.biPlanes = 1;
    buffer->Info.bmiHeader.biBitCount = 32;
    buffer->Info.bmiHeader.biCompression = BI_RGB;

    i32 bitmapMemorySize = (buffer->Width * buffer->Height) * buffer->BytesPerPixel;
    buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32UpdateWindowDisplay(HDC deviceContext, Win32BackBuffer* buffer,
                                       i32 windowWidth, i32 windowHeight)
{
    PatBlt(deviceContext, 0, buffer->Height, buffer->Width, windowHeight - buffer->Height, BLACKNESS);
    PatBlt(deviceContext, buffer->Width, 0, windowWidth - buffer->Width, windowHeight, BLACKNESS);
    /*StretchDIBits(deviceContext,
                  0, 0, windowWidth, windowHeight,
                  0, 0, buffer->Width, buffer->Height,
                  buffer->Memory,
                  &buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);*/
    StretchDIBits(deviceContext,
                  0, 0, buffer->Width, buffer->Height,
                  0, 0, buffer->Width, buffer->Height,
                  buffer->Memory,
                  &buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

internal void Win32UpdateAudio(GameSoundOutput* sound, Win32SoundUpdate* update)
{
    if ((update->FrequencyRatio >= XAUDIO2_MIN_FREQ_RATIO) && (update->FrequencyRatio <= XAUDIO2_MAX_FREQ_RATIO))
    {
        if (FAILED(sourceVoice->SetFrequencyRatio(update->FrequencyRatio)))
        {
            OutputDebugStringA("Failed to change sound frequency\n");
        }
    }
    if ((update->VolumeLevel >= (-XAUDIO2_MAX_VOLUME_LEVEL)) && (update->VolumeLevel <= XAUDIO2_MAX_VOLUME_LEVEL))
    {
        if (FAILED(sourceVoice->SetVolume(update->VolumeLevel)))
        {
            OutputDebugStringA("Failed to change sound volume\n");
        }
    }
    /*if (WaitForSingleObjectEx(bufferEndEvent, 0, TRUE) == WAIT_OBJECT_0)
    {
        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes = sound->Size;
        buffer.pAudioData = (BYTE*)sound->Samples;
        if (FAILED(sourceVoice->SubmitSourceBuffer(&buffer)))
        {
            OutputDebugStringA("Failed to submit XAUDIO2_BUFFER\n");
        }
    }*/
}

POUND_PLATFORM_FREE_FILE_MEMORY(PlatformFreeFileMemory)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

POUND_PLATFORM_LOAD_FILE(PlatformLoadFile)
{
    void* memory = 0;
    HANDLE file = CreateFileA(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
    if (file == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(file, &fileSize))
    {
        CloseHandle(file);
        return 0;
    }
    Assert(fileSize.QuadPart <= 0xffffffff);
    u32 fileSize32 = (u32)fileSize.QuadPart;
    memory = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (memory)
    {
        DWORD read;
        if (ReadFile(file, memory, fileSize32, &read, 0) && (read == fileSize32))
        {
        }
        else
        {
            PlatformFreeFileMemory(memory);
            memory = 0;
        }
    }
    CloseHandle(file);
    return memory;
}

internal void Win32ProcessMessages(HWND window, GameInput* input)
{
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(window, &mousePos);
    input->MousePositionX = mousePos.x;
    input->MousePositionY = mousePos.y;
    input->MouseLeft = (GetKeyState(VK_LBUTTON) & (1 << 15)) != 0;
    input->MouseRight = (GetKeyState(VK_RBUTTON) & (1 << 15)) != 0;
    input->MouseMiddle = (GetKeyState(VK_MBUTTON) & (1 << 15)) != 0;
    input->MouseSideX = (GetKeyState(VK_XBUTTON1) & (1 << 15)) != 0;
    input->MouseSideY = (GetKeyState(VK_XBUTTON2) & (1 << 15)) != 0;
    MSG message = {};
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            {
                u32 VKCode = (u32)message.wParam;
                bool wasAltDown = (message.lParam & (1 << 29)) != 0;
                if (wasAltDown && (VKCode == VK_F4))
                {
                    OutputDebugStringA("ALT-F4 pressed!\n");
                    running = false;
                }
            }
            break;
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 VKCode = (u32)message.wParam;
                bool isDown = (message.lParam & (1u << 31)) == 0;
                bool wasDown = (message.lParam & (1 << 30)) != 0;
                if (isDown != wasDown)
                {

                    if (VKCode == 'W')
                    {
                        input->KeyW = isDown;
                    }
                    else if (VKCode == 'A')
                    {
                        input->KeyA = isDown;
                    }
                    else if (VKCode == 'S')
                    {
                        input->KeyS = isDown;
                    }
                    else if (VKCode == 'D')
                    {
                        input->KeyD = isDown;
                    }
                    else if (VKCode == 'Q')
                    {
                        input->KeyQ = isDown;
                    }
                    else if (VKCode == 'E')
                    {
                        input->KeyE = isDown;
                    }
                    else if (VKCode == VK_UP)
                    {
                        input->KeyUp = isDown;
                    }
                    else if (VKCode == VK_DOWN)
                    {
                        input->KeyDown = isDown;
                    }
                    else if (VKCode == VK_LEFT)
                    {
                        input->KeyLeft = isDown;
                    }
                    else if (VKCode == VK_RIGHT)
                    {
                        input->KeyRight = isDown;
                    }
                    else if (VKCode == VK_SPACE)
                    {
                        input->KeySpace = isDown;
                    }
                    else if (VKCode == VK_SHIFT)
                    {
                        input->KeyShift = isDown;
                    }
                    else if (VKCode == VK_CONTROL)
                    {
                        input->KeyCtrl = isDown;
                    }
                    else if (VKCode == VK_ESCAPE)
                    {
                        input->KeyEsc = isDown;
                    }
                    else if (VKCode == VK_RETURN)
                    {
                        input->KeyEnter = isDown;
                    }
                }
            }
            break;
            default:
            {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            }
            break;
        }
    }
}

LRESULT Win32MainWindowCallBack(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message) 
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        }
        break;
        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
            running = false;
        }
        break;
        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
            running = false;
        }
        break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            OutputDebugStringA("Input's not allow to process here\n");
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);

            WindowDimension dim = Win32GetWindowDimension(window);
            Win32UpdateWindowDisplay(deviceContext, &g_BackBuffer, dim.Width, dim.Height);
            
            EndPaint(window, &paint);
        }
        break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }
        break;
        default:
        {
            result = DefWindowProcA(window, message, wParam, lParam);
        }
        break;
    }
    return result;
}

global_var i64 g_PerfCounterFrequency;

inline LARGE_INTEGER Win32GetWallClock()
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

inline f32 Win32GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    return (f32)(end.QuadPart - start.QuadPart) / (f32)g_PerfCounterFrequency;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, PSTR cmdLine, int cmdShow)
{   
    WNDCLASSA windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = Win32MainWindowCallBack;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "HandmadeHeroWindowClass";

    char* dllName = "HandmadeHeroLib.dll";
    Win32GameCode game = Win32LoadGameCode(dllName);

    UINT desiredSchedulerMilliseconds = 1;
    bool granularSleep = timeBeginPeriod(desiredSchedulerMilliseconds);

    LARGE_INTEGER PerfCounterFrequencyResult;
    QueryPerformanceFrequency(&PerfCounterFrequencyResult);
    g_PerfCounterFrequency = PerfCounterFrequencyResult.QuadPart;

    Win32ResizeBackBuffer(&g_BackBuffer, 960, 540);

    if (RegisterClassA(&windowClass))
    {
        HWND windowHandle = CreateWindowExA(
            0,
            windowClass.lpszClassName,
            "Handmade Hero",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            instance,
            0);
        if (windowHandle)
        {
            u32 monitorRefreshRate = 60;
            HDC refreshDC = GetDC(windowHandle);
            u32 Win32RefreshRate = GetDeviceCaps(refreshDC, VREFRESH);
            ReleaseDC(windowHandle, refreshDC);
            if (Win32RefreshRate > 1)
            {
                monitorRefreshRate = Win32RefreshRate;
            }
            u32 gameFPS = 30;
            f32 targetSecondsPerFrame = 1.0f / (f32)gameFPS;
            LARGE_INTEGER lastCount = Win32GetWallClock();
            u64 lastCycleCount = __rdtsc();

            LPVOID baseAddress = 0;
            GameMemory gameMemory = {};
            gameMemory.PermanentSize = Megabytes(64);
            gameMemory.TransientSize = Gigabytes(2ull);
            u64 TotalSize = gameMemory.PermanentSize + gameMemory.TransientSize;
            gameMemory.PermanentMemory = VirtualAlloc(baseAddress, TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            gameMemory.TransientMemory = (u8*)gameMemory.PermanentMemory + gameMemory.PermanentSize;
            gameMemory.PlatformLoadFile = PlatformLoadFile;
            gameMemory.PlatformFreeFileMemory = PlatformFreeFileMemory;

            GameSoundOutput soundOutput = {};
            
            Win32SoundUpdate win32SoundUpdate = {};
            win32SoundUpdate.FrequencyRatio = 1.0f;
            win32SoundUpdate.VolumeLevel = 1.0f;
            SoundUpdate soundUpdate = {};

            RenderUpdate renderUpdate = {};

            if (!gameMemory.PermanentMemory || !gameMemory.TransientMemory)
            {
                running = false;
            }
            else
            {
                running = true;
                game.GameStartUp(&gameMemory, &soundOutput);

                Win32SourceVoiceSettings settings = {};
                settings.WaveFormat = soundOutput.AudioFormat;
                settings.Channels = soundOutput.NumberOfChannels;
                settings.SampleRate = soundOutput.SampleRate;
                settings.BitsPerSample = soundOutput.BitsPerSample;

                Win32InitXAudio2(&settings);
                if (soundOutput.Samples)
                {
                    XAUDIO2_BUFFER buffer = {};
                    buffer.AudioBytes = soundOutput.Size;
                    buffer.pAudioData = (BYTE*)soundOutput.Samples;
                    buffer.Flags = XAUDIO2_END_OF_STREAM;
                    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
                    if (SUCCEEDED(sourceVoice->SubmitSourceBuffer(&buffer)))
                    {
                        if (SUCCEEDED(sourceVoice->Start()))
                        {
                        }
                    }
                }
            }
            
            GameInput gameInput = {};
            
            while (running)
            {
                /*FILETIME newLastWriteTime = Win32GetLastWriteTime(dllName);
                if (CompareFileTime(&newLastWriteTime, &game.LastWriteTime) != 0)
                {
                    Win32UnloadGameCode(&game);
                    Win32LoadGameCode(dllName);
                }*/
                gameInput.DeltaTime = targetSecondsPerFrame;
                Win32ProcessMessages(windowHandle, &gameInput);

                GameBackBuffer backBuffer = {};
                backBuffer.Memory = g_BackBuffer.Memory;
                backBuffer.Width = g_BackBuffer.Width;
                backBuffer.Height = g_BackBuffer.Height;
                backBuffer.BytesPerPixel = g_BackBuffer.BytesPerPixel;
                backBuffer.RowStride = g_BackBuffer.RowStride;
                game.GameUpdateAndRender(&gameMemory, &backBuffer, &soundOutput, &gameInput, &renderUpdate, &soundUpdate);

                // 10 change == 1 octave
                win32SoundUpdate.FrequencyRatio += win32SoundUpdate.FrequencyRatio >= 1 
                    ? (f32)soundUpdate.FrequencyChange * 0.1f 
                    : (f32)soundUpdate.FrequencyChange * 0.05f * win32SoundUpdate.FrequencyRatio;
                soundUpdate.FrequencyChange = 0;

                u64 endCycleCount = __rdtsc();
                LARGE_INTEGER endCount = Win32GetWallClock();
                f32 elapsedSeconds = Win32GetSecondsElapsed(lastCount, endCount);
                if (elapsedSeconds < targetSecondsPerFrame)
                {
                    DWORD sleepMilliseconds = (DWORD)(1000.0f * (targetSecondsPerFrame - elapsedSeconds));
                    Sleep(sleepMilliseconds);
                }
                endCount = Win32GetWallClock();
                elapsedSeconds = Win32GetSecondsElapsed(lastCount, endCount);

                Win32UpdateAudio(&soundOutput, &win32SoundUpdate);

                HDC dc = GetDC(windowHandle);
                WindowDimension dim = Win32GetWindowDimension(windowHandle);
                Win32UpdateWindowDisplay(dc, &g_BackBuffer, dim.Width, dim.Height);
                ReleaseDC(windowHandle, dc);
#if 0
                i32 elapsedMilliseconds = (i32)(1000.0f * elapsedSeconds);
                u32 elapsedMegaCycle = (u32)((endCycleCount - lastCycleCount) / 1000000);
                i32 fps = (i32)(g_PerfCounterFrequency / (endCount.QuadPart - lastCount.QuadPart));

                char buf[256];
                wsprintfA(buf, "Time: %dms - %dfps - %d million cycles\n", elapsedMilliseconds, fps, elapsedMegaCycle);
                OutputDebugStringA(buf);
#endif
                lastCycleCount = endCycleCount;
                lastCount = endCount;
            }
            if (soundOutput.Samples)
            {
                VirtualFree(soundOutput.Samples, 0, MEM_RELEASE);
            }
            if (gameMemory.PermanentMemory)
            {
                VirtualFree(gameMemory.PermanentMemory, 0, MEM_RELEASE);
            }
        }
        else
        {
            // TODO: log fail
        }
    }
    else
    {
        //TODO: Log fail
    }

    return 0;
}
