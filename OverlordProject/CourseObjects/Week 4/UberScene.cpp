#include "stdafx.h"
#include "UberScene.h"
#include "PhysxManager.h"
#include "GameObject.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "../../Materials/UberMaterial.h"
#include "ModelComponent.h"

UberScene::UberScene() : GameScene(L"UberScene")
{
}

void UberScene::Initialize()
{
	const auto gameContext = GetGameContext();

	gameContext.pGameTime->ForceElapsedUpperbound(true);

	// Create PhysX ground plane
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(ground);


	auto myMaterial = new UberMaterial();
	myMaterial->SetDiffuseTexture(L"./Resources/Textures/Chair_Dark.dds");
	myMaterial->EnableDiffuseTexture(true);
	myMaterial->EnableSpecularPhong(true);
	myMaterial->EnableEnvironmentMapping(true);
	myMaterial->SetEnvironmentCube(L"./Resources/Textures/Sunol_Cubemap.dds");
	myMaterial->EnableFresnelFaloff(true);
	gameContext.pMaterialManager->AddMaterial(myMaterial, 70);

	auto model = new ModelComponent(L"./Resources/Meshes/Teapot.ovm");
	model->SetMaterial(70);

	auto teapot = new GameObject();
	teapot->AddComponent(model);
	AddChild(teapot);
}

void UberScene::Update()
{
}

void UberScene::Draw()
{
}
