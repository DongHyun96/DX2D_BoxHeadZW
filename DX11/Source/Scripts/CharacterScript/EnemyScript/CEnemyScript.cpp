#include "pch.h"
#include "CEnemyScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CEnemyScript::CEnemyScript()
    : CCharacterScript(SCRIPT_TYPE::ENEMYSCRIPT)
{
}

CEnemyScript::~CEnemyScript()
{
}

void CEnemyScript::Move()
{
    Transform()->SetPrevRelativePos(Transform()->GetRelativePos()); // 이동 처리 직전에 이전 PrevPos 저장(blocking 처리용)
    
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::WALK:
    {
        m_Velocity = Vec3(); // Velocity 초기화
        Vec3 Direction{};
    
        if (KEY_PRESSED(KEY::LEFT)) Direction.x -= 1.f; // Left
        if (KEY_PRESSED(KEY::RIGHT)) Direction.x += 1.f; // Right
        if (KEY_PRESSED(KEY::UP))   Direction.y += 1.f; // Up
        if (KEY_PRESSED(KEY::DOWN)) Direction.y -= 1.f; // Down

        if (KEY_PRESSED(KEY::LSHIFT)) m_MoveSpeedFactor = 2.f;
        else m_MoveSpeedFactor = 1.f;

        if (Direction.LengthSquared() == 0.f) return;
        Direction.Normalize();
    
        m_Velocity = Direction * m_MoveSpeedBase * m_MoveSpeedFactor;
    
        Vec3 Pos = Transform()->GetRelativePos() + m_Velocity * DT;
        Transform()->SetRelativePos(Pos);
    }
        break;
    case ENEMY_MAINSTATE::ATTACK: break;
    case ENEMY_MAINSTATE::PUSHED_OUT:
    {
        if (MovePushedOut()) // MovePushedOut 처리 완료
        {
            ENEMY_MAINSTATE NextState = GetOwner()->GetScriptComponent<CStatScript>()->IsDead()
                                         ? ENEMY_MAINSTATE::DIE : ENEMY_MAINSTATE::WALK;
            SetMainState(NextState);
        }
    }
        break;
        break;
    case ENEMY_MAINSTATE::DIE:
        break;
    case ENEMY_MAINSTATE::END:
        break;
    }
    
    
}

void CEnemyScript::UpdateCurrentFacedDirection()
{
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::WALK: case ENEMY_MAINSTATE::ATTACK:
        break;
    case ENEMY_MAINSTATE::PUSHED_OUT: case ENEMY_MAINSTATE::DIE: break; // AnimHandler에서 직접 Direction 처리를 진행할 예정
        
    case ENEMY_MAINSTATE::END:
        break;
    }
    
    EDIRECTION CurrentDirection = GetEightDirection(m_Velocity);
    
    // 속력이 0인 멈춰있는 상황
    if (CurrentDirection == EDIRECTION::END)
    {
        // 만약 이전에도 END였으면, 맨 처음으로 들어오는 Update tick -> 기본 방향인 Down으로 맞춰춘다.
        if (m_CurrentFacedDirection == EDIRECTION::END)
            m_CurrentFacedDirection = EDIRECTION::DOWN;
        
        return; // 이전에 바라봤던 방향으로 처리
    }
    
    m_CurrentFacedDirection = CurrentDirection;
}
