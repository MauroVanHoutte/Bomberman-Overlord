#include "stdafx.h"

#include "SpriteComponent.h"
 #include <utility>

#include "GameObject.h"
#include "TextureData.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TransformComponent.h"

SpriteComponent::SpriteComponent(std::wstring spriteAsset, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color):
	m_pTexture(nullptr),
	m_SpriteAsset(std::move(spriteAsset)),
	m_Pivot(pivot),
	m_Color(color)
{
}

void SpriteComponent::Initialize(const GameContext& )
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

bool SpriteComponent::IsMouseHovering(const GameContext& gameContext)
{
	auto mousePos = gameContext.pInput->GetMousePosition();

	auto textureDimension = m_pTexture->GetDimension();

	auto pos = GetGameObject()->GetTransform()->GetPosition();
	if (mousePos.x > pos.x - (textureDimension.x / 2.f) && 
		mousePos.x < pos.x + (textureDimension.x / 2.f) && 
		mousePos.y > pos.y - (textureDimension.y / 2.f) && 
		mousePos.y < pos.y + (textureDimension.y / 2.f))
	{
		return true;
	}

	return false;
}

void SpriteComponent::Update(const GameContext& )
{
}

void SpriteComponent::Draw(const GameContext& )
{
	if (!m_pTexture)
		return;

	//TODO: Here you need to draw the SpriteComponent using the Draw of the sprite renderer
	// The sprite renderer is a singleton
	// you will need to position, the rotation and the scale
	// You can use the QuaternionToEuler function to help you with the z rotation 

	auto* gameObject{ GetGameObject() };
	auto transformComponent = gameObject->GetTransform();
	DirectX::XMFLOAT2 position{ transformComponent->GetPosition().x, transformComponent->GetPosition().y };
	const float rotation{ QuaternionToEuler(gameObject->GetTransform()->GetRotation()).z };

	

	SpriteRenderer::GetInstance()->Draw(m_pTexture, position, m_Color, m_Pivot, { transformComponent->GetScale().x,transformComponent->GetScale().y }, rotation, transformComponent->GetPosition().z);
}
