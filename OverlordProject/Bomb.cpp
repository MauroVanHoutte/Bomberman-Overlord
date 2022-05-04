#include "stdafx.h"
#include "Bomb.h"
#include "ModelComponent.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "GameScene.h"
#include "BomberManCharacter.h"
#include "PhysxProxy.h"
#include "RigidBodyComponent.h"
#include "Rock.h"
#include "ColliderComponent.h"
#include "PhysxManager.h"
#include "SoundManager.h"

Bomb::Bomb(BomberManCharacter* owner, DirectX::XMFLOAT3 pos, int power, FMOD::Sound* explosion) :
	m_pOwner{ owner },
	m_pFuseParticleEmitter{ nullptr },
	m_pModel{ nullptr },
	m_Position{ pos },
	m_Power{ power },
	m_pExplosionSound{ explosion }
{
	m_Strengths.resize(4, 0);
}

void Bomb::Initialize(const GameContext& gameContext)
{
	GetTransform()->Translate(m_Position);

	if (gameContext.pMaterialManager->GetMaterial(9) == nullptr)
	{
		auto material = new DiffuseMaterial_Shadow();
		material->SetDiffuseTexture(L"./Resources/Textures/Bomb_Diffuse.png");
		material->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
		gameContext.pMaterialManager->AddMaterial(material, 9);
	}
	m_pModel = new ModelComponent(L"./Resources/Meshes/bomb.ovm");
	m_pModel->SetMaterial(9);
	auto modelObj = new GameObject();
	modelObj->AddComponent(m_pModel);
	modelObj->GetTransform()->Scale(0.15f, 0.15f, 0.15f);
	AddChild(modelObj);

	auto particleObj = new GameObject();
	m_pFuseParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Spark.png", 20);
	particleObj->AddComponent(m_pFuseParticleEmitter);
	m_pFuseParticleEmitter->SetMinSize(1.5f);
	m_pFuseParticleEmitter->SetMaxSize(2.5f);
	m_pFuseParticleEmitter->SetMinEnergy(.2f);
	m_pFuseParticleEmitter->SetMaxEnergy(.4f);
	m_pFuseParticleEmitter->SetMinSizeGrow(1.5f);
	m_pFuseParticleEmitter->SetMaxSizeGrow(2.f);
	m_pFuseParticleEmitter->SetMinEmitterRange(0.f);
	m_pFuseParticleEmitter->SetMaxEmitterRange(0.f);
	m_pFuseParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.6f));
	particleObj->GetTransform()->Translate(0, 4.5f, 0);
	AddChild(particleObj);

	auto rigidBody = new RigidBodyComponent(true);
	AddComponent(rigidBody);
	std::shared_ptr<physx::PxGeometry> geometry = std::make_shared<physx::PxSphereGeometry>(physx::PxSphereGeometry(3.f));
	auto material = PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);
	m_pCollider = new ColliderComponent(geometry, *material);
	AddComponent(m_pCollider);

	auto lamda = [this](GameObject* trigger, GameObject* other, TriggerAction action) {
		if (action == TriggerAction::LEAVE && this == trigger)
		{
			auto bomberman = dynamic_cast<BomberManCharacter*>(other);
			if (bomberman != nullptr)
			{
				this->SetIsTrigger(false);
			}
		}
	};

	SetOnTriggerCallBack(lamda);
}

void Bomb::PostInitialize(const GameContext& )
{
	m_pCollider->EnableTrigger(true);
}

void Bomb::Update(const GameContext& gameContext)
{
	if (!m_IsTrigger && m_pCollider->IsTrigger())
	{
		m_pCollider->EnableTrigger(false);
	}

	m_FuseTimer += gameContext.pGameTime->GetElapsed();
	if (m_FuseTimer > m_FuseDuration)
	{
		if (m_pModel != nullptr)
		{
			RemoveChild(m_pModel->GetGameObject());
			delete m_pModel->GetGameObject();
			m_pModel = nullptr;
			RemoveChild(m_pFuseParticleEmitter->GetGameObject());
			delete m_pFuseParticleEmitter->GetGameObject();
			m_pFuseParticleEmitter = nullptr;

			SoundManager::GetInstance()->GetSystem()->playSound(m_pExplosionSound, nullptr, false, nullptr);

			RaycastRocks();
			SetExplosionEmitters();
		}

		
		
		RaycastBombsCharacters();
		
		if (m_FuseTimer > m_FuseDuration + m_ExplosionDuration)
		{
			((BomberManCharacter*)m_pOwner)->BombDeleted();
			GetScene()->RemoveChild(this);
		}
	}
}

void Bomb::SetIsTrigger(bool trigger)
{
	m_IsTrigger = trigger;
}

void Bomb::RaycastRocks()
{
	for (size_t i = 0; i < 4; i++)
	{
		const physx::PxU32 bufferSize{ 50 };
		physx::PxRaycastHit hitBuffer[bufferSize];
		physx::PxRaycastBuffer hit{hitBuffer, bufferSize};
		auto pos = GetTransform()->GetPosition();
		physx::PxVec3 origin{ pos.x, pos.y, pos.z };
		physx::PxVec3 direction;

		switch (i)
		{
		case 0:
			direction = physx::PxVec3{ 1, 0, 0 };
			break;
		case 1:
			direction = physx::PxVec3{ -1, 0, 0 };
			break;
		case 2:
			direction = physx::PxVec3{ 0, 0, 1 };
			break;
		case 3:
			direction = physx::PxVec3{ 0, 0, -1 };
			break;
		}

		if (!GetScene()->GetPhysxProxy()->Raycast(origin, direction, m_BlockSize * m_Power, hit))
		{
			m_Strengths[i] = int(m_Power);
			continue;
		}

		m_Strengths[i] = m_Power;

		for (size_t touch = 0; touch < hit.getNbTouches(); touch++)
		{
			auto curHit = hit.getTouch(touch);
			auto rock = dynamic_cast<Rock*>(((RigidBodyComponent*)curHit.actor->userData)->GetGameObject());
			if (rock != nullptr)
			{
				m_Strengths[i] = int(curHit.distance / m_BlockSize);
				if (rock->GetType() == RockType::BREAKABLE)
				{
					rock->MarkForDeletion();
					++m_Strengths[i];
				}
				break;
			}
		}
	}
}

void Bomb::SetExplosionEmitters()
{
	AddExplosionEmitter(0.f, 0.f, 0.f);

	for (size_t i = 0; i < m_Strengths.size(); i++)
	{
		physx::PxVec3 direction;

		switch (i)
		{
		case 0:
			direction = physx::PxVec3{ 1, 0, 0 };
			break;
		case 1:
			direction = physx::PxVec3{ -1, 0, 0 };
			break;
		case 2:
			direction = physx::PxVec3{ 0, 0, 1 };
			break;
		case 3:
			direction = physx::PxVec3{ 0, 0, -1 };
			break;
		}

		for (int strength = 1; strength <= m_Strengths[i]; strength++)
		{
			physx::PxVec3 pxPos;
			pxPos = direction * float(strength) * m_BlockSize;
			AddExplosionEmitter(pxPos.x, pxPos.y, pxPos.z);
		}
	}
}

void Bomb::AddExplosionEmitter(float x, float y, float z)
{
	auto emitter = new ParticleEmitterComponent(L"./Resources/Textures/FireBall.png", 5);
	emitter->SetMinSize(4.5f);
	emitter->SetMaxSize(4.5f);
	emitter->SetMinEnergy(.2f);
	emitter->SetMaxEnergy(.4f);
	emitter->SetMinSizeGrow(1.5f);
	emitter->SetMaxSizeGrow(2.f);
	emitter->SetMinEmitterRange(0.f);
	emitter->SetMaxEmitterRange(0.f);
	auto gameObject = new GameObject();
	gameObject->AddComponent(emitter);
	
	gameObject->GetTransform()->Translate(x, y, z);


	AddChild(gameObject);
}

void Bomb::SetOff()
{
	if (m_pModel != nullptr)
	{
		m_FuseTimer = m_FuseDuration;
		RemoveChild(m_pModel->GetGameObject());
		delete m_pModel->GetGameObject();
		m_pModel = nullptr;
		RemoveChild(m_pFuseParticleEmitter->GetGameObject());
		delete m_pFuseParticleEmitter->GetGameObject();
		m_pFuseParticleEmitter = nullptr;

		SoundManager::GetInstance()->GetSystem()->playSound(m_pExplosionSound, nullptr, false, nullptr);

		RaycastRocks();
		SetExplosionEmitters();
	}
}

void Bomb::RaycastBombsCharacters()
{
	for (size_t i = 0; i < 4; i++)
	{
		const physx::PxU32 bufferSize{ 50 };
		physx::PxRaycastHit hitBuffer[bufferSize];
		physx::PxRaycastBuffer hit{ hitBuffer, bufferSize };
		auto pos = GetTransform()->GetPosition();
		physx::PxVec3 origin{ pos.x, pos.y, pos.z };
		physx::PxVec3 direction;
		float strength = (m_Strengths[i] * m_BlockSize) + m_BlockSize/2;

		if (strength < 8)
		{
			strength = 4.f;
		}

		switch (i)
		{
		case 0:
			direction = physx::PxVec3{ 1, 0, 0 };
			break;
		case 1:
			direction = physx::PxVec3{ -1, 0, 0 };
			break;
		case 2:
			direction = physx::PxVec3{ 0, 0, 1 };
			break;
		case 3:
			direction = physx::PxVec3{ 0, 0, -1 };
			break;
		}

		if (!GetScene()->GetPhysxProxy()->Raycast(origin, direction, strength, hit))
			continue;

		for (size_t touch = 0; touch < hit.getNbTouches(); touch++)
		{
			auto curTouch = hit.getTouch(touch);
			auto rigidBody = ((RigidBodyComponent*)curTouch.actor->userData);

			auto bomberman = dynamic_cast<BomberManCharacter*>(rigidBody->GetGameObject());

			if (bomberman != nullptr)
			{
				Logger::Log(LogLevel::Info, L"BombermanHit");
				bomberman->BombHit();
			}

			auto bomb = dynamic_cast<Bomb*>(rigidBody->GetGameObject());

			if (bomb != nullptr && bomb != this)
			{
				bomb->SetOff();
			}
		}
	}
}
