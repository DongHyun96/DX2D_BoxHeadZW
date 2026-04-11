#include "pch.h"
#include "EAnimTransitionStrategy.h"

#include "CEnemyAnimHandler.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"
#include "Source/Scripts/CharacterScript/EnemyScript/Runner/CRunner.h"

void CommonEnemyTransitionStrategy::UseWalkStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler)
{
    ENEMY_MAINSTATE CurrentMainState    = _AnimHandler->m_MainEnemyScript->GetMainState();
    const Vec3 CurrentVelocity          = _AnimHandler->m_MainEnemyScript->GetVelocity();
    const EDIRECTION CurrentDirection   = _AnimHandler->m_MainEnemyScript->GetCurrentFacedDirection();
    
    const wstring& AnimCategory         = mapEnemyMainStateAnimCategory.at(CurrentMainState);
    const int FlipBookIndexByDirection = static_cast<int>(CurrentDirection);

    // 이동하고 있지 않은 상태
    if (CurrentVelocity.LengthSquared() == 0.f)
    {
        // 해당 방향으로 자연스럽게 멈춤
        // 2번 index가 멈춘 상태의 Sprite 모양
        _AnimHandler->FlipbookRender()->Stop(AnimCategory, FlipBookIndexByDirection, 2);
        _AnimHandler->m_PrevAnimDirection = EDIRECTION::END;
        return;
    }

    // 이전 상태와 동일한 Animation이 재생중인 상태 -> 한 번 더 재생 처리 방지
    if (CurrentDirection == _AnimHandler->m_PrevAnimDirection && CurrentMainState == _AnimHandler->m_PrevMainState) return;

    // FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 12, -1); // Walk
    _AnimHandler->FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 9, -1); // Walk
}

void CommonEnemyTransitionStrategy::UseAttackStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler)
{
    ENEMY_MAINSTATE CurrentMainState    = _AnimHandler->m_MainEnemyScript->GetMainState();
    const EDIRECTION CurrentDirection   = _AnimHandler->m_MainEnemyScript->GetCurrentFacedDirection();
    
    // 이전 상태와 동일한 Animation이 재생 중인 상태 -> 한 번 더 재생 처리 방지
    const int FlipBookIndexByDirection = static_cast<int>(CurrentDirection);
    if (CurrentDirection == _AnimHandler->m_PrevAnimDirection && CurrentMainState == _AnimHandler->m_PrevMainState)
    {
        // 같은 방향에 같은 모션인데, 해당 모션 재생이 모두 끝난 경우, 다시 재생시켜주어야 한다
        if (_AnimHandler->FlipbookRender()->GetIsStopped())
        {
            _AnimHandler->FlipbookRender()->Play(L"Attack", FlipBookIndexByDirection, _AnimHandler->m_AttackAnimFPS, 1); // Attack 모션 시작
        }
        return;
    }
        
    // 다른 방향 또는 MainState가 바뀌었을 때, 새로운 공격 방향으로 Attack 모션을 재생시킨다
    _AnimHandler->FlipbookRender()->Play(L"Attack", FlipBookIndexByDirection, 8.f, 1); // Attack 모션 시작
}

void RunnerTransitionStrategy::UseWalkStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler)
{
    // Use Common Walk Animation
    if (m_Runner->GetCurrentWalkType() != ENEMY_WALK_TYPE::STRAIGHT) // Straight이 아닌 SpawnWalk 또는, CellPath인 경우 
    {
        CEnemyAnimHandler::s_mapAnimTransitionStrategies.at(ENEMY_ANIMTRANS_TYPE::COMMON)->UseWalkStateTransitionStrategy(_AnimHandler);
        return;
    }

    // Use Sixteen run animation
    if (m_Runner->GetCurrentWalkType() == ENEMY_WALK_TYPE::STRAIGHT)
    {
        ENEMY_MAINSTATE CurrentMainState            = _AnimHandler->m_MainEnemyScript->GetMainState();
        const Vec3 CurrentVelocity                  = _AnimHandler->m_MainEnemyScript->GetVelocity();
        const SIXTEEN_DIRECTION CurrentDirection    = m_Runner->GetCurrentFacedSixteenDirection();
    
        
        const int FlipBookIndexByDirection = static_cast<int>(CurrentDirection);

        /*// 이동하고 있지 않은 상태
        if (CurrentVelocity.LengthSquared() == 0.f)
        {
            // 해당 방향으로 자연스럽게 멈춤
            // 2번 index가 멈춘 상태의 Sprite 모양
            _AnimHandler->FlipbookRender()->Stop(L"Run", FlipBookIndexByDirection, 2);
            _AnimHandler->m_PrevAnimDirection = EDIRECTION::END;
            return;
        }*/

        // 이전 상태와 동일한 Animation이 재생중인 상태 -> 한 번 더 재생 처리 방지
        if (CurrentDirection == m_PrevSixteenDirection && CurrentMainState == _AnimHandler->m_PrevMainState) return;

        _AnimHandler->FlipbookRender()->Play(L"Run", FlipBookIndexByDirection, 13, -1); // Run
        
        m_PrevSixteenDirection = CurrentDirection;
    }
}

void RunnerTransitionStrategy::UseAttackStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler)
{
    ENEMY_MAINSTATE CurrentMainState    = _AnimHandler->m_MainEnemyScript->GetMainState();
    const SIXTEEN_DIRECTION CurrentDirection   = m_Runner->GetCurrentFacedSixteenDirection();
    
    // 이전 상태와 동일한 Animation이 재생 중인 상태 -> 한 번 더 재생 처리 방지
    const int FlipBookIndexByDirection = static_cast<int>(CurrentDirection);
    if (CurrentDirection == m_PrevSixteenDirection && CurrentMainState == _AnimHandler->m_PrevMainState)
    {
        // 같은 방향에 같은 모션인데, 해당 모션 재생이 모두 끝난 경우, 다시 재생시켜주어야 한다
        if (_AnimHandler->FlipbookRender()->GetIsStopped())
        {
            _AnimHandler->FlipbookRender()->Play(L"Attack", FlipBookIndexByDirection, 8.f, 1); // Attack 모션 시작
        }
        return;
    }
        
    // 다른 방향 또는 MainState가 바뀌었을 때, 새로운 공격 방향으로 Attack 모션을 재생시킨다
    _AnimHandler->FlipbookRender()->Play(L"Attack", FlipBookIndexByDirection, 8.f, 1); // Attack 모션 시작
    
    m_PrevSixteenDirection = CurrentDirection;
}
