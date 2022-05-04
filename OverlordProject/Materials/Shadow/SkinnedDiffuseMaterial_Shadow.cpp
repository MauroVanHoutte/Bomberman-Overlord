//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SkinnedDiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pBoneTransforms = nullptr;
ID3DX11EffectVectorVariable* SkinnedDiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;

SkinnedDiffuseMaterial_Shadow::SkinnedDiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Skinned_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{}

void SkinnedDiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	//TODO: store the diffuse texture in the appropriate member
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkinnedDiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	//TODO: store the light direction in the appropriate member0
	m_LightDirection = dir;
}

void SkinnedDiffuseMaterial_Shadow::LoadEffectVariables()
{
	//TODO: load all the necessary shader variables
	auto effect = GetEffect();
	m_pDiffuseSRVvariable = effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_pLightWVPvariable = effect->GetVariableByName("gWorldViewProj_Light")->AsMatrix();
	m_pShadowSRVvariable = effect->GetVariableByName("gShadowMap")->AsShaderResource();
	m_pLightDirectionVariable = effect->GetVariableByName("gLightDirection")->AsVector();
	m_pBoneTransforms = effect->GetVariableByName("gBones")->AsMatrix();
}

void SkinnedDiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	using namespace DirectX;
	//TODO: update all the necessary shader variables
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	XMFLOAT4X4 wvpLight;
	auto vpLight = gameContext.pShadowMapper->GetLightVP();
	auto world = pModelComponent->GetTransform()->GetWorld();
	auto matrix = XMLoadFloat4x4(&world) * XMLoadFloat4x4(&vpLight);
	XMStoreFloat4x4(&wvpLight, matrix);
	m_pLightWVPvariable->SetMatrix(*wvpLight.m);
	m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());
	m_pLightDirectionVariable->SetFloatVector(&m_LightDirection.x);
	m_pBoneTransforms->SetMatrixArray(*pModelComponent->GetAnimator()->GetBoneTransforms().data()->m, 0, pModelComponent->GetAnimator()->GetBoneTransforms().size());
}