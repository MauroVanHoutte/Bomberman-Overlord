float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;
float4x4 gBones[70];

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 BoneIndices : BLENDINDICES;
	float4 BoneWeights : BLENDWEIGHTS;
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//TODO: complete Vertex Shader 
	//Hint: use the previously made shaders PosNormTex3D_Shadow and PosNormTex3D_Skinned as a guide
	float4 originalPosition = float4(input.pos, 1);
	float4 transformedPosition = 0;
	float3 transformedNormal = 0;

	//Skinning Magic...

	for (int i = 0; i < 4; ++i)
	{
		//if the bone index is > -1 the vertex is attached to a bone
		if (input.BoneIndices[i] > -1)
		{
			transformedPosition += input.BoneWeights[i] * mul(originalPosition, gBones[input.BoneIndices[i]]);
			transformedNormal += input.BoneWeights[i] * mul(input.normal, (float3x3)gBones[input.BoneIndices[i]]);
		}
	}
	transformedPosition.w = 1;
	
	output.pos = mul ( transformedPosition, gWorldViewProj ); 
	output.normal = normalize(mul(transformedNormal, (float3x3)gWorld)); 

	output.texCoord = input.texCoord;
	output.lPos = mul(transformedPosition, gWorldViewProj_Light);
	return output;
}

float EvaluateShadowMap(float4 lpos)
{
	//TODO: complete
	return gShadowMap.Sample(samLinear, lpos.xy).r;
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
        input.lPos.z < 0.0f  || input.lPos.z > 1.0f ) return diffuseColour * 0.3f;

	input.lPos.x = input.lPos.x/2 + 0.5;
    	input.lPos.y = input.lPos.y/-2 + 0.5;
		
	float shadowValue = EvaluateShadowMap(input.lPos);
	
	input.lPos.z -= gShadowMapBias; 
	//if clip space z value greater than shadow map value then pixel is in shadow
	if (shadowValue < input.lPos.z)
		return diffuseColour * 0.3f;

	float3 colour_rgb = diffuseColour.rgb;
	float colour_a = diffuseColour.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	colour_rgb = colour_rgb * diffuseStrength;
	
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

