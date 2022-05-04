#pragma once
#include "GameScene.h"

class BombermanScene : public GameScene
{
public:
	BombermanScene();
	virtual ~BombermanScene();

	BombermanScene(const BombermanScene& other) = delete;
	BombermanScene(BombermanScene&& other) noexcept = delete;
	BombermanScene& operator=(const BombermanScene& other) = delete;
	BombermanScene& operator=(BombermanScene&& other) noexcept = delete;

	void CharacterDied();
	void SetupGame(int player);
	void Restart();
	void CleanupLevel();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;
private:
	void SetupLevel(int nrPlayers);
	GameObject* m_pCamera = nullptr;
	int m_NrPlayers;
	int m_PlayersLeft;
};