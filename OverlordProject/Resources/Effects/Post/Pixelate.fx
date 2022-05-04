//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;
float gPixelSizeX;
float gPixelSizeY;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState Depth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;	
};
/// Create Rasterizer State (Backface culling) 
RasterizerState BackCulling
{
    CullMode = NONE;
};

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Position = float4(input.Position, 1);
	output.TexCoord = input.TexCoord;
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input) : SV_Target
{
	int width;
	int height;
	gTexture.GetDimensions(width, height);
	float dx = gPixelSizeX*(1.f / width);
	float dy = gPixelSizeY*(1.f / height);

	input.TexCoord.x = dx * floor(input.TexCoord.x / dx);
	input.TexCoord.y = dy * floor(input.TexCoord.y / dy);

	return gTexture.Sample(samPoint, input.TexCoord);
}


//TECHNIQUE
//---------
technique11 Shake
{
    pass P0
    {
		SetRasterizerState(BackCulling);
		SetDepthStencilState(Depth, 0);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}