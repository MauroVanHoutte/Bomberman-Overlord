#pragma once
#include "GameScene.h"

class ParticleEmitterComponent;
class ModelComponent;

class ParticleScene final : public GameScene
{
public:
	ParticleScene();
	virtual ~ParticleScene() = default;

	ParticleScene(const ParticleScene& other) = delete;
	ParticleScene(ParticleScene&& other) noexcept = delete;
	ParticleScene& operator=(const ParticleScene& other) = delete;
	ParticleScene& operator=(ParticleScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	ParticleEmitterComponent* m_pParticleEmitter;
	ModelComponent* m_pModel;
	GameObject* m_pTeapot = nullptr;
};

