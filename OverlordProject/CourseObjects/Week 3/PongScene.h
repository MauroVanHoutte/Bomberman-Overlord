#pragma once
#include "GameScene.h"

class PongScene final : public GameScene
{
public:
	PongScene();
	virtual ~PongScene() = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	GameObject* m_pBall;
	GameObject* m_LeftPaddle;
	GameObject* m_RightPaddle;
};
