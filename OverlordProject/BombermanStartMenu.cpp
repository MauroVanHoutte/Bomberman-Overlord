#include "stdafx.h"
#include "BombermanStartMenu.h"
#include "SpriteComponent.h"
#include "GameObject.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include "BombermanScene.h"

BombermanStartMenu::BombermanStartMenu() : GameScene(L"BombermanStartMenu")
{
}

void BombermanStartMenu::Initialize()
{ 
	auto window = OverlordGame::GetGameSettings().Window;

	auto obj = new GameObject();
	auto backgroundSprite = new SpriteComponent(L"./Resources/Textures/Menu.png");
	obj->AddComponent(backgroundSprite);
	AddChild(obj);

	obj = new GameObject();
	m_p2Players = new SpriteComponent(L"./Resources/Textures/2Players.png", {0.5f, 0.5f});
	obj->AddComponent(m_p2Players);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f - 175, 0);
	AddChild(obj);

	obj = new GameObject();
	m_p3Players = new SpriteComponent(L"./Resources/Textures/3Players.png", { 0.5f, 0.5f });
	obj->AddComponent(m_p3Players);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f - 75, 0);
	AddChild(obj);

	obj = new GameObject();
	m_p4Players = new SpriteComponent(L"./Resources/Textures/4Players.png", { 0.5f, 0.5f });
	obj->AddComponent(m_p4Players);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f + 25, 0);
	AddChild(obj);

	obj = new GameObject();
	m_pQuit = new SpriteComponent(L"./Resources/Textures/quit.png", { 0.5f, 0.5f });
	obj->AddComponent(m_pQuit);
	obj->GetTransform()->Translate(window.Width / 2.f, window.Height / 2.f + 125, 0);
	AddChild(obj);
}

void BombermanStartMenu::Update()
{
	auto input = GetGameContext().pInput;

	if (m_p2Players->IsMouseHovering(GetGameContext()))
	{
		m_p2Players->SetTexture(L"./Resources/Textures/2PlayersHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON) && !input->IsMouseButtonDown(VK_LBUTTON, true))
		{
			dynamic_cast<BombermanScene*>(SceneManager::GetInstance()->GetScene(L"Bomberman"))->SetupGame(2);
			SceneManager::GetInstance()->SetActiveGameScene(L"Bomberman");
		}
	}
	else
	{
		m_p2Players->SetTexture(L"./Resources/Textures/2Players.png");
	}

	if (m_p3Players->IsMouseHovering(GetGameContext()))
	{
		m_p3Players->SetTexture(L"./Resources/Textures/3PlayersHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON) && !input->IsMouseButtonDown(VK_LBUTTON, true))
		{
			dynamic_cast<BombermanScene*>(SceneManager::GetInstance()->GetScene(L"Bomberman"))->SetupGame(3);
			SceneManager::GetInstance()->SetActiveGameScene(L"Bomberman");
		}
	}
	else
	{
		m_p3Players->SetTexture(L"./Resources/Textures/3Players.png");
	}

	if (m_p4Players->IsMouseHovering(GetGameContext()))
	{
		m_p4Players->SetTexture(L"./Resources/Textures/4PlayersHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON) && !input->IsMouseButtonDown(VK_LBUTTON, true))
		{
			dynamic_cast<BombermanScene*>(SceneManager::GetInstance()->GetScene(L"Bomberman"))->SetupGame(4);
			SceneManager::GetInstance()->SetActiveGameScene(L"Bomberman");
		}
	}
	else
	{
		m_p4Players->SetTexture(L"./Resources/Textures/4Players.png");
	}

	if (m_pQuit->IsMouseHovering(GetGameContext()))
	{
		m_pQuit->SetTexture(L"./Resources/Textures/quitHover.png");
		if (input->IsMouseButtonDown(VK_LBUTTON) && !input->IsMouseButtonDown(VK_LBUTTON, true))
		{
			PostQuitMessage(WM_QUIT);
		}
	}
	else
	{
		m_pQuit->SetTexture(L"./Resources/Textures/quit.png");
	}
}

void BombermanStartMenu::Draw()
{
}
