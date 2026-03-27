#include "pch.h"
#include "CEnemyAnimHandler.h"

#include "CEnemyScript.h"
#include "Source/ScriptMgr.h"

CEnemyAnimHandler::CEnemyAnimHandler()
    : CScript(static_cast<int>(SCRIPT_TYPE::ENEMYANIMHANDLER))
{
}

CEnemyAnimHandler::~CEnemyAnimHandler()
{
}


void CEnemyAnimHandler::Init()
{
    AddScriptParam(SCRIPT_PARAM::INT, &m_AnimFPSTemp, L"AnimFPS", true);
}

void CEnemyAnimHandler::Begin()
{
    m_MainEnemyScript = GetOwner()->GetScriptComponent<CEnemyScript>().Get();
}

void CEnemyAnimHandler::Tick()
{
    UpdateAnimDirection();
    UpdateAnimTransition();
}

void CEnemyAnimHandler::UpdateAnimDirection()
{
    // Update Current AnimDirection
    EDIRECTION CurrentDirection = GetEightDirection(m_MainEnemyScript->GetVelocity());
    
    // 현재 Velocity 크기가 0이면, 이전 AnimDirection 유지 
    m_AnimDirection = (CurrentDirection == EDIRECTION::END) ? m_PrevAnimDirection : CurrentDirection; 
}

void CEnemyAnimHandler::UpdateAnimTransition()
{
    ENEMY_STATE CurrentMainState = m_MainEnemyScript->GetMainState();
    // ENEMY_STATE CurrentMainState = ENEMY_STATE::ATTACK;
    const wstring& AnimCategory = mapEnemyMainStateAnimCategory.at(CurrentMainState);
    
    const Vec3 CurrentVelocity = m_MainEnemyScript->GetVelocity();
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
        CurrentMainState == m_PrevMainState
        ) return;

    // FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 12, -1); // Walk
    FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, m_AnimFPSTemp, -1); // Walk
    // FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 16, -1); // Attack 공격 속도 16

    m_PrevAnimDirection     = m_AnimDirection;
    m_PrevMainState         = CurrentMainState;
}

void CEnemyAnimHandler::SaveToLevelFile(FILE* _File)
{
}

void CEnemyAnimHandler::LoadFromLevelFile(FILE* _File)
{
}
