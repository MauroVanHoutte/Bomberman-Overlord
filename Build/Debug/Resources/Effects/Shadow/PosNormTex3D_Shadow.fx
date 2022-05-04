float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.0002f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

float2 texOffset(int u, int v)
{
	return float2(u * 1.0f / 1280, v * 1.0f / 720);
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	//TODO: complete Vertex Shader
	//Hint: Don't forget to project our position to light clip space and store it in lPos
	output.pos = mul(float4(input.pos, 1.0f), gWorldViewProj);
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	output.texCoord = input.texCoord;
	output.lPos = mul(float4(input.pos, 1.0f), gWorldViewProj_Light);
	return output;
}

float EvaluateShadowMap(float4 lpos)
{
	//TODO: complete
	return gShadowMap.Sample(samPoint, lpos.xy).r;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//re-homogenize position after interpolation
	input.lPos.xyz /= input.lPos.w;
	
	float4 diffuseColour = gDiffuseMap.Sample(samLinear, input.texCoord);

	//if position is not visible to the light - dont illuminate it
    if( input.lPos.x < -1.0f || input.lPos.x > 1.0f ||
		input.lPos.y < -1.0f || input.lPos.y > 1.0f ||
        input.lPos.z < 0.0f  || input.lPos.z > 1.0f ) return diffuseColour * 0.15f;

	input.lPos.x = input.lPos.x/2 + 0.5;
    input.lPos.y = input.lPos.y/-2 + 0.5;
		
	
	input.lPos.z += gShadowMapBias;

	float sum = 0;
	float x, y;

	for (y = -1.5; y <= 1.5; y += 1.0)
	{
		for (x = -1.5; x <= 1.5; x += 1.0)
		{
			sum += gShadowMap.SampleCmpLevelZero(cmpSampler, input.lPos.xy + texOffset(x, y), input.lPos.z);
		}
	}

	float shadowFactor = sum / 16.0;
	//if clip space z value greater than shadow map value then pixel is in shadow
	

	float3 colour_rgb = diffuseColour.rgb;
	float colour_a = diffuseColour.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	colour_rgb = colour_rgb * diffuseStrength * shadowFactor;
	
	return float4(colour_rgb, colour_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

