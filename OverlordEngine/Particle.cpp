#include "stdafx.h"
#include "Particle.h"
#include "MathHelper.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	using namespace DirectX;
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	if (!m_IsActive)
		return;

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();
	
	if (m_CurrentEnergy < 0)
	{
		m_IsActive = false;
		return;
	}

	auto position = XMLoadFloat3(&m_VertexInfo.Position);
	auto velocity = XMLoadFloat3(&m_EmitterSettings.Velocity);

	position += velocity * gameContext.pGameTime->GetElapsed();

	XMStoreFloat3(&m_VertexInfo.Position, position);

	auto lifePercentage = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color = m_EmitterSettings.Color;

	if (m_EmitterSettings.Fade)
	{
		m_VertexInfo.Color.w = 2 * lifePercentage;

	}

	m_VertexInfo.Size = m_InitSize + (m_InitSize * m_SizeGrow * (1- lifePercentage));

}

void Particle::Init(XMFLOAT3 initPosition)
{
	using namespace DirectX;
	UNREFERENCED_PARAMETER(initPosition);
	//TODO: See Lab9_2
	m_IsActive = true;
	m_CurrentEnergy = m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);

	XMVECTOR randomDirection{ 1,0,0 };
	
	auto rotationMatrix = XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));
	randomDirection = XMVector3TransformNormal(randomDirection, rotationMatrix);
	auto distance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);

	auto pos = XMLoadFloat3(&initPosition);
	XMStoreFloat3(&m_VertexInfo.Position, pos +(distance * randomDirection));

	m_InitSize = m_VertexInfo.Size = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	if (m_EmitterSettings.Rotate)
		m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
	else
		m_VertexInfo.Rotation = 0;
}
