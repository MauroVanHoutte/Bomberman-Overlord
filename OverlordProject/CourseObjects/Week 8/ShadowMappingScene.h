#pragma once
#include "GameScene.h"

class ModelComponent;

class ShadowMappingScene final : public GameScene
{
public:
	ShadowMappingScene();
	virtual ~ShadowMappingScene() = default;

	ShadowMappingScene(const ShadowMappingScene& other) = delete;
	ShadowMappingScene(ShadowMappingScene&& other) noexcept = delete;
	ShadowMappingScene& operator=(const ShadowMappingScene& other) = delete;
	ShadowMappingScene& operator=(ShadowMappingScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	ModelComponent* m_pModel = nullptr;
};

