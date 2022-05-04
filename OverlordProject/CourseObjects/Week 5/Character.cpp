
#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "CameraComponent.h"
#include "FixedCamera.h"


Character::Character(float radius, float height, float moveSpeed) : 
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0), 
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_MaxRunVelocity(50.0f), 
	m_TerminalVelocity(20),
	m_Gravity(9.81f), 
	m_RunAccelerationTime(0.3f), 
	m_JumpAccelerationTime(10.f), 
	m_RunAcceleration(m_MaxRunVelocity/m_RunAccelerationTime), 
	m_JumpAcceleration(m_Gravity/m_JumpAccelerationTime), 
	m_RunVelocity(0), 
	m_JumpVelocity(0),
	m_Velocity(0,0,0)
{}

void Character::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Create controller
	m_pController = new ControllerComponent(PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f));
	AddComponent(m_pController);
	//TODO: Add a fixed camera as child
	m_pCamera = new FixedCamera();
	AddChild(m_pCamera);
	//TODO: Register all Input Actions

	gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::FORWARD), InputTriggerState::Down, 'W'));
	gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::LEFT), InputTriggerState::Down, 'A'));
	gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::RIGHT), InputTriggerState::Down, 'D'));
	gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::BACKWARD), InputTriggerState::Down, 'S'));
	gameContext.pInput->AddInputAction(InputAction(int(CharacterMovement::JUMP), InputTriggerState::Down, ' '));

}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Set the camera as active
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
	m_pCamera->GetComponent<CameraComponent>()->SetActive();
}

void Character::Update(const GameContext& gameContext)
{
	using namespace DirectX; //gives acces to operators
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Update the character (Camera rotation, Character Movement, Character Gravity)
	

	auto look = DirectX::XMFLOAT2(0, 0);
	if (InputManager::IsMouseButtonDown(VK_LBUTTON))
	{
		const auto mouseMove = InputManager::GetMouseMovement();
		look.x = static_cast<float>(mouseMove.x);
		look.y = static_cast<float>(mouseMove.y);
	}

	if (look.x == 0 && look.y == 0)
	{
		look = InputManager::GetThumbstickPosition(false);
	}

	m_TotalYaw += look.x * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	m_TotalPitch += look.y * m_RotationSpeed * gameContext.pGameTime->GetElapsed();

	GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);

	auto move = DirectX::XMFLOAT2(0, 0);

	if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::FORWARD))
		move.y += 1;
	if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::BACKWARD))
		move.y -= 1;
	if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::LEFT))
		move.x -= 1;
	if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::RIGHT))
		move.x += 1;

	const auto forward = XMLoadFloat3(&GetTransform()->GetForward());
	const auto right = XMLoadFloat3(&GetTransform()->GetRight());
	auto velocity = XMLoadFloat3(&m_Velocity);

	auto velocityY = XMVectorGetY(velocity);

	if (move.x != 0 || move.y != 0)
	{
		auto forwardMovement = forward * move.y;
		velocity += forwardMovement * gameContext.pGameTime->GetElapsed() * m_RunAcceleration;
		velocity += right * move.x * gameContext.pGameTime->GetElapsed() * m_RunAcceleration;
		auto velocitySquared = XMVector3LengthSq(velocity);
		if (XMVectorGetX(velocitySquared) > m_MaxRunVelocity * m_MaxRunVelocity)
		{
			velocity = XMVector3Normalize(velocity);
			velocity *= m_MaxRunVelocity;
		}
	}
	else
	{
		velocity = XMVectorSetX(velocity, 0);
		velocity = XMVectorSetZ(velocity, 0);
	}

	velocity = XMVectorSetY(velocity, velocityY);

	if (!(m_pController->GetCollisionFlags().isSet(physx::PxControllerFlag::eCOLLISION_DOWN)))
	{
		m_JumpVelocity -= m_JumpAcceleration * gameContext.pGameTime->GetElapsed();

		if (m_JumpVelocity > m_TerminalVelocity)
		{
			m_JumpVelocity = m_TerminalVelocity;
		}
		if (m_JumpVelocity < -m_TerminalVelocity)
		{
			m_JumpVelocity = -m_TerminalVelocity;
		}
	}
	else
	{
		m_JumpVelocity = 0;
		if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::JUMP))
		{
			m_JumpVelocity = 0;
			velocity = XMVectorSetY(velocity, 100);
		}
		else
		{
			velocity = XMVectorSetY(velocity, 0);
		}
	}

	float finalY = XMVectorGetY(velocity) + m_JumpVelocity;
	velocity = XMVectorSetY(velocity, finalY);

	XMFLOAT3 displacement;
	XMStoreFloat3(&displacement, velocity * gameContext.pGameTime->GetElapsed());

	XMStoreFloat3(&m_Velocity, velocity);

	m_pController->Move(displacement);

}