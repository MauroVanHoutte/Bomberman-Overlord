#pragma once
#include "GameScene.h"
#include "EffectHelper.h"

class Material;

class UberScene : public GameScene
{
public:
	UberScene();
	virtual ~UberScene() = default;

	UberScene(const UberScene& other) = delete;
	UberScene(UberScene&& other) noexcept = delete;
	UberScene& operator=(const UberScene& other) = delete;
	UberScene& operator=(UberScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

};