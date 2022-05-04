#pragma once
#include "GameScene.h"
#include "MeshDrawComponent.h"
#include "SkinnedVertex.h"

class BoneObject;

class SoftwareSkinningScene : public GameScene
{
public:
	SoftwareSkinningScene();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	BoneObject *m_pBone0, *m_pBone1;
	float m_BoneRotation;
	int m_RotationSign;

	void CreateMesh(float length);
	MeshDrawComponent* m_pMeshDrawer;
	std::vector<SkinnedVertex> m_SkinnedVertices;
};