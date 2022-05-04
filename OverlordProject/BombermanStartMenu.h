#pragma once
#include "GameScene.h"

class SpriteComponent;

class BombermanStartMenu : public GameScene
{
public:
	BombermanStartMenu();
	virtual ~BombermanStartMenu() = default;

	BombermanStartMenu(const BombermanStartMenu& other) = delete;
	BombermanStartMenu(BombermanStartMenu&& other) noexcept = delete;
	BombermanStartMenu& operator=(const BombermanStartMenu& other) = delete;
	BombermanStartMenu& operator=(BombermanStartMenu&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	GameObject* m_pCamera;
	SpriteComponent* m_p2Players;
	SpriteComponent* m_p3Players;
	SpriteComponent* m_p4Players;
	SpriteComponent* m_pQuit;
};