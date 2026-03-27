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
    const Vec2 MousePos             = ToVec2(KeyMgr::GetInst()->GetMouseWorldPos());
    const Vec2 PlayerPos2D          = ToVec2(Transform()->GetRelativePos());
    const Vec2 PlayerToMousePos     = MousePos - PlayerPos2D;
    
    UpdateAnimDirection(PlayerToMousePos);
    UpdateWalkingBackward(PlayerToMousePos);
    UpdateAnimTransition();    
}

void CPlayerAnimHandler::UpdateAnimDirection(const Vec2& _PlayerToMousePos)
{
    m_AnimDirection = GetDirection(_PlayerToMousePos);
    // 이 경우, 마우스포인터 좌표와 Player의 위치가 완전히 일치하는 상황 (거의 아예 안나올거다)
    // 따로 DOWN 방향으로 처리
    if (m_AnimDirection == EDIRECTION::END) m_AnimDirection = EDIRECTION::DOWN;
}

void CPlayerAnimHandler::UpdateWalkingBackward(const Vec2& _PlayerToMousePos)
{
    const Vec3 VelocityDirection = m_MainPlayerScript->GetVelocity().Normalized();
    const Vec2 VelocityDirection2D = ToVec2(VelocityDirection);
    
    const float DotProduct = VelocityDirection2D.Dot(_PlayerToMousePos.Normalized());
    const float AngleBetween = acosf(DotProduct); // 바라보는 방향과 진행방향과의 사잇각

    m_WalkingBackward = AngleBetween > XM_PIDIV2; 
}

void CPlayerAnimHandler::UpdateAnimTransition()
{
    PLAYER_HANDSTATE CurrentHandState = m_MainPlayerScript->GetHandState();
    const wstring& AnimCategory = mapPlayerHandStateAnimCategory.at(CurrentHandState); 
    
    Vec3 CurrentVelocity = m_MainPlayerScript->GetVelocity();
    const int FlipBookIndexByDirection = static_cast<int>(m_AnimDirection);

    if (CurrentVelocity.LengthSquared() == 0.f) // 이동하고 있지 않은 상태
    {
        // 해당 방향으로 자연스럽게 멈춤
        // 2번 index가 멈춘 상태의 Sprite 모양
        FlipbookRender()->Stop(AnimCategory, FlipBookIndexByDirection, 2);
        m_PrevAnimDirection = EDIRECTION::END;
        return;
    }

    // 이전 상태와 동일한 Animation이 재생중인 상태 -> 한 번 더 재생 처리 방지
    if (m_AnimDirection == m_PrevAnimDirection &&
        m_WalkingBackward == m_PrevWalkingBackward &&
        CurrentHandState == m_PrevHandState
        ) return;

    FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 12, -1, m_WalkingBackward);

    m_PrevAnimDirection     = m_AnimDirection;
    m_PrevWalkingBackward   = m_WalkingBackward;
    m_PrevHandState         = CurrentHandState;
}
