#include "stdafx.h"
#include "PauseScene.h"
#include "OverlordGame.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include "BombermanScene.h"

BombermanPauseScene::BombermanPauseScene() : GameScene(L"PauseScene")
{
}

void BombermanPauseScene::Initialize()
{
	auto window = OverlordGame::GetGameSettings().Window;

	auto obj = new GameObject();
	auto backgroundSprite = new SpriteComponent(L"./Resources/Textures/Menu.png");
	obj->AddComponent(backgroundSprite);
	AddChild(obj);

	obj = new GameObject();
	m_pResume = new SpriteComponent(L"./Resources/Textures/Resume.png", { 0.5f, 0.5f });
	obj->AddComponent(m_pResume);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f - 175, 0);
	AddChild(obj);

	obj = new GameObject();
	m_pMainMenu = new SpriteComponent(L"./Resources/Textures/MainMenu.png", { 0.5f, 0.5f });
	obj->AddComponent(m_pMainMenu);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f - 50, 0);
	AddChild(obj);

	obj = new GameObject();
	m_pRestart = new SpriteComponent(L"./Resources/Textures/Restart.png", { 0.5f, 0.5f });
	obj->AddComponent(m_pRestart);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f + 75, 0);
	AddChild(obj);

	obj = new GameObject();
	m_pQuit = new SpriteComponent(L"./Resources/Textures/quit.png", { 0.5f, 0.5f });
	obj->AddComponent(m_pQuit);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f + 200, 0);
	AddChild(obj);
}

void BombermanPauseScene::Update()
{
	auto input = GetGameContext().pInput;

	if (m_pResume->IsMouseHovering(GetGameContext()))
	{
		m_pResume->SetTexture(L"./Resources/Textures/ResumeHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON))
		{
			SceneManager::GetInstance()->SetActiveGameScene(L"Bomberman");
		}
	}
	else
	{
		m_pResume->SetTexture(L"./Resources/Textures/Resume.png");
	}

	if (m_pMainMenu->IsMouseHovering(GetGameContext()))
	{
		m_pMainMenu->SetTexture(L"./Resources/Textures/MainMenuHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON))
		{
			SceneManager::GetInstance()->SetActiveGameScene(L"BombermanStartMenu");
			dynamic_cast<BombermanScene*>(SceneManager::GetInstance()->GetScene(L"Bomberman"))->CleanupLevel();
		}
	}
	else
	{
		m_pMainMenu->SetTexture(L"./Resources/Textures/MainMenu.png");
	}

	if (m_pRestart->IsMouseHovering(GetGameContext()))
	{
		m_pRestart->SetTexture(L"./Resources/Textures/RestartHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON))
		{
			dynamic_cast<BombermanScene*>(SceneManager::GetInstance()->GetScene(L"Bomberman"))->Restart();
			SceneManager::GetInstance()->SetActiveGameScene(L"Bomberman");
		}
	}
	else
	{
		m_pRestart->SetTexture(L"./Resources/Textures/Restart.png");
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

void BombermanPauseScene::Draw()
{
}
