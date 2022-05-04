#include "stdafx.h"
#include "BombPickup.h"
#include "ParticleEmitterComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "PhysxManager.h"
#include "BomberManCharacter.h"
#include "GameScene.h"

BombPickup::BombPickup(const DirectX::XMFLOAT3& position)
	: m_Position{ position }
{
}

void BombPickup::Initialize(const GameContext& )
{
	auto pos = m_Position;
	pos.y += 3.f;
	GetTransform()->Translate(pos);

	auto rigidBody = new RigidBodyComponent(true);
	AddComponent(rigidBody);
	auto physxMaterial = PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);
	std::shared_ptr<physx::PxGeometry> geometry = std::make_shared<physx::PxSphereGeometry>(physx::PxSphereGeometry(3.f));
	auto collider = new ColliderComponent(geometry, *physxMaterial);
	collider->EnableTrigger(true);
	AddComponent(collider);

	auto lamda = [this](GameObject* trigger, GameObject* other, TriggerAction action) {
		if (action == TriggerAction::ENTER && this == trigger && !this->IsMarkedForDeletion())
		{
			auto bomberman = dynamic_cast<BomberManCharacter*>(other);
			if (bomberman != nullptr)
			{
				bomberman->BombPickup();
				this->MarkForDeletion();
			}
		}
	};

	SetOnTriggerCallBack(lamda);

	auto particleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/BombSprite.png", 1);
	particleEmitter->SetMinSize(4.5f);
	particleEmitter->SetMaxSize(4.5f);
	particleEmitter->SetMinEnergy(1.f);
	particleEmitter->SetMaxEnergy(1.f);
	particleEmitter->SetMinSizeGrow(0.f);
	particleEmitter->SetMaxSizeGrow(0.f);
	particleEmitter->SetMinEmitterRange(0.f);
	particleEmitter->SetMaxEmitterRange(0.f);
	particleEmitter->SetFade(false);
	particleEmitter->SetRotate(false);

	AddComponent(particleEmitter);
}

void BombPickup::PostInitialize(const GameContext& )
{
}

void BombPickup::Update(const GameContext& )
{
}

void BombPickup::LateUpdate(const GameContext& )
{
	if (m_ToBeDeleted)
	{
		GetScene()->RemoveChild(this);
	}
}

void BombPickup::MarkForDeletion()
{
	m_ToBeDeleted = true;
}

bool BombPickup::IsMarkedForDeletion()
{
	return m_ToBeDeleted;
}