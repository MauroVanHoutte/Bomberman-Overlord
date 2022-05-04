#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostPixelate : public PostProcessingMaterial
{
public:
	PostPixelate(float pixelSizeX, float pixelSizeY);
	virtual ~PostPixelate() = default;

	PostPixelate(const PostPixelate& other) = delete;
	PostPixelate(PostPixelate&& other) noexcept = delete;
	PostPixelate& operator=(const PostPixelate& other) = delete;
	PostPixelate& operator=(PostPixelate&& other) noexcept = delete;
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
	ID3DX11EffectScalarVariable* m_pPixelSizeX;
	ID3DX11EffectScalarVariable* m_pPixelSizeY;
	float m_PixelSizeX;
	float m_PixelSizeY;
};