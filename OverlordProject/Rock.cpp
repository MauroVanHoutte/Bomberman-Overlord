#include "stdafx.h"
#include "Rock.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "PhysxManager.h"
#include "ColliderComponent.h"
#include "GameScene.h"
#include "BombPickup.h"
#include "PowerPickup.h"

Rock::Rock(const DirectX::XMFLOAT3& position, RockType type)
	: m_Type{type}
	, m_Position{position}
	, m_ToBeDeleted{false}
{
}

void Rock::Initialize(const GameContext& )
{
	ModelComponent* model = new ModelComponent(L"./Resources/Meshes/Cube.ovm", true);

	switch (m_Type)
	{
	case RockType::BREAKABLE:
		model->SetMaterial(7);
		break;
	case RockType::UNBREAKABLE:
		model->SetMaterial(8);
		break;
	default:
		break;
	}
	AddComponent(model);
	GetTransform()->Translate(m_Position);
	GetTransform()->Scale(9, 9, 9);

	auto rigidComponent = new RigidBodyComponent(true);

	std::shared_ptr<physx::PxGeometry> geometry = std::make_shared<physx::PxBoxGeometry>(physx::PxBoxGeometry(4.5f, 4.5f, 4.5f));
	auto material = PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);
	ColliderComponent* collider = new ColliderComponent(geometry, *material);

	AddComponent(rigidComponent);

	AddComponent(collider);
}

void Rock::PostInitialize(const GameContext& )
{
}

void Rock::Update(const GameContext& )
{
}

void Rock::LateUpdate(const GameContext& )
{
	if (m_ToBeDeleted)
	{
		int nr = rand() % 6;

		switch (nr)
		{
		case 0:
			GetScene()->AddChild(new PowerPickup(GetTransform()->GetPosition()));
			break;
		case 1:
			GetScene()->AddChild(new BombPickup(GetTransform()->GetPosition()));
			break;
		}

		GetScene()->RemoveChild(this);
	}
}

RockType Rock::GetType()
{
	return m_Type;
}

void Rock::MarkForDeletion()
{
	m_ToBeDeleted = true;
}
