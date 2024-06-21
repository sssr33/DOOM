cbuffer CBuffer : register(b0) {
	matrix model;
	matrix proj;
};

struct VertexShaderInput {
	float2 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input) {
	PixelShaderInput output;
	float4 pos = float4(input.pos, 0.0f, 1.0f);

	pos = mul(pos, model);
	pos = mul(pos, proj);

	output.pos = pos;
	output.tex = input.tex;

	return output;
}
