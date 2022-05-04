#pragma once
#include "GameObject.h"
#include "GeneralStructs.h"

class BoneObject : public GameObject
{
public:
	BoneObject(int boneId, int materialId, float length = 0.5f);
	~BoneObject() = default;

	BoneObject(const BoneObject& other) = delete;
	BoneObject(BoneObject&& other) = delete;
	BoneObject& operator=(const BoneObject& other) = delete;
	BoneObject& operator=(BoneObject&& other) = delete;

	void AddBone(BoneObject* bone);

	const DirectX::XMFLOAT4X4& GetBindPose() const;

	void CalculateBindPose();

protected:
	virtual void Initialize(const GameContext& gameContext);

private:
	float m_Length;
	int m_BoneId;
	int m_MaterialId;
	DirectX::XMFLOAT4X4 m_BindPose;
};