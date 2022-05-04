#pragma once
#include "GameScene.h"

class CharacterScene final : public GameScene
{
public:
	CharacterScene();
	virtual ~CharacterScene() = default;

	CharacterScene(const CharacterScene& other) = delete;
	CharacterScene(CharacterScene&& other) noexcept = delete;
	CharacterScene& operator=(const CharacterScene& other) = delete;
	CharacterScene& operator=(CharacterScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};
