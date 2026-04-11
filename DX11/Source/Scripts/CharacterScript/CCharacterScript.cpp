#include "pch.h"
#include "CCharacterScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CCharacterScript::CCharacterScript(enum SCRIPT_TYPE _ScriptType)
    : CScript(static_cast<int>(_ScriptType))
{
}

CCharacterScript::~CCharacterScript()
{
}

void CCharacterScript::Begin()
{
    CScript::Begin();
    
    // Init BodySize
    m_BodySize = Transform()->GetRelativeScaleXY() * ColliderRect()->GetScale();
    m_BodySizeHalf = m_BodySize * 0.5f; 
}

void CCharacterScript::Tick()
{
    Move();
    UpdateCurrentFacedDirection();
}

void CCharacterScript::MovePushedOut()
{
    m_PushedOutTime += DT;

    // Velocity 구하고 Position 업데이트 시키고 이렇게 하면 됨
    // const float alpha = clamp(m_PushedOutTime / m_PushedOutTotalTime, 0.f, 1.f);
    const float alpha = m_PushedOutTime / m_PushedOutTotalTime;

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
    
    if (m_PushedOutTime < m_PushedOutTotalTime) return;
    
    // PushedOut End
    m_PushedOutTime = 0.f;
    
    // 각자의 MainState enum이 달라서 -> 각자의 다음 MainState 처리는 순수가상함수로 호출처리함
    AfterPushedOutFin();
    
    // 사망하였다면 Collider 꺼줌
    if (GetOwner()->GetScriptComponent<CStatScript>()->IsDead())
        GetCollider2D()->SetActive(false);
}

bool CCharacterScript::IsCurrentlyOutOfBound() const
{
    const Vec2 Pos = Transform()->GetRelativePosXY();
    
    return      Pos.x - m_BodySizeHalf.x  < -GM->GetBackgroundCellManager()->GetWorldSizeHalf() ||
                Pos.x + m_BodySizeHalf.x > GM->GetBackgroundCellManager()->GetWorldSizeHalf()   ||
                Pos.y - m_BodySizeHalf.y < -GM->GetBackgroundCellManager()->GetWorldSizeHalf()  ||
                Pos.y + m_BodySizeHalf.y > GM->GetBackgroundCellManager()->GetWorldSizeHalf();
}

void CCharacterScript::HandleBoundary()
{
    CBackgroundTile* BackgroundCellMgr = GM->GetBackgroundCellManager();
    
    // if (Transform()->GetRelativePosX() - m_BodySizeHalf.x < -BackgroundCellMgr->GetWorldSizeHalf())

    Vec2 Pos = Transform()->GetRelativePosXY();
    
    if (Pos.x - m_BodySizeHalf.x < -BackgroundCellMgr->GetWorldSizeHalf())
        Pos.x = -BackgroundCellMgr->GetWorldSizeHalf() + m_BodySizeHalf.x;
    else if (Pos.x + m_BodySizeHalf.x > BackgroundCellMgr->GetWorldSizeHalf())
        Pos.x = BackgroundCellMgr->GetWorldSizeHalf() - m_BodySizeHalf.x;
    
    if (Pos.y - m_BodySizeHalf.y < -BackgroundCellMgr->GetWorldSizeHalf())
        Pos.y = -BackgroundCellMgr->GetWorldSizeHalf() + m_BodySizeHalf.y;
    else if (Pos.y + m_BodySizeHalf.y > BackgroundCellMgr->GetWorldSizeHalf())
        Pos.y = BackgroundCellMgr->GetWorldSizeHalf() - m_BodySizeHalf.y;
    
    Transform()->SetRelativePosXY(Pos);
}

void CCharacterScript::RewindPushedOut(const Vec2& _PushedOutDirection)
{
    m_PushedOutTime = 0.f;
    m_PushedOutFaceDirection = _PushedOutDirection.Normalized();
    
}
