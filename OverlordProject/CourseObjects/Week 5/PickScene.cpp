
#include "stdafx.h"
#include "PickScene.h"

#include "Components.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "Prefabs.h"
#include "GameObject.h"

PickScene::PickScene() : GameScene(L"PickScene") {}

void PickScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	GetGameContext().pGameTime->ForceElapsedUpperbound(true);

	// Create PhysX ground plane
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	
	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));
	
	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom,*bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0,0,1)))));
	AddChild(ground);


	// ADD SPHERE
	auto sphere1 = new SpherePrefab();
	sphere1->GetTransform()->Translate(0,10,0);

	// Sphere PhysX
	auto rigidbody = new RigidBodyComponent(true);
	sphere1->AddComponent(rigidbody);

	std::shared_ptr<physx::PxGeometry> spheregeom(new physx::PxSphereGeometry(1));
	sphere1->AddComponent(new ColliderComponent(spheregeom,*bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0,0,1)))));

	AddChild(sphere1);

}

void PickScene::Update()
{
	const auto gameContext = GetGameContext();
	if(gameContext.pInput->IsMouseButtonDown(VK_RBUTTON) && !gameContext.pInput->IsMouseButtonDown(VK_RBUTTON, true))
	{
		GameObject* pickedObject = gameContext.pCamera->Pick(gameContext);
		if (pickedObject != nullptr)
		{
			RemoveChild(pickedObject);
			pickedObject = nullptr;
		}
	}
}

void PickScene::Draw() {}
