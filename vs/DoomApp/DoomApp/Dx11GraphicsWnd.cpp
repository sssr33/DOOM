#include "stdafx.h"
#include "Dx11GraphicsWnd.h"
#include "vtable_bind.h"
#include "HResultException.h"
#include "Dx\ShaderLoader.h"
#include "Dx\Vertices.h"

#include <cassert>

InterfacePtr<Dx11GraphicsWnd> Dx11GraphicsWnd::Make() {
    return InterfacePtr<Dx11GraphicsWnd>(new Dx11GraphicsWnd, DoNotAddRefKey{});
}

IRESULT __stdcall Dx11GraphicsWnd::QueryInterface(
    const IGUID* guid,
    void** ppvObject)
{
    return IRESULT_ERROR;
}

int32_t __stdcall Dx11GraphicsWnd::AddRef() {
    return ++this->refCounter;
}

int32_t __stdcall Dx11GraphicsWnd::Release() {
    auto refc = --this->refCounter;

    if (refc == 0) {
        delete this;
    }

    return refc;
}

IRESULT __stdcall Dx11GraphicsWnd::InitializeScreen(
    int width,
    int height,
    int bitDepth
)
{
    if (bitDepth != 8) {
        // for now only 8 bit palette colors supported as should be in original Doom
        assert(false);
        return IRESULT_ERROR;
    }

    this->cpuVideoBufPitch = static_cast<uint32_t>(width * (bitDepth / 8));
    this->cpuVideoBufHeight = static_cast<uint32_t>(height);
    this->cpuVideoBuf.resize(this->cpuVideoBufPitch * this->cpuVideoBufHeight);

    HRESULT hr = S_OK;
    auto& dev = this->dxDev.GetD3DDevice();

    D3D11_TEXTURE2D_DESC texDesc = {};

    texDesc.Width = static_cast<UINT>(width);
    texDesc.Height = static_cast<UINT>(height);
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    hr = dev->CreateTexture2D(&texDesc, nullptr, &this->doomBackBufferTex);
    if (FAILED(hr)) {
        return IRESULT_ERROR;
    }

    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.BindFlags = 0;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = dev->CreateTexture2D(&texDesc, nullptr, &this->doomBackBufferTexCpu);
    if (FAILED(hr)) {
        return IRESULT_ERROR;
    }

    hr = dev->CreateShaderResourceView(this->doomBackBufferTex.Get(), nullptr, &this->doomBackBufferSrv);
    if (FAILED(hr)) {
        return IRESULT_ERROR;
    }

    this->UpdateMatrices();

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWnd::GetCPUBackBuffer(
    void** buffer
)
{
    if (!buffer) {
        return IRESULT_ERROR;
    }

    *buffer = this->cpuVideoBuf.data();

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWnd::SetPalette(
    const uint8_t* paletteIndexes,
    const uint8_t* paletteColors,
    uint32_t paletteSize
)
{
    if (paletteSize > D3D_FL9_1_REQ_TEXTURE1D_U_DIMENSION) {
        assert(false);
        // If it fails try to use D3D_FL9_3_REQ_TEXTURE1D_U_DIMENSION or greated limits
        // Or try to implement rectangle/square palette size
        return IRESULT_ERROR;
    }

    struct BGRA {
        uint8_t b = 0;
        uint8_t g = 0;
        uint8_t r = 0;
        uint8_t a = 255;
    };

    std::vector<BGRA> texData;

    texData.reserve(paletteSize);

    for (uint32_t i = 0; i < paletteSize; ++i) {
        BGRA color;

        color.r = paletteColors[paletteIndexes[i * 3 + 0]];
        color.g = paletteColors[paletteIndexes[i * 3 + 1]];
        color.b = paletteColors[paletteIndexes[i * 3 + 2]];

        texData.push_back(color);
    }

    HRESULT hr = S_OK;
    auto& dev = this->dxDev.GetD3DDevice();

    D3D11_TEXTURE2D_DESC texDesc = {};

    texDesc.Width = paletteSize;
    texDesc.Height = 1;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA texInitData = {};

    texInitData.pSysMem = texData.data();
    texInitData.SysMemPitch = static_cast<UINT>(sizeof(BGRA) * texDesc.Width);
    texInitData.SysMemSlicePitch = texInitData.SysMemPitch * texDesc.Height;

    hr = dev->CreateTexture2D(&texDesc, &texInitData, &this->doomPaletteTex);
    if (FAILED(hr)) {
        return IRESULT_ERROR;
    }

    hr = dev->CreateShaderResourceView(this->doomPaletteTex.Get(), nullptr, &this->doomPaletteSrv);
    if (FAILED(hr)) {
        return IRESULT_ERROR;
    }

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWnd::StartFrame() {
    this->wndMsgQueue.ProcessQueuedMessages();

    if (this->hasFocus) {
        // set cursor to window center to keep rceive mouse deltas from center
        // This allows to use mouse for moving camera in windowed mode without directinput or other similar apis
        const auto& d3dViewport = this->dxSwapChain.GetD3DViewport();

        POINT point;

        point.x = static_cast<LONG>(d3dViewport.Width / 2);
        point.y = static_cast<LONG>(d3dViewport.Height / 2);

        ClientToScreen(this->wnd.GetHwnd(), &point);

        SetCursorPos(point.x, point.y);
    }

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWnd::FinishScreenUpdate() {
    try {
        // set flag only here to update doomBackBufferTex only when game says it's done drawing
        // On window resizes doomBackBufferTex not need to be updated
        this->doomBackBufferUpdated = true;

        this->Draw();
    }
    catch (...) {
        // check logic
        assert(false);
        return IRESULT_ERROR;
    }

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWnd::TryGetNextInputEvent(
    IGraphicsWndInputEvent** evt)
{
    return this->inputHandler.TryGetNextInputEvent(evt);
}

Dx11GraphicsWnd::Dx11GraphicsWnd()
    : wnd(L"DoomDx11GraphicsWnd", L"Doom")
    , dxSwapChain(this->wnd.GetHwnd(), this->dxDev.GetD3DDevice(), this->dxDev.GetD2DContext())
{
    this->vtable = VTable::Instance().GetVTable();

    this->wnd.SetCustomWndProcHandler(
        std::bind(
            &Dx11GraphicsWnd::WndProc,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            std::placeholders::_5
        )
    );

    this->inputHandler.SetWindowToGameCoords(
        [this](const DirectX::XMINT2& windowCoords)
        {
            DirectX::XMINT2 center;
            const auto& d3dViewport = this->dxSwapChain.GetD3DViewport();

            center.x = static_cast<int32_t>(d3dViewport.Width / 2);
            center.y = static_cast<int32_t>(d3dViewport.Height / 2);

            DirectX::XMINT2 move;

            move.x = windowCoords.x - center.x;
            move.y = windowCoords.y - center.y;

            // game calculates mouse delta movement so actual numbers should not matter
            // Using window coords gives good sensitivity, so no need to transform to doom actual resolution
            this->globalMouseCoords.x += move.x;
            this->globalMouseCoords.y += move.y;

            return this->globalMouseCoords;
        });

    this->InitDxResources();
}

LRESULT Dx11GraphicsWnd::WndProc(Window* window, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    try {
        switch (uMsg) {
            // https://stackoverflow.com/questions/21816323/smooth-window-resizing-in-windows-using-direct2d-1-1
            // https://stackoverflow.com/questions/53000291/how-to-smooth-ugly-jitter-flicker-jumping-when-resizing-windows-especially-drag
            // https://github.com/bigfatbrowncat/noflicker_directx_window
        case WM_NCCALCSIZE: {
            // Use the result of DefWindowProc's WM_NCCALCSIZE handler to get the upcoming client rect.
            // Technically, when wparam is TRUE, lparam points to NCCALCSIZE_PARAMS, but its first
            // member is a RECT with the same meaning as the one lparam points to when wparam is FALSE.
            DefWindowProc(this->wnd.GetHwnd(), uMsg, wParam, lParam);
            if (RECT* rect = (RECT*)lParam; rect->right > rect->left && rect->bottom > rect->top) {
                // A real app might want to compare these dimensions with the current swap chain
                // dimensions and skip all this if they're unchanged.
                UINT width = rect->right - rect->left;
                UINT height = rect->bottom - rect->top;

                this->dxSwapChain.Resize(
                    DirectX::XMUINT2(width, height),
                    this->dxDev.GetD3DDevice(),
                    this->dxDev.GetD3DContext(),
                    this->dxDev.GetD2DContext());

                this->Resize();
            }

            return 0;
        }
                          //case WM_NCPAINT: // can be used to remove border
        case WM_ERASEBKGND:
        case WM_PAINT: {
            RECT updateRegion = {};
            if (!GetUpdateRect(this->wnd.GetHwnd(), &updateRegion, FALSE)) {
                return 0;
            }

            this->Draw();

            if (!ValidateRect(this->wnd.GetHwnd(), &updateRegion)) {
                assert(false);
            }

            return 0;
        }
        case WM_KILLFOCUS:
            this->hasFocus = false;
            break;
        case WM_SETFOCUS:
            this->hasFocus = true;
            break;
        default: {
            if (this->inputHandler.HandleInputEvent(uMsg, wParam, lParam)) {
                return 0;
            }

            break;
        }
        }
    }
    catch (...) {
        // check logic
        assert(false);
    }

    return DefWindowProc(this->wnd.GetHwnd(), uMsg, wParam, lParam);
}

void Dx11GraphicsWnd::Draw() {
    this->UpdateVSCBuffer();
    this->UpdateDoomBackBufferTex();

    const auto& d3dCtx = this->dxDev.GetD3DContext();
    const auto& d3dTarget = this->dxSwapChain.GetD3DRenderTargetView();

    d3dCtx->RSSetViewports(1, &this->dxSwapChain.GetD3DViewport());

    d3dCtx->OMSetRenderTargets(1, d3dTarget.GetAddressOf(), nullptr);

    {
        auto clearColor = D2D1::ColorF(D2D1::ColorF::Black);
        FLOAT clearColorD3D[] =
        {
            clearColor.r,
            clearColor.g,
            clearColor.b,
            clearColor.a
        };

        d3dCtx->ClearRenderTargetView(d3dTarget.Get(), clearColorD3D);
    }

    d3dCtx->IASetVertexBuffers(
        0,
        1,
        this->quadGeometry.GetAddressOf(),
        &this->quadGeometryVertexStride,
        &this->quadGeometryVertexOffset);
    d3dCtx->IASetPrimitiveTopology(this->quadGeometryTopology);
    d3dCtx->IASetInputLayout(this->vsInputLayout.Get());

    d3dCtx->VSSetShader(this->vs.Get(), nullptr, 0);
    d3dCtx->VSSetConstantBuffers(0, 1, this->vsCBuffer.GetAddressOf());

    d3dCtx->PSSetShader(this->ps.Get(), nullptr, 0);
    d3dCtx->PSSetSamplers(0, 1, this->pointSampler.GetAddressOf());
    {
        ID3D11ShaderResourceView* srvs[] = { this->doomBackBufferSrv.Get(), this->doomPaletteSrv.Get() };
        d3dCtx->PSSetShaderResources(0, static_cast<UINT>(std::size(srvs)), srvs);
    }

    d3dCtx->Draw(this->quadGeometryVertexCount, 0);

    //auto d2dTarget = this->dxSwapChain.GetD2DRenderTargetBitmap();
    //auto d2dCtx = this->dxDev.GetD2DContext();

    //d2dCtx->BeginDraw();
    //d2dCtx->SetTarget(d2dTarget.Get());

    ////d2dCtx->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));

    //d2dCtx->Clear(D2D1::ColorF(D2D1::ColorF::BlueViolet, 0.3f));

    //Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

    //auto greenBrush = d2dCtx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSeaGreen, 0.5f), &brush);

    //static float x = 0.f;

    //x += 0.5f;

    //d2dCtx->DrawLine(D2D1::Point2F(100.f + x, 100.f), D2D1::Point2F(400.f, 200.f), brush.Get(), 10.f);

    //d2dCtx->EndDraw();

    this->dxSwapChain.Present(this->dxDev.GetD3DContext());
}

void Dx11GraphicsWnd::Resize() {
    this->UpdateMatrices();
}

void Dx11GraphicsWnd::UpdateVSCBuffer() {
    if (!this->vsCBufferDataUpdated) {
        return;
    }

    const auto& d3dCtx = this->dxDev.GetD3DContext();

    d3dCtx->UpdateSubresource1(
        this->vsCBuffer.Get(),
        0,
        NULL,
        &this->vsCBufferData,
        0,
        0,
        0
    );

    this->vsCBufferDataUpdated = false;
}

void Dx11GraphicsWnd::UpdateDoomBackBufferTex() {
    if (!this->doomBackBufferUpdated) {
        return;
    }

    HRESULT hr = S_OK;
    const auto& d3dCtx = this->dxDev.GetD3DContext();

    D3D11_MAPPED_SUBRESOURCE mappedRes = {};

    hr = d3dCtx->Map(this->doomBackBufferTexCpu.Get(), 0, D3D11_MAP_WRITE, 0, &mappedRes);
    if (FAILED(hr)) {
        // check if happens
        assert(false);
        return;
    }

    {
        auto mappedData = static_cast<uint8_t*>(mappedRes.pData);
        auto mappedHeight = mappedRes.DepthPitch / mappedRes.RowPitch;

        if (mappedHeight == this->cpuVideoBufHeight) {
            if (mappedRes.RowPitch == this->cpuVideoBufPitch) {
                std::copy(this->cpuVideoBuf.begin(), this->cpuVideoBuf.end(), mappedData);
            }
            else {
                auto src = this->cpuVideoBuf.data();
                auto dst = mappedData;
                auto copySize = (std::min)(mappedRes.RowPitch, this->cpuVideoBufPitch);

                if (this->cpuVideoBufPitch > mappedRes.RowPitch) {
                    // check if happens
                    assert(false);
                }

                for (uint32_t y = 0; y < this->cpuVideoBufHeight; ++y, src += this->cpuVideoBufPitch, dst += mappedRes.RowPitch) {
                    std::copy(src, src + copySize, dst);
                }
            }
        }
        else {
            // check if happens
            assert(false);
        }
    }

    d3dCtx->Unmap(this->doomBackBufferTexCpu.Get(), 0);

    d3dCtx->CopyResource(this->doomBackBufferTex.Get(), this->doomBackBufferTexCpu.Get());

    this->doomBackBufferUpdated = false;
}

void Dx11GraphicsWnd::UpdateMatrices() {
    const auto& d3dViewport = this->dxSwapChain.GetD3DViewport();
    float aspectRatio = d3dViewport.Width / d3dViewport.Height;
    const float projHeight = 2.f;
    float projWidth = aspectRatio * projHeight;

    auto projMatrix = DirectX::XMMatrixOrthographicLH(projWidth, projHeight, 0.1f, 10.f);

    DirectX::XMStoreFloat4x4(&this->vsCBufferData.proj, DirectX::XMMatrixTranspose(projMatrix));

    float quadWidth = 1.f;
    float quadHeight = 1.f;

    if (this->doomBackBufferTex) {
        D3D11_TEXTURE2D_DESC texDesc = {};

        this->doomBackBufferTex->GetDesc(&texDesc);

        quadWidth = static_cast<float>(texDesc.Width);
        quadHeight = static_cast<float>(texDesc.Height);
    }

    // inscribe quad size into proj size
    {
        float heightScale = projHeight / quadHeight;

        quadWidth *= heightScale;
        quadHeight *= heightScale;

        if (quadWidth > projWidth) {
            float widthScale = projWidth / quadWidth;

            quadWidth *= widthScale;
            quadHeight *= widthScale;
        }
    }

    auto modelMatrix =
        DirectX::XMMatrixScaling(quadWidth, quadHeight, 1.f) *
        DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    DirectX::XMStoreFloat4x4(&this->vsCBufferData.model, DirectX::XMMatrixTranspose(modelMatrix));

    this->vsCBufferDataUpdated = true;
}

void Dx11GraphicsWnd::InitDxResources() {
    this->InitSamplers();
    this->InitShaders();
    this->InitGeometry();
}

void Dx11GraphicsWnd::InitSamplers() {
    HRESULT hr = S_OK;
    auto& dev = this->dxDev.GetD3DDevice();
    D3D11_SAMPLER_DESC desc = {};

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.MinLOD = std::numeric_limits<float>::lowest();
    desc.MaxLOD = (std::numeric_limits<float>::max)();
    desc.MipLODBias = 0.f;
    desc.MaxAnisotropy = 1;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.BorderColor[0] = 1.f;
    desc.BorderColor[1] = 1.f;
    desc.BorderColor[2] = 1.f;
    desc.BorderColor[3] = 1.f;

    hr = dev->CreateSamplerState(&desc, &this->linearSampler);
    Helpers::ThrowIfFailed(hr);

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    hr = dev->CreateSamplerState(&desc, &this->pointSampler);
    Helpers::ThrowIfFailed(hr);
}

void Dx11GraphicsWnd::InitShaders() {
    HRESULT hr = S_OK;
    auto& dev = this->dxDev.GetD3DDevice();

    auto vsCode = ShaderLoader::GetDoomBackBufferVS();

    hr = dev->CreateVertexShader(vsCode.data(), static_cast<SIZE_T>(vsCode.size()), nullptr, &this->vs);
    Helpers::ThrowIfFailed(hr);

    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = dev->CreateInputLayout(
        vertexDesc,
        static_cast<UINT>(std::size(vertexDesc)),
        vsCode.data(),
        static_cast<SIZE_T>(vsCode.size()),
        &this->vsInputLayout);
    Helpers::ThrowIfFailed(hr);

    this->UpdateMatrices();

    CD3D11_BUFFER_DESC constantBufferDesc(sizeof(this->vsCBufferData), D3D11_BIND_CONSTANT_BUFFER);

    hr = dev->CreateBuffer(&constantBufferDesc, nullptr, &this->vsCBuffer);
    Helpers::ThrowIfFailed(hr);

    auto psCode = ShaderLoader::GetDoomBackBufferPS();

    hr = dev->CreatePixelShader(psCode.data(), static_cast<SIZE_T>(psCode.size()), nullptr, &this->ps);
    Helpers::ThrowIfFailed(hr);
}

void Dx11GraphicsWnd::InitGeometry() {
    HRESULT hr = S_OK;
    auto& dev = this->dxDev.GetD3DDevice();

    static const Vertex2DTexCoords quadGeometry[] =
    {
        //triangle strip quad
        { { -0.5f, 0.5f }, { 0.f, 0.f } }, // left top
        { { 0.5f, 0.5f }, { 1.f, 0.f } }, // right top
        { { -0.5f, -0.5f }, { 0.f, 1.f } }, // left bottom
        { { 0.5f, -0.5f }, { 1.f, 1.f } }, // right bottom
    };

    CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(quadGeometry), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);

    D3D11_SUBRESOURCE_DATA subResData = {};

    subResData.pSysMem = &quadGeometry;

    hr = dev->CreateBuffer(&vertexBufferDesc, &subResData, &this->quadGeometry);
    Helpers::ThrowIfFailed(hr);

    this->quadGeometryTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    this->quadGeometryVertexCount = static_cast<UINT>(std::size(quadGeometry));
    this->quadGeometryVertexStride = static_cast<UINT>(sizeof(quadGeometry[0]));
}




Dx11GraphicsWnd::VTable& Dx11GraphicsWnd::VTable::Instance() {
    static VTable instance;
    return instance;
}

IGraphicsWndVtbl* Dx11GraphicsWnd::VTable::GetVTable() {
    return &this->vtable;
}

Dx11GraphicsWnd::VTable::VTable() {
    vtable_bind_base<Dx11GraphicsWnd>(this->vtable);
    vtable_bind<&Dx11GraphicsWnd::InitializeScreen>(this->vtable.InitializeScreen);
    vtable_bind<&Dx11GraphicsWnd::GetCPUBackBuffer>(this->vtable.GetCPUBackBuffer);
    vtable_bind<&Dx11GraphicsWnd::SetPalette>(this->vtable.SetPalette);
    vtable_bind<&Dx11GraphicsWnd::StartFrame>(this->vtable.StartFrame);
    vtable_bind<&Dx11GraphicsWnd::FinishScreenUpdate>(this->vtable.FinishScreenUpdate);
    vtable_bind<&Dx11GraphicsWnd::TryGetNextInputEvent>(this->vtable.TryGetNextInputEvent);
}
