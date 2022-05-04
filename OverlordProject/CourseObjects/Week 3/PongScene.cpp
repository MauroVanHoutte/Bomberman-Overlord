#include "stdafx.h"
#include "PongScene.h"
#include "CubePrefab.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "PhysxManager.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

PongScene::PongScene() : GameScene(L"PongScene")
{
}

void PongScene::Initialize()
{
	auto camera = new GameObject();
	camera->AddComponent(new CameraComponent());

	SetActiveCamera(camera->GetComponent<CameraComponent>());

	camera->GetTransform()->Translate(0, 50, 0);
	camera->GetTransform()->Rotate(90, 0, 0);

	AddChild(camera);

	auto topWall = new CubePrefab(100, 2, 2, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
	topWall->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geometryWall(new physx::PxBoxGeometry(50, 1, 1));

	topWall->AddComponent(new ColliderComponent(geometryWall, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.f, 0.f, 0.f)));

	topWall->GetTransform()->Translate(0, 2, 22);

	AddChild(topWall);

	auto botWall = new CubePrefab(100, 2, 2, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
	botWall->AddComponent(new RigidBodyComponent(true));

	botWall->AddComponent(new ColliderComponent(geometryWall, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.f, 0.f, 0.f)));

	botWall->GetTransform()->Translate(0, 2, -21);

	AddChild(botWall);

	m_pBall = new CubePrefab(2, 2, 2, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
	m_pBall->AddComponent(new RigidBodyComponent(false));

	std::shared_ptr<physx::PxGeometry> geometryBall(new physx::PxBoxGeometry(1, 1, 1));

	m_pBall->AddComponent(new ColliderComponent(geometryBall, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.f, 0.f, 2.f)));

	m_pBall->GetTransform()->Translate(0, 2, 0);

	AddChild(m_pBall);

	auto ballRigidBody = m_pBall->GetComponent<RigidBodyComponent>();
	ballRigidBody->SetConstraint(RigidBodyConstraintFlag::TransX, false);
	ballRigidBody->SetConstraint(RigidBodyConstraintFlag::TransZ, false);
	ballRigidBody->SetConstraint(RigidBodyConstraintFlag::TransY, true);
	ballRigidBody->SetConstraint(RigidBodyConstraintFlag::RotY, true);
	ballRigidBody->SetConstraint(RigidBodyConstraintFlag::RotX, true);
	ballRigidBody->SetConstraint(RigidBodyConstraintFlag::RotZ, true);
	ballRigidBody->SetDensity(0.01f);
	m_pBall->GetComponent<RigidBodyComponent>()->AddForce(physx::PxVec3(0.5f, 0, 1.5f), physx::PxForceMode::eIMPULSE);



	m_LeftPaddle = new CubePrefab(2, 2, 8, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
	m_LeftPaddle->AddComponent(new RigidBodyComponent(false));
	auto leftPaddleRigidBody = m_LeftPaddle->GetComponent<RigidBodyComponent>();
	leftPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::TransX, true);
	leftPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::TransY, true);
	leftPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::TransZ, false);
	leftPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::RotX, true);
	leftPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::RotY, true);
	leftPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::RotZ, true);
	//leftPaddleRigidBody->SetDensity(100.f);

	std::shared_ptr<physx::PxGeometry> geometryPaddle(new physx::PxBoxGeometry(1, 1, 4));

	m_LeftPaddle->AddComponent(new ColliderComponent(geometryPaddle, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.f, 0.f, 0.f)));
	
	m_LeftPaddle->GetTransform()->Translate(-32, 2, 0);

	AddChild(m_LeftPaddle);

	m_RightPaddle = new CubePrefab(2, 2, 8, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
	m_RightPaddle->AddComponent(new RigidBodyComponent(false));
	auto rightPaddleRigidBody = m_RightPaddle->GetComponent<RigidBodyComponent>();
	rightPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::TransX, true);
	rightPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::TransY, true);
	rightPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::TransZ, false);
	rightPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::RotX, true);
	rightPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::RotY, true);
	rightPaddleRigidBody->SetConstraint(RigidBodyConstraintFlag::RotZ, true);
	//rightPaddleRigidBody->SetDensity(100.f);



	m_RightPaddle->AddComponent(new ColliderComponent(geometryPaddle, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.f, 0.f, 0.f)));

	m_RightPaddle->GetTransform()->Translate(32, 2, 5);

	AddChild(m_RightPaddle);


	auto ground = new CubePrefab(100, 2, 100, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geometryGround(new physx::PxBoxGeometry(50, 1, 50));

	ground->AddComponent(new ColliderComponent(geometryGround, *PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.f, 0.f, 0.f)));

	ground->GetTransform()->Translate(0, 0, 0);

	AddChild(ground);
}

void PongScene::Update()
{
	auto input = GetGameContext().pInput;
	bool inputLeft{ false };
	bool inputRight{ false };
	if (input->IsKeyboardKeyDown('W'))
	{
		m_LeftPaddle->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0,0, 10));
		inputLeft = true;
	}
	if (input->IsKeyboardKeyDown('S'))
	{
		m_LeftPaddle->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 0, -10));
		inputLeft = true;
	}
	if (input->IsKeyboardKeyDown('T'))
	{
		m_RightPaddle->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 0, 10));
		inputRight = true;
	}
	if (input->IsKeyboardKeyDown('G'))
	{
		m_RightPaddle->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 0, -10));
		inputRight = true;
	}

	if (!inputLeft)
	{
		m_LeftPaddle->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 0, 0));
	}
	if (!inputRight)
	{
		m_RightPaddle->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 0, 0));
	}

	auto ballRigidBody = m_pBall->GetComponent<RigidBodyComponent>();
	auto velocity = ballRigidBody->GetPxRigidBody()->getLinearVelocity();
	velocity.normalize();
	ballRigidBody->AddForce(velocity/10, physx::PxForceMode::eFORCE);
}

void PongScene::Draw()
{
}
