#pragma once
#include "GameScene.h"
#include "EffectHelper.h"

class Material;

class SpikeyScene final: public GameScene
{
public:
	SpikeyScene();
	virtual ~SpikeyScene() = default;

	SpikeyScene(const SpikeyScene& other) = delete;
	SpikeyScene(SpikeyScene&& other) noexcept = delete;
	SpikeyScene& operator=(const SpikeyScene& other) = delete;
	SpikeyScene& operator=(SpikeyScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	float m_FpsInterval;
};

