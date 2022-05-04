#include "stdafx.h"
#include "CharacterScene.h"
#include "GameObject.h"
#include "ControllerComponent.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "TransformComponent.h"
#include "Character.h"
#include "CubePrefab.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"

CharacterScene::CharacterScene() : GameScene(L"CharacterScene")
{
}

void CharacterScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	GameObject* character = new Character();
	AddChild(character);
	character->GetTransform()->Translate(0, 10, 0);
	auto ground = new CubePrefab(100, 2, 100, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geometryGround(new physx::PxBoxGeometry(50, 1, 50));

	ground->AddComponent(new ColliderComponent(geometryGround, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.f, 0.f, 0.f)));

	ground->GetTransform()->Translate(0, 0, 0);

	AddChild(ground);
}

void CharacterScene::Update()
{
}

void CharacterScene::Draw()
{
}
