#pragma once
#include "GameScene.h"

class SpriteComponent;

class BombermanPauseScene : public GameScene
{
public:
	BombermanPauseScene();
	virtual ~BombermanPauseScene() = default;

	BombermanPauseScene(const BombermanPauseScene& other) = delete;
	BombermanPauseScene(BombermanPauseScene&& other) noexcept = delete;
	BombermanPauseScene& operator=(const BombermanPauseScene& other) = delete;
	BombermanPauseScene& operator=(BombermanPauseScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	SpriteComponent* m_pResume;
	SpriteComponent* m_pMainMenu;
	SpriteComponent* m_pRestart;
	SpriteComponent* m_pQuit;
};