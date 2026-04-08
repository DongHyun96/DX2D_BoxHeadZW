#include "pch.h"
#include "CRunner.h"

#include "Source/ScriptMgr.h"

CRunner::CRunner()
    : CEnemyScript(SCRIPT_TYPE::RUNNER)
{
    m_AttackFlipbookCount = 16;
}

CRunner::~CRunner()
{
}

void CRunner::Begin()
{
    CEnemyScript::Begin();
}

void CRunner::Tick()
{
    CEnemyScript::Tick();

    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::WALK: DebugUtil::SetPermanentDebugLog("RunnerState", "WALK", DEF_COLOR_CYAN);
        break;
    case ENEMY_MAINSTATE::ATTACK: DebugUtil::SetPermanentDebugLog("RunnerState", "ATTACK", DEF_COLOR_CYAN);
        break;
    case ENEMY_MAINSTATE::PUSHED_OUT: DebugUtil::SetPermanentDebugLog("RunnerState", "PUSHED_OUT", DEF_COLOR_CYAN);
        break;
    case ENEMY_MAINSTATE::DIE: DebugUtil::SetPermanentDebugLog("RunnerState", "DIE", DEF_COLOR_CYAN);
        break;
    case ENEMY_MAINSTATE::END: DebugUtil::SetPermanentDebugLog("RunnerState", "END", DEF_COLOR_CYAN);
        break;
    }
    
}

void CRunner::UpdateCurrentFacedDirection()
{
    // PushedOut과 Die의 경우, Flipbook 관련 방향 처리가 다르기 때문에 FacedDirection Update 필요 없이 따로 처리
    
    if (m_MainState == ENEMY_MAINSTATE::WALK)
    {
        EDIRECTION NextDirection = GetEightDirection(m_Velocity);
    
        // 속력이 0인 멈춰있는 상황
        if (NextDirection == EDIRECTION::END)
        {
            // 만약 이전에도 END였으면, 맨 처음으로 들어오는 Update tick -> 기본 방향인 Down으로 맞춰춘다.
            if (m_CurrentFacedDirection == EDIRECTION::END) m_CurrentFacedDirection = EDIRECTION::DOWN;
            return; // 이전에 바라봤던 방향으로 처리
        }
    
        m_CurrentFacedDirection = NextDirection;

        // Straight Walk 상태라면, SixteenDirection 에 대해서도 처리를 해주어야 함
        if (GetCurrentWalkType() == ENEMY_WALK_TYPE::STRAIGHT)
        {
            SIXTEEN_DIRECTION NextDirection = GetSixteenDirection(m_Velocity);
            
            // 속력이 0인 멈춰있는 상황
            if (NextDirection == SIXTEEN_DIRECTION::END)
            {
                // 만약 이전에도 END였으면, 맨 처음으로 들어오는 Update tick -> 기본 방향인 Down으로 맞춰춘다.
                if (m_CurrentFacedSixteenDirection == SIXTEEN_DIRECTION::END) m_CurrentFacedSixteenDirection = SIXTEEN_DIRECTION::DOWN;
                return; // 이전에 바라봤던 방향으로 처리
            }
    
            m_CurrentFacedSixteenDirection = NextDirection;
        }
        
        return;
    }
    
    
    
    if (m_MainState == ENEMY_MAINSTATE::ATTACK)
    {
        const Vec2 ToTarget = GetTargetObject()->Transform()->GetRelativePosXY() - Transform()->GetRelativePosXY();
        m_CurrentFacedDirection         = GetEightDirection(ToTarget.Normalized());
        m_CurrentFacedSixteenDirection  = GetSixteenDirection(ToTarget.Normalized());
    }
}

void CRunner::SetCurrentWalkType(ENEMY_WALK_TYPE _WalkType)
{
    CEnemyScript::SetCurrentWalkType(_WalkType);

    // Setting된 WalkType에 따른 SpeedFactor 조정
    m_MoveSpeedFactor = (_WalkType == ENEMY_WALK_TYPE::STRAIGHT) ? 3.f : 1.f; 
    
}
