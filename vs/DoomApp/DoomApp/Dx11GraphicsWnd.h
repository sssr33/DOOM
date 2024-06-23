#pragma once

#include <atomic>
#include <vector>
#include <cstdint>
#include <winExt\InterfacePtr.h>
#include <winExt\IGraphicsWnd.h>

#include "Window.h"
#include "WindowsThreadMessageQueue.h"
#include "Dx\Dx11Device.h"
#include "Dx\Dx11HwndSwapChain.h"
#include "Dx\CBuffer.h"
#include "Input\Dx11GraphicsWndInputHandler.h"

// for additional description see comments in IGraphicsWnd/IGraphicsWndVtbl
class Dx11GraphicsWnd : public IGraphicsWnd {
public:
    static InterfacePtr<Dx11GraphicsWnd> Make();

    IRESULT __stdcall QueryInterface(
        const IGUID* guid,
        void** ppvObject);

    int32_t __stdcall AddRef();

    int32_t __stdcall Release();

    IRESULT __stdcall InitializeScreen(
        int width,
        int height,
        int bitDepth
        );

    IRESULT __stdcall GetCPUBackBuffer(
        void** buffer
        );

    IRESULT __stdcall SetPalette(
        const uint8_t* paletteIndexes,
        const uint8_t* paletteColors,
        uint32_t paletteSize
    );

    IRESULT __stdcall StartFrame();

    IRESULT __stdcall FinishScreenUpdate();

    IRESULT __stdcall TryGetNextInputEvent(
        IGraphicsWndInputEvent** evt);

private:
    class VTable {
    public:
        static VTable& Instance();

        IGraphicsWndVtbl* GetVTable();

    private:
        VTable();

        IGraphicsWndVtbl vtable = {};
    };

    Dx11GraphicsWnd();

    LRESULT WndProc(Window* window, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Draw();
    void Resize();

    void UpdateVSCBuffer();
    void UpdateDoomBackBufferTex();
    void UpdateMatrices();

    void InitDxResources();
    void InitSamplers();
    void InitShaders();
    void InitGeometry();

    std::atomic<int32_t> refCounter = 1;

    std::vector<uint8_t> cpuVideoBuf;
    uint32_t cpuVideoBufPitch = 0;
    uint32_t cpuVideoBufHeight = 0;

    Dx11Device dxDev;
    Window wnd;
    Dx11HwndSwapChain dxSwapChain;
    WindowsThreadMessageQueue wndMsgQueue;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> doomPaletteTex;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> doomBackBufferTex;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> doomBackBufferTexCpu;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> doomBackBufferSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> doomPaletteSrv;
    bool doomBackBufferUpdated = false;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> pointSampler;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> linearSampler;

    Microsoft::WRL::ComPtr<ID3D11Buffer> quadGeometry;
    D3D11_PRIMITIVE_TOPOLOGY quadGeometryTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    UINT quadGeometryVertexCount = 0;
    UINT quadGeometryVertexStride = 0;
    UINT quadGeometryVertexOffset = 0;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuffer;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> vsInputLayout;
    bool vsCBufferDataUpdated = false;
    DoomBackBufferVSCBuffer vsCBufferData = {};

    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;

    Dx11GraphicsWndInputHandler inputHandler;
};
