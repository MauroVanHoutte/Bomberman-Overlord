#include "stdafx.h"
#include "BombermanScene.h"
#include "GameObject.h"
#include "ModelComponent.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "BomberManCharacter.h"
#include "DebugRenderer.h"
#include "PhysxProxy.h"
#include "Rock.h"
#include "FixedCamera.h"
#include "FreeCamera.h"
#include "PostPixelate.h"
#include "SceneManager.h"
#include "Materials/UberMaterial.h"

BombermanScene::BombermanScene() : GameScene(L"Bomberman")
	, m_NrPlayers{}
	, m_PlayersLeft{}
{
}

BombermanScene::~BombermanScene()
{

}

void BombermanScene::CharacterDied()
{
	--m_PlayersLeft;
}

void BombermanScene::SetupGame(int player)
{
	m_NrPlayers = player;
	m_PlayersLeft = player;

	m_pCamera = new FixedCamera();
	m_pCamera->GetTransform()->Translate(10, 115, -100);
	m_pCamera->GetTransform()->Rotate(1.f, 0, 0, false);
	AddChild(m_pCamera);
	SetActiveCamera(m_pCamera->GetComponent<CameraComponent>());

	auto pGroundObj = new GameObject();
	auto pGroundModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm");
	pGroundModel->SetMaterial(7);

	pGroundObj->AddComponent(pGroundModel);
	pGroundObj->GetTransform()->Scale(30.0f, 30.f, 30.0f);
	AddChild(pGroundObj);
	pGroundObj->GetTransform()->Translate(0, -3, 0);

	SetupLevel(m_NrPlayers);
}

void BombermanScene::Restart()
{
	CleanupLevel();
	SetupGame(m_NrPlayers);
}

void BombermanScene::CleanupLevel()
{
	auto& children = GetChildren();

	for (auto& pChild : children)
	{
		SafeDelete(pChild);
	}
	children.clear();
}

void BombermanScene::Initialize()
{
	DebugRenderer::ToggleDebugRenderer();
	AddPostProcessingEffect(new PostPixelate(2, 2));

	GetGameContext().pShadowMapper->SetLight({ -85.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });
	auto shadowMaterial = new DiffuseMaterial_Shadow();
	shadowMaterial->SetDiffuseTexture(L"./Resources/Textures/Rock0.png");
	shadowMaterial->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(shadowMaterial, 7);
	shadowMaterial = new DiffuseMaterial_Shadow();
	shadowMaterial->SetDiffuseTexture(L"./Resources/Textures/Rock1.png");
	shadowMaterial->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(shadowMaterial, 8);

	UberMaterial* material = new UberMaterial();
	material->EnableDiffuseTexture(true);
	material->EnableEnvironmentMapping(false);
	material->EnableFresnelFaloff(false);
	material->EnableNormalMapping(false); //normal map has annoying artifact on heads
	material->EnableOpacityMap(false);
	material->EnableSpecularBlinn(true);
	material->EnableSpecularPhong(false);
	material->EnableSpecularLevelTexture(true);
	std::wstring diffuse{ L"./Resources/Textures/Timmy/Timmy_Diffuse_" };
	diffuse += std::to_wstring(0) + L".png";
	material->SetDiffuseTexture(diffuse);
	material->SetNormalMapTexture(L"./Resources/Textures/Timmy/Timmy_Normal.png");
	material->SetSpecularColor({ 0.22f, 0.22f, 0.22f, 1.f });
	material->SetSpecularLevelTexture(L"./Resources/Textures/Timmy/Timmy_Glossiness.png");
	material->EnableSkinning(true);

	GetGameContext().pMaterialManager->AddMaterial(material, 0);

	material = new UberMaterial();
	material->EnableDiffuseTexture(true);
	material->EnableEnvironmentMapping(false);
	material->EnableFresnelFaloff(false);
	material->EnableNormalMapping(false); //normal map has annoying artifact on heads
	material->EnableOpacityMap(false);
	material->EnableSpecularBlinn(true);
	material->EnableSpecularPhong(false);
	material->EnableSpecularLevelTexture(true);
	diffuse = L"./Resources/Textures/Timmy/Timmy_Diffuse_";
	diffuse += std::to_wstring(1) + L".png";
	material->SetDiffuseTexture(diffuse);
	material->SetNormalMapTexture(L"./Resources/Textures/Timmy/Timmy_Normal.png");
	material->SetSpecularColor({ 0.22f, 0.22f, 0.22f, 1.f });
	material->SetSpecularLevelTexture(L"./Resources/Textures/Timmy/Timmy_Glossiness.png");
	material->EnableSkinning(true);

	GetGameContext().pMaterialManager->AddMaterial(material, 1);

	material = new UberMaterial();
	material->EnableDiffuseTexture(true);
	material->EnableEnvironmentMapping(false);
	material->EnableFresnelFaloff(false);
	material->EnableNormalMapping(false); //normal map has annoying artifact on heads
	material->EnableOpacityMap(false);
	material->EnableSpecularBlinn(true);
	material->EnableSpecularPhong(false);
	material->EnableSpecularLevelTexture(true);
	diffuse = L"./Resources/Textures/Timmy/Timmy_Diffuse_";
	diffuse += std::to_wstring(2) + L".png";
	material->SetDiffuseTexture(diffuse);
	material->SetNormalMapTexture(L"./Resources/Textures/Timmy/Timmy_Normal.png");
	material->SetSpecularColor({ 0.22f, 0.22f, 0.22f, 1.f });
	material->SetSpecularLevelTexture(L"./Resources/Textures/Timmy/Timmy_Glossiness.png");
	material->EnableSkinning(true);

	GetGameContext().pMaterialManager->AddMaterial(material, 2);

	material = new UberMaterial();
	material->EnableDiffuseTexture(true);
	material->EnableEnvironmentMapping(false);
	material->EnableFresnelFaloff(false);
	material->EnableNormalMapping(false); //normal map has annoying artifact on heads
	material->EnableOpacityMap(false);
	material->EnableSpecularBlinn(true);
	material->EnableSpecularPhong(false);
	material->EnableSpecularLevelTexture(true);
	diffuse = L"./Resources/Textures/Timmy/Timmy_Diffuse_";
	diffuse += std::to_wstring(3) + L".png";
	material->SetDiffuseTexture(diffuse);
	material->SetNormalMapTexture(L"./Resources/Textures/Timmy/Timmy_Normal.png");
	material->SetSpecularColor({ 0.22f, 0.22f, 0.22f, 1.f });
	material->SetSpecularLevelTexture(L"./Resources/Textures/Timmy/Timmy_Glossiness.png");
	material->EnableSkinning(true);

	GetGameContext().pMaterialManager->AddMaterial(material, 3);
}

void BombermanScene::Update()
{
	if (m_PlayersLeft < 2)
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"GameOverScene");
	}
	if (GetGameContext().pInput->IsKeyboardKeyDown(VK_ESCAPE))
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"PauseScene");
	}
}

void BombermanScene::Draw()
{
}

void BombermanScene::SceneActivated()
{
}

void BombermanScene::SceneDeactivated()
{
}

void BombermanScene::SetupLevel(int nrPlayers)
{
	if (nrPlayers > 4)
	{
		Logger::LogError(L"Cant play with more then 4 players");
	}

	using namespace DirectX;
	/*
	15*13 border
	*/
	float blocksize = 9.f;
	int centerCol = 7;
	int centerRow = 6;
	--centerCol;
	--centerRow;

	XMFLOAT3 topLeftPos{ -blocksize * centerCol, 0, blocksize * centerRow };



	for (size_t col = 0; col < 15; col++)
	{
		for (size_t row = 0; row < 13; row++)
		{
			if ((col == 1 && row == 1) || (col == 2 && row == 1) || (col == 1 && row == 2))
				continue;
			if ((col == 1 && row == 11) || (col == 2 && row == 11) || (col == 1 && row == 10))
				continue;
			if ((col == 13 && row == 1) || (col == 12 && row == 1) || (col == 13 && row == 2))
				continue;
			if ((col == 13 && row == 11) || (col == 12 && row == 11) || (col == 13 && row == 10))
				continue;

			if (col == 0 || col == 14 || row == 0 || row == 12 || (row % 2 == 0 && col % 2 == 0))
				AddChild(new Rock({topLeftPos.x + (blocksize * col), 0, topLeftPos.z - (blocksize *row)}, RockType::UNBREAKABLE));
			else
				AddChild(new Rock({ topLeftPos.x + (blocksize * col), 0, topLeftPos.z - (blocksize * row) }, RockType::BREAKABLE));
		}
	}

	GameObject* character;

	switch (nrPlayers)
	{
	case 4:
		character = new BomberManCharacter(3);
		AddChild(character);
		character->GetTransform()->Translate(topLeftPos.x + blocksize * 13, 0, topLeftPos.z - blocksize * 11);
	case 3:
		character = new BomberManCharacter(2);
		AddChild(character);
		character->GetTransform()->Translate(topLeftPos.x + blocksize, 0, topLeftPos.z - blocksize * 11);
	case 2:
		character = new BomberManCharacter(1);
		AddChild(character);
		character->GetTransform()->Translate(topLeftPos.x + blocksize * 13, 0, topLeftPos.z - blocksize); 
	case 1:
		character = new BomberManCharacter(0);
		AddChild(character);
		character->GetTransform()->Translate(topLeftPos.x + blocksize, 0, topLeftPos.z - blocksize);
	}
}

