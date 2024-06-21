#pragma once
#include "Dx11Include.h"

#include <span>

class ShaderLoader {
public:
    static std::span<const BYTE> GetDoomBackBufferVS();
    static std::span<const BYTE> GetDoomBackBufferPS();
};
