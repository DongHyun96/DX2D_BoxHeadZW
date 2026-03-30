#include "pch.h"
#include "CPlayerAnimHandler.h"

#include "CPlayerScript.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"

CPlayerAnimHandler::CPlayerAnimHandler()
    : CCharacterAnimHandler(SCRIPT_TYPE::PLAYERANIMHANDLER)
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
    UpdateWalkingBackward();
    CCharacterAnimHandler::Tick();
}

void CPlayerAnimHandler::UpdateWalkingBackward()
{
    const Vec3 VelocityDirection = m_MainPlayerScript->GetVelocity().Normalized();
    const Vec2 VelocityDirection2D = ToVec2(VelocityDirection);
    
    const float DotProduct = VelocityDirection2D.Dot(m_MainPlayerScript->GetPlayerToMousePos().Normalized());
    const float AngleBetween = acosf(DotProduct); // 바라보는 방향과 진행방향과의 사잇각

    m_WalkingBackward = AngleBetween > XM_PIDIV2; 
}

void CPlayerAnimHandler::UpdateAnimTransition()
{
    const PLAYER_MAINSTATE CurrentMainState = m_MainPlayerScript->GetMainState();
    const PLAYER_HANDSTATE CurrentHandState = m_MainPlayerScript->GetHandState();

    switch (CurrentMainState)
    {
    case PLAYER_MAINSTATE::IDLE: // HandState에 따른 Walk 처리
    {
        const wstring& WeaponCategory       = mapPlayerHandStateAnimCategory.at(CurrentHandState);
        const Vec3 CurrentVelocity          = m_MainPlayerScript->GetVelocity();
        const int FlipBookIndexByDirection  = static_cast<int>(m_MainPlayerScript->GetCurrentFacedDirection());

        if (CurrentVelocity.LengthSquared() == 0.f) // 이동하고 있지 않은 상태
        {
            // 해당 방향으로 자연스럽게 멈춤
            // 2번 index가 멈춘 상태의 Sprite 모양
            FlipbookRender()->Stop(WeaponCategory, FlipBookIndexByDirection, 2);
            m_PrevAnimDirection = EDIRECTION::END;
            return;
        }

        // 이전 상태와 동일한 Animation이 재생중인 상태 -> 한 번 더 재생 처리 방지
        if (m_MainPlayerScript->GetCurrentFacedDirection() == m_PrevAnimDirection &&
            m_WalkingBackward == m_PrevWalkingBackward &&
            CurrentHandState == m_PrevHandState
            ) return;

        FlipbookRender()->Play(WeaponCategory, FlipBookIndexByDirection, 12, -1, m_WalkingBackward);
    }
        break;
    case PLAYER_MAINSTATE::PUSHED_OUT: // 얘는 계속해서 Stop처리로 해도 되긴 하겠다
    {
        // PushedOut 방향에 따른 Flipbook Idx를 골라주어야 함
        
        // FlipbookRender()->Play(L"PushedOut", )
        
        m_PushedOutTime += DT;
        if (m_PushedOutTime > m_PushedOutTotalTime) // MainState 전환 처리
        {
            
        }
    }
        break;
    case PLAYER_MAINSTATE::DIE:
        break;
    case PLAYER_MAINSTATE::END:
        break;
    }

    const wstring& MainAnimCategory = GetPlayerMainAnimCategory(CurrentMainState); // PushedOut, Die
   

    m_PrevAnimDirection     = m_MainPlayerScript->GetCurrentFacedDirection();
    m_PrevWalkingBackward   = m_WalkingBackward;
    m_PrevHandState         = CurrentHandState;
}
