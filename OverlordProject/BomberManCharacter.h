#pragma once
#include "GameObject.h"


class ControllerComponent;
class ModelComponent;
class ModelAnimator;
class SpriteComponent;
class SpriteFont;
class PostPixelate;

class BomberManCharacter : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		PLACE_BOMB
	};

	BomberManCharacter(int playerID, float radius = 2, float height = 5);
	virtual ~BomberManCharacter();

	BomberManCharacter(const BomberManCharacter& other) = delete;
	BomberManCharacter(BomberManCharacter&& other) noexcept = delete;
	BomberManCharacter& operator=(const BomberManCharacter& other) = delete;
	BomberManCharacter& operator=(BomberManCharacter&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void LateUpdate(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void BombDeleted();
	void BombHit();
	void PowerPickup();
	void BombPickup();

protected:
	void PlaceBomb();
	void SetupInput(const GameContext& gameContext);

	int m_Health = 3;
	float m_FlickerCounter = 0.f;
	float m_FlickerInterval = 0.1f;
	float m_InvincibilityRemainingDuration = 2.f;
	float m_InvincibilityTotalDuration = 2.f;
	int m_PlacableBombs = 1;
	int m_MaxPlacableBombs = 1;
	int m_BombPower = 1;
	bool m_Invincible = false;
	bool m_ModelAttached = true;
	bool m_ToBeDeleted = false;

	ControllerComponent* m_pController;
	GameObject* m_pModelObject;
	ModelComponent* m_pModel;
	ModelAnimator* m_pAnimator;
	SpriteFont* m_pSpriteFont;
	DirectX::XMFLOAT3 m_SpriteStartPos;
	float m_Offset = 0;

	float m_Radius, m_Height;

	//Running
	float m_MaxRunVelocity;

	int m_PlayerId;

	FMOD::Sound* m_pHurtSound;
	FMOD::Sound* m_pPickupSound;
	FMOD::Sound* m_pPlaceSound;
	FMOD::Sound* m_pExplosionSound;
};