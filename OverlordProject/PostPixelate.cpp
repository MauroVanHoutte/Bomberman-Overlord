#include "stdafx.h"
#include "PostPixelate.h"
#include "RenderTarget.h"

PostPixelate::PostPixelate(float pixelSizeX, float pixelSizeY) : PostProcessingMaterial(L"./Resources/Effects/Post/Pixelate.fx")
	, m_PixelSizeX{pixelSizeX}
	, m_PixelSizeY{pixelSizeY}
{
}

void PostPixelate::LoadEffectVariables()
{
	m_pPixelSizeX = GetEffect()->GetVariableByName("gPixelSizeX")->AsScalar();
	m_pPixelSizeY = GetEffect()->GetVariableByName("gPixelSizeY")->AsScalar();
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
}

void PostPixelate::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
	m_pPixelSizeX->SetFloat(m_PixelSizeX);
	m_pPixelSizeY->SetFloat(m_PixelSizeY);
}
