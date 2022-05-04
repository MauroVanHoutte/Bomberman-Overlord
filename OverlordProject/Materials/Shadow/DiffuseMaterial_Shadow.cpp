//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "DiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectVectorVariable* DiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectMatrixVariable* DiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	UNREFERENCED_PARAMETER(assetFile);
	//TODO: store the diffuse texture in the appropriate member
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	UNREFERENCED_PARAMETER(dir);
	//TODO: store the light direction in the appropriate member
	m_LightDirection = dir;
}

void DiffuseMaterial_Shadow::LoadEffectVariables()
{
	//TODO: load all the necessary shader variables
	auto effect = GetEffect();
	m_pDiffuseSRVvariable = effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_pLightWVPvariable = effect->GetVariableByName("gWorldViewProj_Light")->AsMatrix();
	m_pShadowSRVvariable = effect->GetVariableByName("gShadowMap")->AsShaderResource();
	m_pLightDirectionVariable = effect->GetVariableByName("gLightDirection")->AsVector();
}

void DiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	using namespace DirectX;
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
	//TODO: update all the necessary shader variables
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	XMFLOAT4X4 wvpLight;
	auto vpLight = gameContext.pShadowMapper->GetLightVP();
	auto world = pModelComponent->GetTransform()->GetWorld();
	auto matrix =  XMLoadFloat4x4(&world) * XMLoadFloat4x4(&vpLight);
	auto vpCamera = gameContext.pCamera->GetViewProjection();
	XMStoreFloat4x4(&wvpLight, matrix);
	m_pLightWVPvariable->SetMatrix(*wvpLight.m);
	m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());
	m_pLightDirectionVariable->SetFloatVector(&m_LightDirection.x);
}