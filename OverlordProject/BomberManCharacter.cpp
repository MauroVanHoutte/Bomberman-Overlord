#include "stdafx.h"
#include "BomberManCharacter.h"
#include "ControllerComponent.h"
#include "PhysxManager.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "Materials/UberMaterial.h"
#include "GameScene.h"
#include "Bomb.h"
#include "SpriteComponent.h"
#include "OverlordGame.h" 
#include "ContentManager.h" 
#include "TextRenderer.h"
#include "SpriteFontLoader.h"
#include <Xinput.h>
#include "SoundManager.h"
#include "BombermanScene.h"

BomberManCharacter::BomberManCharacter(int playerID, float radius, float height) :
	m_Radius(radius),
	m_Height(height),
	m_pController(nullptr),
	//Running
	m_MaxRunVelocity(50.0f),
	m_SpriteStartPos{},
	m_PlayerId{playerID}
{
}

BomberManCharacter::~BomberManCharacter()
{
	m_pController->SetUserData(nullptr);
	if (!m_ModelAttached)
	{
		delete m_pModel;
	}
	auto scene = dynamic_cast<BombermanScene*>(GetScene());
	if (scene != nullptr)
	{
		scene->CharacterDied();
	}
}

void BomberManCharacter::Initialize(const GameContext& gameContext)
{
	m_pController = new ControllerComponent(PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f), 4);
	AddComponent(m_pController);

	m_pSpriteFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/stealdream.fnt");

	m_pModelObject = new GameObject();

	m_pModel = new ModelComponent(L"./Resources/Meshes/Timmy.ovm");
	m_pModel->SetMaterial(m_PlayerId);

	m_pModelObject->AddComponent(m_pModel);
	m_pModelObject->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	m_pModelObject->GetTransform()->Translate(0, -3, 0);

	AddChild(m_pModelObject);

	auto window = OverlordGame::GetGameSettings().Window;

	DirectX::XMFLOAT2 spritePivot{};
	m_Offset = 55.f;

	switch (m_PlayerId) //very hard coded, no time to make a ui system
	{
	case 1:
		spritePivot.x = 1.f;
		m_SpriteStartPos.x = (float)window.Width;
		break;
	case 2:
		spritePivot.y = 1.f;
		m_SpriteStartPos.y = (float)window.Height;
		m_Offset = -55.f;
		break;
	case 3: 
		spritePivot.x = 1.f;
		spritePivot.y = 1.f;
		m_SpriteStartPos.x = (float)window.Width;
		m_SpriteStartPos.y = (float)window.Height;
		m_Offset = -55.f;
		break;
	}

	SpriteComponent* healthSprite = new SpriteComponent(L"./Resources/Textures/HeartSprite.png", spritePivot);
	SpriteComponent* powerSprite = new SpriteComponent(L"./Resources/Textures/ExplosionSprite.png", spritePivot);
	SpriteComponent* bombSprite = new SpriteComponent(L"./Resources/Textures/BombSprite.png", spritePivot);

	GameObject* object = new GameObject();
	object->AddComponent(healthSprite);
	object->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	object->GetTransform()->Translate(m_SpriteStartPos);
	AddChild(object);
	
	auto spriteStartPos = m_SpriteStartPos;
	spriteStartPos.y += m_Offset;

	object = new GameObject();
	object->AddComponent(powerSprite);
	object->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	object->GetTransform()->Translate(spriteStartPos);
	AddChild(object);

	spriteStartPos.y += m_Offset;

	object = new GameObject();
	object->AddComponent(bombSprite);
	object->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	object->GetTransform()->Translate(spriteStartPos);	
	AddChild(object);

	SetupInput(gameContext);

	auto soundManager = SoundManager::GetInstance();

	soundManager->GetSystem()->createSound("./Resources/Sounds/Hurt.wav", FMOD_CREATESAMPLE, nullptr, &m_pHurtSound);
	soundManager->GetSystem()->createSound("./Resources/Sounds/Pickup.wav", FMOD_CREATESAMPLE, nullptr, &m_pPickupSound);
	soundManager->GetSystem()->createSound("./Resources/Sounds/PlaceBomb.wav", FMOD_CREATESAMPLE, nullptr, &m_pPlaceSound);
	soundManager->GetSystem()->createSound("./Resources/Sounds/Explosion.wav", FMOD_CREATESAMPLE, nullptr, &m_pExplosionSound);
}

void BomberManCharacter::PostInitialize(const GameContext& )
{
	m_pAnimator = m_pModel->GetAnimator();
	m_pAnimator->SetAnimation(L"idle");
	m_pAnimator->Play();
}

void BomberManCharacter::Update(const GameContext& gameContext)
{
	if (m_Invincible)
	{
		m_FlickerCounter += gameContext.pGameTime->GetElapsed();
		if (m_FlickerCounter > m_FlickerInterval)
		{		gameContext.pInput->SetVibration(1.f, 1.f, GamepadIndex(m_PlayerId));

			m_InvincibilityRemainingDuration -= m_FlickerCounter;
			m_FlickerCounter -= m_FlickerInterval;

			if (m_ModelAttached)
			{
				m_pModelObject->RemoveComponent(m_pModel);
				m_ModelAttached = false;
			}
			else
			{
				m_pModelObject->AddComponent(m_pModel);
				m_ModelAttached = true;
			}

			if (m_InvincibilityRemainingDuration < 0)
			{
				m_InvincibilityRemainingDuration = m_InvincibilityTotalDuration;
				m_Invincible = false;
				if (!m_ModelAttached)
				{
					m_pModelObject->AddComponent(m_pModel);
					m_ModelAttached = true;
				}
			}
		}
	}
	else
	{
		gameContext.pInput->SetVibration(0.0f, 0.0f, GamepadIndex(m_PlayerId));
	}

	using namespace DirectX; //gives acces to operators

	if (gameContext.pInput->IsActionTriggered((int) CharacterMovement::PLACE_BOMB + 5 * m_PlayerId) || gameContext.pInput->IsActionTriggered((int)CharacterMovement::PLACE_BOMB + 20 + 5 * m_PlayerId))
	{
		PlaceBomb();
	}

	auto move = DirectX::XMFLOAT2(0, 0);

	if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::FORWARD + 5 * m_PlayerId) || gameContext.pInput->IsActionTriggered((int)CharacterMovement::FORWARD + 20 + 5 * m_PlayerId))
	{
		move.y += 1;
		GetTransform()->Rotate(0, 180, 0);
	}
	else if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::BACKWARD + 5 * m_PlayerId) || gameContext.pInput->IsActionTriggered((int)CharacterMovement::BACKWARD + 20 + 5 * m_PlayerId))
	{
		move.y -= 1;
		GetTransform()->Rotate(0, 0, 0);
	}
	else if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::LEFT + 5 * m_PlayerId) || gameContext.pInput->IsActionTriggered((int)CharacterMovement::LEFT + 20 + 5 * m_PlayerId))
	{
		move.x -= 1;
		GetTransform()->Rotate(0, 90, 0);
	}
	else if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::RIGHT + 5 * m_PlayerId) || gameContext.pInput->IsActionTriggered((int)CharacterMovement::RIGHT + 20 + 5 * m_PlayerId))
	{
		move.x += 1;
		GetTransform()->Rotate(0, -90, 0);
	}

	XMFLOAT3 displacement{ 0,0,0 };

	if (move.x != 0 || move.y != 0)
	{
		displacement.z = move.y * gameContext.pGameTime->GetElapsed() * m_MaxRunVelocity; //the accelerating movement feels unresponsive and makes it easy to make mistakes while playing so i removed it
		displacement.x = move.x * gameContext.pGameTime->GetElapsed() * m_MaxRunVelocity;
		m_pAnimator->SetAnimation(1, false);
	}
	else
	{
		m_pAnimator->SetAnimation(0, false);
	}

	m_pController->Move(displacement);
}

void BomberManCharacter::LateUpdate(const GameContext& )
{
	if (m_ToBeDeleted)
	{
		GetScene()->RemoveChild(this);
	}
}

void BomberManCharacter::Draw(const GameContext&)
{
	auto textPos = m_SpriteStartPos; //very hard coded, no time to make a ui system
	switch (m_PlayerId)
	{
	case 0:
		textPos.x += 60.f;
		textPos.y += 5.f;
		break;
	case 2:
		textPos.x += 60.f;
		textPos.y += -45.f;
		break;
	case 1:
		textPos.x -= 65.f;
		textPos.y += 5.f;
		break;
	case 3:
		textPos.x -= 65.f;
		textPos.y += -45.f;
		break;
	}

	TextRenderer::GetInstance()->DrawText(m_pSpriteFont, std::to_wstring(m_Health), DirectX::XMFLOAT2(textPos.x, textPos.y));
	textPos.y += m_Offset;
	TextRenderer::GetInstance()->DrawText(m_pSpriteFont, std::to_wstring(m_BombPower), DirectX::XMFLOAT2(textPos.x, textPos.y));
	textPos.y += m_Offset;
	TextRenderer::GetInstance()->DrawText(m_pSpriteFont, std::to_wstring(m_MaxPlacableBombs), DirectX::XMFLOAT2(textPos.x, textPos.y));
}

void BomberManCharacter::BombDeleted()
{
	++m_PlacableBombs;
}

void BomberManCharacter::BombHit()
{
	if (!m_Invincible)
	{
		SoundManager::GetInstance()->GetSystem()->playSound(m_pHurtSound, nullptr, false, nullptr);
		m_Invincible = true;
		--m_Health;
		if (m_Health < 1)
		{
			m_ToBeDeleted = true;
		}
	}
}

void BomberManCharacter::PowerPickup()
{
	++m_BombPower;
	SoundManager::GetInstance()->GetSystem()->playSound(m_pPickupSound, nullptr, false, nullptr);
}

void BomberManCharacter::BombPickup()
{
	++m_MaxPlacableBombs;
	++m_PlacableBombs;
	SoundManager::GetInstance()->GetSystem()->playSound(m_pPickupSound, nullptr, false, nullptr);
}

void BomberManCharacter::PlaceBomb()
{
	using namespace DirectX;
	if (m_PlacableBombs < 1)
		return;
	if (SoundManager::GetInstance()->GetSystem()->playSound(m_pPlaceSound, nullptr, false, nullptr) != FMOD_OK)
	{
		return;
	}
	--m_PlacableBombs;

	auto float3pos = GetTransform()->GetPosition();
	auto position = XMLoadFloat3(&float3pos);
	auto colF = float3pos.x / 9;
	auto rowF = float3pos.z / 9;
	int col;
	int row;
	
	if (colF > 0)
		col = int(colF + 0.5f);
	else
		col = int(colF - 0.5f);

	if (rowF > 0)
		row = int(rowF + 0.5f);
	else
		row = int(rowF - 0.5f);

	
	GameObject* bomb = new Bomb(this, DirectX::XMFLOAT3(col * 9.f, 0, row * 9.f), m_BombPower, m_pExplosionSound);

	GetScene()->AddChild(bomb);
}

void BomberManCharacter::SetupInput(const GameContext& gameContext)
{
	switch (m_PlayerId)
	{
	case 0:
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + (5 * m_PlayerId), InputTriggerState::Down, 'W'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + (5 * m_PlayerId), InputTriggerState::Down, 'A'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + (5 * m_PlayerId), InputTriggerState::Down, 'D'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + (5 * m_PlayerId), InputTriggerState::Down, 'S'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + (5 * m_PlayerId), InputTriggerState::Pressed, VK_CONTROL));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_UP, GamepadIndex::PlayerOne));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT, GamepadIndex::PlayerOne));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT, GamepadIndex::PlayerOne));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, GamepadIndex::PlayerOne));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + 20 + (5 * m_PlayerId), InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_A, GamepadIndex::PlayerOne));
		break;
	case 1:
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + (5 * m_PlayerId), InputTriggerState::Down, 'Y'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + (5 * m_PlayerId), InputTriggerState::Down, 'G'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + (5 * m_PlayerId), InputTriggerState::Down, 'J'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + (5 * m_PlayerId), InputTriggerState::Down, 'H'));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + (5 * m_PlayerId), InputTriggerState::Pressed, ' '));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_UP, GamepadIndex::PlayerTwo));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT, GamepadIndex::PlayerTwo));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT, GamepadIndex::PlayerTwo));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, GamepadIndex::PlayerTwo));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + 20 + (5 * m_PlayerId), InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_A, GamepadIndex::PlayerTwo));
		break;
	case 2:
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + (5 * m_PlayerId), InputTriggerState::Down, VK_UP));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + (5 * m_PlayerId), InputTriggerState::Down, VK_LEFT));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + (5 * m_PlayerId), InputTriggerState::Down, VK_RIGHT));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + (5 * m_PlayerId), InputTriggerState::Down, VK_DOWN));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + (5 * m_PlayerId), InputTriggerState::Pressed, VK_SHIFT));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_UP, GamepadIndex::PlayerThree));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT, GamepadIndex::PlayerThree));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT, GamepadIndex::PlayerThree));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, GamepadIndex::PlayerThree));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + 20 + (5 * m_PlayerId), InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_A, GamepadIndex::PlayerThree));
		break;
	case 3:
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + (5 * m_PlayerId), InputTriggerState::Down, VK_NUMPAD8));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + (5 * m_PlayerId), InputTriggerState::Down, VK_NUMPAD4));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + (5 * m_PlayerId), InputTriggerState::Down, VK_NUMPAD6));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + (5 * m_PlayerId), InputTriggerState::Down, VK_NUMPAD5));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + (5 * m_PlayerId), InputTriggerState::Pressed, VK_RETURN));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_UP, GamepadIndex::PlayerFour));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT, GamepadIndex::PlayerFour));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT, GamepadIndex::PlayerFour));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD) + 20 + (5 * m_PlayerId), InputTriggerState::Down, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, GamepadIndex::PlayerFour));
		gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::PLACE_BOMB) + 20 + (5 * m_PlayerId), InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_A, GamepadIndex::PlayerFour));
		break;
	default:
		break;
	}

}
