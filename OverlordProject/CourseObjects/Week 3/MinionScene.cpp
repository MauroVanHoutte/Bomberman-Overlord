//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MinionScene.h"
#include "SpherePrefab.h"
#include "RigidBodyComponent.h"
#include "geometry/PxSphereGeometry.h"
#include "ColliderComponent.h"
#include "PhysxManager.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "DebugRenderer.h"
#include "CubePrefab.h"

MinionScene::MinionScene(void) :
GameScene(L"MinionScene")
{}

void MinionScene::Initialize()
{
	DebugRenderer::ToggleDebugRenderer();

	Logger::LogInfo(L"Welcome, humble Minion!");

	auto sphere1 = new SpherePrefab(2);
	sphere1->AddComponent(new RigidBodyComponent());

	std::shared_ptr<physx::PxGeometry> geometry(new physx::PxSphereGeometry(2));

	sphere1->AddComponent(new ColliderComponent(geometry, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.5, 0.5, 0.5)));

	sphere1->GetTransform()->Translate(0, 20, 0);

	AddChild(sphere1);

	auto sphere2 = new SpherePrefab(2);
	sphere2->AddComponent(new RigidBodyComponent());

	sphere2->AddComponent(new ColliderComponent(geometry, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.5, 0.5, 0.5)));

	sphere2->GetTransform()->Translate(1, 10, 0);

	AddChild(sphere2);

	auto sphere3 = new SpherePrefab(2);
	sphere3->AddComponent(new RigidBodyComponent());

	sphere3->AddComponent(new ColliderComponent(geometry, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.5, 0.5, 0.5)));

	sphere3->GetTransform()->Translate(0, 30, 0);

	AddChild(sphere3);

	auto ground = new CubePrefab(100, 2, 100);
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geometryGround (new physx::PxBoxGeometry(50, 1, 50));

	ground->AddComponent(new ColliderComponent(geometryGround, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.5, 0.5, 0.5)));

	ground->GetTransform()->Translate(0, 0, 0);

	AddChild(ground);

}

void MinionScene::Update()
{
}

void MinionScene::Draw()
{
}
