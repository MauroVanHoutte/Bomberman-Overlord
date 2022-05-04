#include "stdafx.h"

#include "SpriteTestScene.h"
#include "GameObject.h"

#include "ContentManager.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

#define FPS_COUNTER 1

SpriteTestScene::SpriteTestScene() :
GameScene(L"SpriteTestScene"),
m_FpsInterval(FPS_COUNTER),
m_pObj(nullptr),
m_Counter(105.0f)
{}

void SpriteTestScene::Initialize()
{
	m_pObj = new GameObject();
	m_pObj->AddComponent(new SpriteComponent(L"./Resources/Textures/TestSprite.jpg", DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT4(1, 1, 1, 0.5f)));
	AddChild(m_pObj);

	m_pObj->GetTransform()->Translate(0.f, 720.f, 0.f);
	m_pObj->GetTransform()->Scale(1.f, 2.f, 1.f);
}

void SpriteTestScene::Update()
{
	const auto gameContext = GetGameContext();

	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if (m_FpsInterval >= FPS_COUNTER)
	{
		m_FpsInterval -= FPS_COUNTER;
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", gameContext.pGameTime->GetFPS());
	}

	m_pObj->GetTransform()->Rotate(0, 0, DirectX::XM_PIDIV2 * gameContext.pGameTime->GetTotal(), false);
}

void SpriteTestScene::Draw()
{}
