#pragma once
#include "GameObject.h"

class BombPickup : public GameObject
{
public:
	BombPickup(const DirectX::XMFLOAT3& position);
	virtual ~BombPickup() = default;

	BombPickup(const BombPickup& other) = delete;
	BombPickup(BombPickup&& other) noexcept = delete;
	BombPickup& operator=(const BombPickup& other) = delete;
	BombPickup& operator=(BombPickup&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void LateUpdate(const GameContext& gameContext) override;

	void MarkForDeletion();
	bool IsMarkedForDeletion();

private:
	DirectX::XMFLOAT3 m_Position;
	bool m_ToBeDeleted = false;
};