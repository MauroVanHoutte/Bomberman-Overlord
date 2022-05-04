#pragma once
#include "GameObject.h"

class PowerPickup : public GameObject
{
public:
	PowerPickup(const DirectX::XMFLOAT3& position);
	virtual ~PowerPickup() = default;

	PowerPickup(const PowerPickup& other) = delete;
	PowerPickup(PowerPickup&& other) noexcept = delete;
	PowerPickup& operator=(const PowerPickup& other) = delete;
	PowerPickup& operator=(PowerPickup&& other) noexcept = delete;

	void Initialize(const GameContext & gameContext) override;
	void PostInitialize(const GameContext & gameContext) override;
	void Update(const GameContext & gameContext) override;
	void LateUpdate(const GameContext& gameContext) override;

	void MarkForDeletion();
	bool IsMarkedForDeletion();

private:
	DirectX::XMFLOAT3 m_Position;
	bool m_ToBeDeleted = false;
};