#pragma once
#include "GameScene.h"

class ModelComponent;
class SpriteFont;

class PostProcessingScene final : public GameScene
{
public:
	PostProcessingScene();
	virtual ~PostProcessingScene() = default;

	PostProcessingScene(const PostProcessingScene& other) = delete;
	PostProcessingScene(PostProcessingScene&& other) noexcept = delete;
	PostProcessingScene& operator=(const PostProcessingScene& other) = delete;
	PostProcessingScene& operator=(PostProcessingScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	ModelComponent* m_pModel;
	SpriteFont* m_pFont;
};

