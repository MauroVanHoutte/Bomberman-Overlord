#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
                                               std::wstring technique)
	: m_IsInitialized(false), 
	  m_pInputLayout(nullptr),
	  m_pInputLayoutSize(0),
	  m_effectFile(std::move(effectFile)),
	  m_InputLayoutID(0),
	  m_RenderIndex(renderIndex),
	  m_pRenderTarget(nullptr),
	  m_pVertexBuffer(nullptr),
	  m_pIndexBuffer(nullptr),
	  m_NumVertices(0),
	  m_NumIndices(0),
	  m_VertexBufferStride(sizeof(VertexPosTex)),
	  m_pEffect(nullptr),
	  m_pTechnique(nullptr),
	  m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources
	SafeRelease(m_pInputLayout);
	SafeDelete(m_pRenderTarget);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (!m_IsInitialized)
	{
		//TODO: complete
		//1. LoadEffect (LoadEffect(...))
		LoadEffect(gameContext, m_effectFile);
		//2. CreateInputLaytout (CreateInputLayout(...))
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutSize);
		CreateVertexBuffer(gameContext);
		CreateIndexBuffer(gameContext);
		//3. Create RenderTarget (m_pRenderTarget)
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()
		auto settings = OverlordGame::GetGameSettings();
		RENDERTARGET_DESC desc{};
		desc.Width = settings.Window.Width;
		desc.Height = settings.Window.Height;
		desc.EnableColorSRV = true;
		desc.GenerateMipMaps_Color = true;
		m_pRenderTarget = new RenderTarget(gameContext.pDevice);
		m_pRenderTarget->Create(desc);
		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const std::wstring& effectFile)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(effectFile);

	//TODO: complete
	//Load Effect through ContentManager
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	//Check if m_TechniqueName (default constructor parameter) is set
	if (m_TechniqueName.empty())
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByName(LPCSTR(m_TechniqueName.c_str()));
	}
	// If SET > Use this Technique (+ check if valid)
	// If !SET > Use Technique with index 0

	//Call LoadEffectVariables
	LoadEffectVariables();
	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(previousRendertarget);

	//TODO: complete
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	m_pRenderTarget->Clear(gameContext, DirectX::Colors::CornflowerBlue);
	
	//2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRendertarget);
	//3. Set InputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	//4. Set VertexBuffer
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//5. Set PrimitiveTopology (TRIANGLELIST)
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//6. Draw
	D3DX11_TECHNIQUE_DESC desc;
	m_pTechnique->GetDesc(&desc);
	for (size_t i = 0; i < desc.Passes; i++)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	m_NumVertices = 4;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//Create vertex array containing three elements in system memory
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	//*************
	//VERTEX BUFFER
	//fill a buffer description to copy the vertexdata into graphics memory
	//create a ID3D10Buffer in graphics memory containing the vertex info
	D3D11_BUFFER_DESC vertexBuffDesc;
	vertexBuffDesc.ByteWidth = sizeof(VertexPosTex) * m_NumVertices;
	vertexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBuffDesc.MiscFlags = 0;

	VertexPosTex vertexArray[4]
	{
		VertexPosTex(DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f))
		, VertexPosTex(DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f))
		, VertexPosTex(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f))
		, VertexPosTex(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f))
	};

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexArray;

	gameContext.pDevice->CreateBuffer(&vertexBuffDesc, &data, &m_pVertexBuffer);
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	m_NumIndices = 6;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	// Create index buffer
	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();

	D3D11_BUFFER_DESC indexBuffDesc;
	indexBuffDesc.ByteWidth = sizeof(UINT) * m_NumIndices;
	indexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	indexBuffDesc.MiscFlags = 0;

	//Make indices
	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indices;

	gameContext.pDevice->CreateBuffer(&indexBuffDesc, &data, &m_pIndexBuffer);
}
