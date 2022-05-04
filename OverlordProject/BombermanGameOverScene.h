#pragma once
#include "GameScene.h"

class SpriteComponent;

class BombermanGameOverScene : public GameScene
{
public:
	BombermanGameOverScene();
	virtual ~BombermanGameOverScene() = default;

	BombermanGameOverScene(const BombermanGameOverScene& other) = delete;
	BombermanGameOverScene(BombermanGameOverScene&& other) noexcept = delete;
	BombermanGameOverScene& operator=(const BombermanGameOverScene& other) = delete;
	BombermanGameOverScene& operator=(BombermanGameOverScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SceneActivated() override;

private:
	SpriteComponent* m_pMainMenu;
	SpriteComponent* m_pQuit;
};