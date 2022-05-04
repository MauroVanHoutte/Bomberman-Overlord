#include "stdafx.h"

#include "PostProcessingScene.h"
#include "GameObject.h"

#include "ContentManager.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "SpriteFont.h"
#include "TextRenderer.h"

#include "../../Materials/DiffuseMaterial.h"
#include "PostBlur.h"
#include "PostGrayscale.h"

PostProcessingScene::PostProcessingScene(void) :
	GameScene(L"PostProcessingScene"),
	m_pModel(nullptr),
	m_pFont(nullptr)
{}

void PostProcessingScene::Initialize()
{
	const auto gameContext = GetGameContext();

	auto pDiffuseMaterial = new DiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Knight.jpg");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial, 0);

	m_pModel = new ModelComponent(L"./Resources/Meshes/Timmy.ovm");
	m_pModel->SetMaterial(0);
	auto obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);
	obj->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt");

	AddPostProcessingEffect(new PostBlur());
	AddPostProcessingEffect(new PostGrayscale());
}

void PostProcessingScene::Update()
{}

void PostProcessingScene::Draw()
{
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Post Processing", DirectX::XMFLOAT2(10, 10), (DirectX::XMFLOAT4) DirectX::Colors::Red);
}
