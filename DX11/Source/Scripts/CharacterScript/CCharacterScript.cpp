#include "pch.h"
#include "CCharacterScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CCharacterScript::CCharacterScript(enum SCRIPT_TYPE _ScriptType)
    : CScript(static_cast<int>(_ScriptType))
{
}

CCharacterScript::~CCharacterScript()
{
}

void CCharacterScript::Tick()
{
    Move();
    UpdateCurrentFacedDirection();
}

bool CCharacterScript::MovePushedOut()
{
    m_PushedOutTime += DT;

    // Velocity 구하고 Position 업데이트 시키고 이렇게 하면 됨
    const float alpha = std::clamp(m_PushedOutTime / m_PushedOutTotalTime, 0.f, 1.f);

    constexpr float kBurstRatio = 0.12f;  // 초반 12% 구간만 강하게
    constexpr float kMaxSpeed   = 750.f;
    constexpr float kDecelPow   = 5.f;    // 클수록 더 빨리 죽음

    float pushedOutSpeed{};
    if (alpha < kBurstRatio) pushedOutSpeed = kMaxSpeed;
        
    else
    {
        const float t = (alpha - kBurstRatio) / (1.f - kBurstRatio); // 0~1
        pushedOutSpeed = kMaxSpeed * powf(1.f - t, kDecelPow);       // 끝에서 0
    }
    
    m_Velocity = -ToVec3(m_PushedOutFaceDirection) * pushedOutSpeed;
    
    Vec3 Pos = Transform()->GetRelativePos() + m_Velocity * DT;
    Transform()->SetRelativePos(Pos);
    
    if (m_PushedOutTime < m_PushedOutTotalTime) return false;
    
    // PushedOut End
    m_PushedOutTime = 0.f;
    return true;
}

void CCharacterScript::RewindPushedOut(const Vec2& _PushedOutDirection)
{
    m_PushedOutTime = 0.f;
    m_PushedOutFaceDirection = _PushedOutDirection.Normalized();
    
}
