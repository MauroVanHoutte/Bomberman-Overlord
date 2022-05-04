#include "stdafx.h"
#include "SoftwareSkinningScene.h"
#include "../../Materials/ColorMaterial.h"
#include "MaterialManager.h"
#include "GameObject.h"
#include "Bone.h"
#include "TransformComponent.h"

SoftwareSkinningScene::SoftwareSkinningScene()
	: GameScene(L"SoftwareSkinningScene")
	,m_pBone0{nullptr}
	, m_pBone1{nullptr}
	, m_BoneRotation{0}
	, m_RotationSign{1}
{
}

void SoftwareSkinningScene::Initialize()
{
	auto meshDrawer = new GameObject();
	m_pMeshDrawer = new MeshDrawComponent(24);
	meshDrawer->AddComponent(m_pMeshDrawer);
	AddChild(meshDrawer);

	CreateMesh(15.f);

	auto gameContext = GetGameContext();
	auto material = new ColorMaterial();
	gameContext.pMaterialManager->AddMaterial(material, 0);
	m_pBone0 = new BoneObject(0, 0, 15.f);
	m_pBone1 = new BoneObject(1, 0, 15.f);
	auto root = new GameObject();
	root->AddChild(m_pBone0);
	m_pBone0->AddBone(m_pBone1);

	AddChild(root);
	m_pBone0->CalculateBindPose();
}

void SoftwareSkinningScene::Update()
{
	using namespace DirectX;
	auto gameTime = GetGameContext().pGameTime;

	m_BoneRotation += m_RotationSign * 45 * gameTime->GetElapsed();
	
	if (m_RotationSign < 0 && m_BoneRotation < -45)
	{
		m_RotationSign = 1;
	}
	if (m_RotationSign > 0 && m_BoneRotation > 45)
	{
		m_RotationSign = -1;
	}

	m_pBone0->GetTransform()->Rotate( 0, m_BoneRotation, 0);
	m_pBone1->GetTransform()->Rotate( 0, -m_BoneRotation * 2, 0);

	auto bone0BindPose = DirectX::XMLoadFloat4x4(&m_pBone0->GetBindPose());
	auto bone1BindPose = DirectX::XMLoadFloat4x4(&m_pBone1->GetBindPose());

	auto boneTransform0 = bone0BindPose * DirectX::XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld());
	auto boneTransform1 = bone1BindPose * DirectX::XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld());

	for (size_t i = 0; i < 48; i++)
	{
		auto bone1 = XMVector3Transform(XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position), boneTransform0);
		auto bone2 = XMVector3Transform(XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position), boneTransform1);
		XMStoreFloat3(&m_SkinnedVertices[i].transformedVertex.Position, bone1 * m_SkinnedVertices[i].m_Blendweight1 + bone2 * m_SkinnedVertices[i].m_Blendweight2);
	}
}

void SoftwareSkinningScene::Draw()
{
	m_pMeshDrawer->RemoveTriangles();
	for (size_t i = 0; i < 48; i += 4)
	{
		m_pMeshDrawer->AddQuad(QuadPosNormCol(m_SkinnedVertices[i].transformedVertex, m_SkinnedVertices[i + 1].transformedVertex, m_SkinnedVertices[i + 2].transformedVertex, m_SkinnedVertices[i + 3].transformedVertex));
	}
	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene::CreateMesh(float length)
{
	auto pos = DirectX::XMFLOAT3(length / 2, 0, 0); 
	auto offset = DirectX::XMFLOAT3(length / 2, 2.5f, 2.5f); 
	auto col = DirectX::XMFLOAT4(1, 0, 0, 0.2f);
	//*****
	//BOX1*
	//*****
	//FRONT
	auto norm = DirectX::XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 1, 0));
	//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 1, 0));
	//BOTTOM
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 1, 0));
	//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 1, 0));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 1, 0));
	//RIGHT
	norm = DirectX::XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//*****//BOX2*//*****
	col = DirectX::XMFLOAT4(0, 1, 0, 0.2f);
	pos = DirectX::XMFLOAT3(22.5f, 0, 0);
	//FRONT
	norm = DirectX::XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0, 1));
	//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//BOTTOM
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//RIGHT
	norm = DirectX::XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0, 1));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0, 1));
}
