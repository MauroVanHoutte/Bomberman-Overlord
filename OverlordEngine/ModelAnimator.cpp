#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber, bool reset)
{
	UNREFERENCED_PARAMETER(clipNumber);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
	if (clipNumber >= GetClipCount())
	{
		Reset(false);
		Logger::LogError(L"[ModelAnimator] clipnumber non existent");
		return;
	}
	else
	{
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber], reset);
	}
}

void ModelAnimator::SetAnimation(std::wstring clipName, bool reset)
{
	UNREFERENCED_PARAMETER(clipName);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message
	for (auto clip : m_pMeshFilter->m_AnimationClips)
	{
		if (clip.Name == clipName)
		{
			SetAnimation(clip, reset);
			return;
		}
	}

	Reset();
	Logger::LogError(L"[ModelAnimator] clipname not existent");
}

void ModelAnimator::SetAnimation(AnimationClip clip, bool reset)
{
	UNREFERENCED_PARAMETER(clip);
	//TODO: complete
	//Set m_ClipSet to true
	//Set m_CurrentClip
	m_ClipSet = true;
	m_CurrentClip = clip;
	//Call Reset(false)
	if (reset)
	{
		Reset(false);
	}
}

void ModelAnimator::Reset(bool pause)
{
	UNREFERENCED_PARAMETER(pause);
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	m_IsPlaying = pause;

	//Set m_TickCount to zero
	//Set m_AnimationSpeed to 1.0f
	m_TickCount = 0;
	m_AnimationSpeed = 1.f;

	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
	if (m_ClipSet)
	{
		m_Transforms.assign(m_CurrentClip.Keys[0].BoneTransforms.begin(), m_CurrentClip.Keys[0].BoneTransforms.end());
	}
	else
	{
		auto identityMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4X4 matrix;
		DirectX::XMStoreFloat4x4(&matrix, identityMatrix);
		m_Transforms.assign(m_CurrentClip.Keys[0].BoneTransforms.size(), matrix);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	using namespace DirectX;
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		Clamp(passedTicks, m_CurrentClip.Duration, 0.f);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.Duration;
			}
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration)
			{
				m_TickCount -= m_CurrentClip.Duration;
			}
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA{m_CurrentClip.Keys[0]};
		AnimationKey keyB{m_CurrentClip.Keys[m_CurrentClip.Keys.size()-1]};
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (auto& key : m_CurrentClip.Keys)
		{
			if (key.Tick > keyA.Tick && key.Tick < m_TickCount)
			{
				keyA = key;
			}

			if (key.Tick < keyB.Tick && key.Tick > m_TickCount)
			{
				keyB = key;
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float blendWeightB = (m_TickCount - keyA.Tick) / (keyB.Tick - keyA.Tick);
		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
		m_Transforms.resize(keyA.BoneTransforms.size());
		for (size_t i = 0; i < keyA.BoneTransforms.size(); ++i)
		{
			auto transformA = keyA.BoneTransforms[i];
			auto transformB = keyB.BoneTransforms[i];
			XMVECTOR scaleA, translationA, rotationA;
			XMMatrixDecompose(&scaleA, &rotationA, &translationA, XMLoadFloat4x4(&transformA));
			XMVECTOR scaleB, translationB, rotationB;
			XMMatrixDecompose(&scaleB, &rotationB, &translationB, XMLoadFloat4x4(&transformB));
			auto lerpedScale = XMVectorLerp(scaleA, scaleB, blendWeightB);
			auto lerpedTranslation = XMVectorLerp(translationA, translationB, blendWeightB);
			auto lerpedRotation = XMQuaternionSlerp(rotationA, rotationB, blendWeightB);
			auto world = XMMatrixScaling(XMVectorGetX(lerpedScale), XMVectorGetY(lerpedScale), XMVectorGetZ(lerpedScale)) * XMMatrixRotationQuaternion(lerpedRotation) * XMMatrixTranslation(XMVectorGetX(lerpedTranslation), XMVectorGetY(lerpedTranslation), XMVectorGetZ(lerpedTranslation));
			XMStoreFloat4x4(&m_Transforms[i], world);
		}
	}
}
