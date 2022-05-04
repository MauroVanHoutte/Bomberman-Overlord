#include "stdafx.h"

#include "ModelTestScene.h"
#include "GameObject.h"
#include "Components.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "../../Materials/DiffuseMaterial.h"


ModelTestScene::ModelTestScene() :
GameScene(L"ModelTestScene"),
m_pChair(nullptr)
{}

void ModelTestScene::Initialize()
{
	const auto gameContext = GetGameContext();

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	//CHAIR OBJECT
	//************
	m_pChair = new GameObject();

	//1. Attach a modelcomponent (chair.ovm)
	auto model = new ModelComponent(L"./Resources/Meshes/Teapot.ovm");
	m_pChair->AddComponent(model);
	m_pChair->GetTransform()->Translate(0, 0, 20);

	//2. Create a ColorMaterial and add it to the material manager
	auto myMaterial = new DiffuseMaterial();
	myMaterial->SetDiffuseTexture(L"./Resources/Textures/Chair_Dark.dds");
	gameContext.pMaterialManager->AddMaterial(myMaterial, 61);

	//3. Assign the material to the previous created modelcomponent
	model->SetMaterial(61);

	AddChild(m_pChair);
	// Build and Run

	//4. Create a DiffuseMaterial (using PosNormTex3D.fx)
	//		Make sure you are able to set a texture (DiffuseMaterial::SetDiffuseTexture(const wstring& assetFile))
	//		Load the correct shadervariable and set it during the material variable update
	//5. Assign the material to the modelcomponent
	// Build and Run

	//6. Attach a rigidbody component (pure-dynamic)
	//7. Attach a collider component (Use a PxConvexMeshGeometry [chair.ovpc])
	// Build and Run
}

void ModelTestScene::Update()
{}

void ModelTestScene::Draw()
{}
