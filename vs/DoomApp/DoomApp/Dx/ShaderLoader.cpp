#include "ShaderLoader.h"
#include <DoomBackBufferVS_data.h>
#include <DoomBackBufferPS_data.h>

std::span<const BYTE> ShaderLoader::GetDoomBackBufferVS() {
    return std::span<const BYTE>(DoomBackBufferVS_data);
}

std::span<const BYTE> ShaderLoader::GetDoomBackBufferPS() {
    return std::span<const BYTE>(DoomBackBufferPS_data);
}
