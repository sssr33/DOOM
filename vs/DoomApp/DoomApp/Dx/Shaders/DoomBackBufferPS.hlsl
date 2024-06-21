// contains indexes from 0 to 255. DXGI_FORMAT_R8_UNORM maps 0...255 to 0...1
Texture2D TexBackBuffer8bitIndexes : register(t0);

// contains RGBA(BGRA) colors. DXGI_FORMAT_B8G8R8A8_UNORM
// For algorithm simplicity this is texture must have height == 1 because 8bit palette always have 256 colors which is small texture(256*1)
Texture2D TexPaletteColors : register(t1);

// TexBackBuffer8bitIndexes must be sampled with point sampler because indexes must not blend by linear filter
// TexPaletteColors must be sampled with point sampler because it's a color palette and adjanced colors not connected in any way, so linear blend must not happen
SamplerState SamplerPoint : register(s0);

struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET {
	float paletteIdx = TexBackBuffer8bitIndexes.Sample(SamplerPoint, input.tex).r;
	float4 paletteColor = TexPaletteColors.Sample(SamplerPoint, float2(paletteIdx, 0.f));

	return paletteColor;

	//return float4(0, 1, 0, 1);
}
