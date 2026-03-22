#include "pch.h"
#include "CPlayerAnimHandler.h"

#include "CPlayerScript.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"

CPlayerAnimHandler::CPlayerAnimHandler()
    : CScript(static_cast<int>(SCRIPT_TYPE::PLAYERANIMHANDLER))
{
}

CPlayerAnimHandler::~CPlayerAnimHandler()
{
}

void CPlayerAnimHandler::Init()
{
}

void CPlayerAnimHandler::Begin()
{
    m_MainPlayerScript = GetOwner()->GetScriptComponent<CPlayerScript>().Get();
}

void CPlayerAnimHandler::Tick()
{
    UpdateAnimDirection();
    UpdateAction();    
}

void CPlayerAnimHandler::UpdateAnimDirection()
{
    const Vec2 MousePos     = ToVec2(KeyMgr::GetInst()->GetMouseWorldPos());
    const Vec2 PlayerPos2D  = ToVec2(Transform()->GetRelativePos());
    Vec2 PlayerToMousePos   = MousePos - PlayerPos2D;
    
    m_AnimDirection = GetDirection(PlayerToMousePos);
    // 이 경우, 마우스포인터 좌표와 Player의 위치가 완전히 일치하는 상황 (거의 아예 안나올거다)
    // 따로 DOWN 방향으로 처리
    if (m_AnimDirection == EDIRECTION::END) m_AnimDirection = EDIRECTION::DOWN;
}

void CPlayerAnimHandler::UpdateAction()
{
    Vec3 CurrentVelocity = m_MainPlayerScript->GetVelocity();
    const int FlipBookIndexByDirection = static_cast<int>(m_AnimDirection);

    if (CurrentVelocity.LengthSquared() == 0.f)
    {
        // 해당 방향으로 자연스럽게 멈춤
        // 2번 index가 멈춘 상태의 Sprite 모양
        FlipbookRender()->Stop(FlipBookIndexByDirection, 2);
        m_PrevAnimDirection = EDIRECTION::END;
        return;
    }
    
    if (m_AnimDirection == m_PrevAnimDirection) return; // 한 번 더 재생 처리 방지


    // 방향을 확인해서 뒤로이동하는 중인지 알아야 함
    FlipbookRender()->Play(FlipBookIndexByDirection, 12);

    m_PrevAnimDirection = m_AnimDirection;    
}
