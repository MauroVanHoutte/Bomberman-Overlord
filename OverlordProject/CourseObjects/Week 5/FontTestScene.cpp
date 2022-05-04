//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "FontTestScene.h"
#include "Logger.h"
#include "SpriteRenderer.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "TextRenderer.h"

#define FPS_COUNTER 1

FontTestScene::FontTestScene() :
GameScene(L"FontTestScene"),
m_FpsInterval(FPS_COUNTER),
m_Counter(105.0f)
{}

void FontTestScene::Initialize()
{
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt");
}

void FontTestScene::Update()
{
	const auto gameContext = GetGameContext();

	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if (m_FpsInterval >= FPS_COUNTER)
	{
		m_FpsInterval -= FPS_COUNTER;
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", gameContext.pGameTime->GetFPS());
	}
}

void FontTestScene::Draw()
{
	const auto gameContext = GetGameContext();

	if (m_pFont->GetFontName() != L"")
	{
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Hello World!", DirectX::XMFLOAT2(10, 15), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Red));
		std::wstringstream sFps;
		sFps << L"FPS: " << gameContext.pGameTime->GetFPS();

		TextRenderer::GetInstance()->DrawText(m_pFont, sFps.str(), DirectX::XMFLOAT2(10, 50), DirectX::XMFLOAT4(DirectX::Colors::Yellow));
	}
}
