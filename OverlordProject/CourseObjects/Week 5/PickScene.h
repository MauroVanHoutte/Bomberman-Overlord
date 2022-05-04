#pragma once
#include "GameScene.h"

class PickScene final : public GameScene
{
public:
	PickScene();
	virtual ~PickScene() = default;

	PickScene(const PickScene& other) = delete;
	PickScene(PickScene&& other) noexcept = delete;
	PickScene& operator=(const PickScene& other) = delete;
	PickScene& operator=(PickScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};


