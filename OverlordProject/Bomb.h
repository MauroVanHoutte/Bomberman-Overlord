#pragma once
#include "GameObject.h"

class BomberManCharacter;
class ParticleEmitterComponent;
class ModelComponent;
class ColliderComponent;

class Bomb : public GameObject
{
public:
	Bomb(BomberManCharacter* owner, DirectX::XMFLOAT3 pos, int power, FMOD::Sound* explosion);
	~Bomb() = default;

	Bomb(const Bomb& other) = delete;
	Bomb(Bomb&& other) noexcept = delete;
	Bomb& operator=(const Bomb& other) = delete;
	Bomb& operator=(Bomb&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void SetIsTrigger(bool);
private:
	void RaycastBombsCharacters();
	void RaycastRocks();
	void SetExplosionEmitters();
	void AddExplosionEmitter(float x, float y, float z);
	void SetOff();
	std::vector<int> m_Strengths;

	BomberManCharacter* m_pOwner;
	ParticleEmitterComponent* m_pFuseParticleEmitter;
	ModelComponent* m_pModel;
	ColliderComponent* m_pCollider;
	DirectX::XMFLOAT3 m_Position;
	float m_FuseTimer = 0;
	float m_FuseDuration = 2.f;
	float m_ExplosionDuration = 0.5f;
	int m_Power;
	float m_BlockSize = 9.f;

	bool m_IsTrigger = true;

	FMOD::Sound* m_pExplosionSound;
};