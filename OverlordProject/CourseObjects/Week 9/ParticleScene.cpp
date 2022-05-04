#include "stdafx.h"

#include "ParticleScene.h"
#include "GameObject.h"

#include "ModelAnimator.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "ModelComponent.h"
#include "../../Materials/ColorMaterial.h"

ParticleScene::ParticleScene() :
	GameScene(L"ParticleScene"),
	m_pParticleEmitter(nullptr),
	m_pModel(nullptr)
{}

void ParticleScene::Initialize()
{
	const auto gameContext = GetGameContext();

	//gameContext.pInput->ForceMouseToCenter(true);
	gameContext.pGameTime->ForceElapsedUpperbound(true);

	auto obj = new GameObject();

	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Smoke.png", 60);
	m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(0, 6.0f, 0));
	m_pParticleEmitter->SetMinSize(1.0f);
	m_pParticleEmitter->SetMaxSize(2.0f);
	m_pParticleEmitter->SetMinEnergy(1.0f);
	m_pParticleEmitter->SetMaxEnergy(2.0f);
	m_pParticleEmitter->SetMinSizeGrow(3.5f);
	m_pParticleEmitter->SetMaxSizeGrow(5.5f);
	m_pParticleEmitter->SetMinEmitterRange(0.2f);
	m_pParticleEmitter->SetMaxEmitterRange(0.5f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.6f));
	obj->AddComponent(m_pParticleEmitter);
	AddChild(obj);

	auto colMat = new ColorMaterial();
	gameContext.pMaterialManager->AddMaterial(colMat, 0);

	//Teapot
	//******
	m_pTeapot = new GameObject();
	m_pModel = new ModelComponent(L"./Resources/Meshes/Teapot.ovm");
	m_pModel->SetMaterial(0);
	m_pTeapot->AddComponent(m_pModel);
	AddChild(m_pTeapot);
	m_pTeapot->GetTransform()->Scale(.3f, .3f, .3f);

	gameContext.pInput->AddInputAction(InputAction(0, InputTriggerState::Down, VK_RIGHT));
	gameContext.pInput->AddInputAction(InputAction(1, InputTriggerState::Down, VK_LEFT));
	gameContext.pInput->AddInputAction(InputAction(2, InputTriggerState::Down, VK_UP));
	gameContext.pInput->AddInputAction(InputAction(3, InputTriggerState::Down, VK_DOWN));
}

void ParticleScene::Update()
{
	const auto gameContext = GetGameContext();

	auto initPos = m_pParticleEmitter->GetTransform()->GetPosition();
	auto moveSpeed = 10.0f * gameContext.pGameTime->GetElapsed();
	initPos.x += moveSpeed * sin(gameContext.pGameTime->GetTotal());

	m_pTeapot->GetTransform()->Rotate(0.0f, DirectX::XM_PIDIV2*gameContext.pGameTime->GetTotal(), 0.0f, false);

	if (gameContext.pInput->IsActionTriggered(0))
	{
		initPos.x += moveSpeed;
	}
	if (gameContext.pInput->IsActionTriggered(1))
	{
		initPos.x -= moveSpeed;
	}
	if (gameContext.pInput->IsActionTriggered(2))
	{
		initPos.z += moveSpeed;
	}
	if (gameContext.pInput->IsActionTriggered(3))
	{
		initPos.z -= moveSpeed;
	}
	m_pParticleEmitter->GetTransform()->Translate(initPos);
}

void ParticleScene::Draw()
{}
