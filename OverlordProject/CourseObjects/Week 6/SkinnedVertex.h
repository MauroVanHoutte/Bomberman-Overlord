#pragma once

struct SkinnedVertex
{
	SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color, float blendweight1, float blendweight2)
		: transformedVertex{position, normal, color}
		, originalVertex{position, normal, color}
		, m_Blendweight1{blendweight1}
		, m_Blendweight2{blendweight2}
	{

	}

	VertexPosNormCol transformedVertex;
	VertexPosNormCol originalVertex;
	float m_Blendweight1;
	float m_Blendweight2;
};