#pragma once
#include "GameObject.h"

enum class RockType
{
	BREAKABLE,
	UNBREAKABLE
};
class Rock : public GameObject
{
public:
	Rock(const DirectX::XMFLOAT3& position, RockType type);
	virtual ~Rock() = default;

	Rock(const Rock& other) = delete;
	Rock(Rock&& other) noexcept = delete;
	Rock& operator=(const Rock& other) = delete;
	Rock& operator=(Rock&& other) noexcept = delete;

	void Initialize(const GameContext & gameContext) override;
	void PostInitialize(const GameContext & gameContext) override;
	void Update(const GameContext & gameContext) override;
	void LateUpdate(const GameContext& gameContext) override;

	RockType GetType();
	void MarkForDeletion();

private:
	RockType m_Type;
	float m_BlockSize = 9.f;
	DirectX::XMFLOAT3 m_Position;
	bool m_ToBeDeleted;
};