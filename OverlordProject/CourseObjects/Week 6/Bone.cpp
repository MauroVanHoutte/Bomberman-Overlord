#include "stdafx.h"
#include "Bone.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

BoneObject::BoneObject(int boneId, int materialId, float length)
	: m_BoneId{boneId}
	, m_MaterialId{materialId}
	, m_Length{length}
{
}

void BoneObject::AddBone(BoneObject* bone)
{
	bone->GetTransform()->Translate(m_Length, 0, 0);
	AddChild(bone);
}

const DirectX::XMFLOAT4X4& BoneObject::GetBindPose() const
{
	return m_BindPose;
}

void BoneObject::CalculateBindPose()
{
	auto world = DirectX::XMLoadFloat4x4(&GetTransform()->GetWorld());
	auto determinant = DirectX::XMMatrixDeterminant(world);
	auto bindPose = DirectX::XMMatrixInverse( &determinant, world);
	DirectX::XMStoreFloat4x4(&m_BindPose, bindPose);
	auto children = GetChildren<BoneObject>();
	for (BoneObject* bone : children)
	{
		bone->CalculateBindPose();
	}
}

void BoneObject::Initialize(const GameContext& )
{

	auto firstBone = new GameObject();
	auto boneModel = new ModelComponent(L"./Resources/Meshes/Bone.ovm");
	boneModel->SetMaterial(m_MaterialId);
	firstBone->AddComponent(boneModel);
	firstBone->GetTransform()->Rotate(0, -90, 0);
	firstBone->GetTransform()->Scale(m_Length, m_Length, m_Length);
	AddChild(firstBone);
}
