#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	SafeDelete(m_pShadowMat);
	SafeDelete(m_pShadowRT);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: create shadow generator material + initialize it
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);
	//TODO: create a rendertarget with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC
	RENDERTARGET_DESC desc{};
	desc.EnableColorBuffer = false;
	desc.EnableDepthSRV = true;
	desc.Width = 1280;
	desc.Height = 720;
	m_pShadowRT = new RenderTarget(gameContext.pDevice);
	m_pShadowRT->Create(desc);

	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	using namespace DirectX;
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(direction);
	//TODO: store the input parameters in the appropriate datamembers
	m_LightPosition = position;
	m_LightDirection = direction;
	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember

	auto loadedPosition = XMLoadFloat3(&position);
	auto loadedDirection = XMLoadFloat3(&direction);
	loadedPosition = XMVectorSetW(loadedPosition, 1.f);
	loadedDirection = XMVectorSetW(loadedDirection, 0.f);

	loadedDirection = XMVector3Normalize(loadedDirection);

	XMVECTOR worldUp{ 0, 1, 0 };
	auto rightVector = XMVector3Cross(loadedDirection, worldUp);
	auto upVector = XMVector3Cross(rightVector, loadedDirection);
	
	auto focusPoint = loadedPosition + loadedDirection;

	upVector = XMVector3Normalize(upVector);

	XMMATRIX viewMatrix = XMMatrixLookToLH(loadedPosition, loadedDirection, upVector);

	auto windowSetting = OverlordGame::GetGameSettings().Window;

	XMMATRIX projection = XMMatrixOrthographicLH(m_Size * windowSetting.AspectRatio, m_Size, 1.f, 600.f);

	XMStoreFloat4x4(&m_LightVP, viewMatrix * projection);

}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	//TODO: clear this render target
	m_pShadowRT->Clear(gameContext, { 0 });
	//TODO: set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	UNREFERENCED_PARAMETER(world);
	UNREFERENCED_PARAMETER(bones);

	//TODO: update shader variables in material
	m_pShadowMat->SetBones(&bones.data()->m[0][0], bones.size());
	m_pShadowMat->SetWorld(world);
	//TODO: set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)
	ShadowMapMaterial::ShadowGenType type;
	if (pMeshFilter->m_HasAnimations)
		type = ShadowMapMaterial::ShadowGenType::Skinned;
	else
		type = ShadowMapMaterial::ShadowGenType::Static;

	VertexBufferData buffer = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[int(type)]);
	UINT offset = 0;

	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &buffer.pVertexBuffer, &buffer.VertexStride, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[int(type)]);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//TODO: invoke draw call

	D3DX11_TECHNIQUE_DESC desc;
	m_pShadowMat->m_pShadowTechs[int(type)]->GetDesc(&desc);
	for (size_t i = 0; i < desc.Passes; i++)
	{
		m_pShadowMat->m_pShadowTechs[int(type)]->GetPassByIndex(0)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
	if (pMeshFilter->m_HasAnimations)
		pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[ShadowMapMaterial::ShadowGenType::Skinned], m_pShadowMat->m_InputLayoutSizes[ShadowMapMaterial::ShadowGenType::Skinned], m_pShadowMat->m_InputLayoutDescriptions[ShadowMapMaterial::ShadowGenType::Skinned]);
	else
		pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[ShadowMapMaterial::ShadowGenType::Static], m_pShadowMat->m_InputLayoutSizes[ShadowMapMaterial::ShadowGenType::Static], m_pShadowMat->m_InputLayoutDescriptions[ShadowMapMaterial::ShadowGenType::Static]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}
