#include "stdafx.h"
#include "BombermanGameOverScene.h"
#include <OverlordGame.h>
#include "GameObject.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include "BombermanScene.h" 

BombermanGameOverScene::BombermanGameOverScene() : GameScene(L"GameOverScene")
{
}

void BombermanGameOverScene::Initialize()
{
	auto window = OverlordGame::GetGameSettings().Window;

	auto obj = new GameObject();
	auto backgroundSprite = new SpriteComponent(L"./Resources/Textures/Menu.png");
	obj->AddComponent(backgroundSprite);
	AddChild(obj);

	obj = new GameObject();
	m_pMainMenu = new SpriteComponent(L"./Resources/Textures/MainMenu.png", { 0.5f, 0.5f });
	obj->AddComponent(m_pMainMenu);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f - 50, 0);
	AddChild(obj);

	obj = new GameObject();
	m_pQuit = new SpriteComponent(L"./Resources/Textures/quit.png", { 0.5f, 0.5f });
	obj->AddComponent(m_pQuit);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f + 75, 0);
	AddChild(obj);
}

void BombermanGameOverScene::Update()
{
	auto input = GetGameContext().pInput;

	if (m_pMainMenu->IsMouseHovering(GetGameContext()))
	{
		m_pMainMenu->SetTexture(L"./Resources/Textures/MainMenuHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON))
		{
			SceneManager::GetInstance()->SetActiveGameScene(L"BombermanStartMenu");
		}
	}
	else
	{
		m_pMainMenu->SetTexture(L"./Resources/Textures/MainMenu.png");
	}

	if (m_pQuit->IsMouseHovering(GetGameContext()))
	{
		m_pQuit->SetTexture(L"./Resources/Textures/quitHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON))
		{
			PostQuitMessage(WM_QUIT);
		}
	}
	else
	{
		m_pQuit->SetTexture(L"./Resources/Textures/quit.png");
	}
}

void BombermanGameOverScene::Draw()
{
}

void BombermanGameOverScene::SceneActivated()
{
	dynamic_cast<BombermanScene*>(SceneManager::GetInstance()->GetScene(L"Bomberman"))->CleanupLevel();
}
