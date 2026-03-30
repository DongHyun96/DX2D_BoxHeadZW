#include "pch.h"
#include "CEnemyAnimHandler.h"

#include "CEnemyScript.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/StatScript/EnemyStat/CEnemyStat.h"

CEnemyAnimHandler::CEnemyAnimHandler()
    : CCharacterAnimHandler(SCRIPT_TYPE::ENEMYANIMHANDLER)
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

void CEnemyAnimHandler::UpdateAnimTransition()
{
    ENEMY_MAINSTATE CurrentMainState    = m_MainEnemyScript->GetMainState();
    const Vec3 CurrentVelocity          = m_MainEnemyScript->GetVelocity();
    const EDIRECTION CurrentDirection   = m_MainEnemyScript->GetCurrentFacedDirection();

    switch (CurrentMainState)
    {
    case ENEMY_MAINSTATE::WALK:
    {
        
        const wstring& AnimCategory         = mapEnemyMainStateAnimCategory.at(CurrentMainState);
        const int FlipBookIndexByDirection = static_cast<int>(CurrentDirection);

        // 이동하고 있지 않은 상태
        if (CurrentVelocity.LengthSquared() == 0.f)
        {
            // 해당 방향으로 자연스럽게 멈춤
            // 2번 index가 멈춘 상태의 Sprite 모양
            FlipbookRender()->Stop(AnimCategory, FlipBookIndexByDirection, 2);
            m_PrevAnimDirection = EDIRECTION::END;
            return;
        }

        // 이전 상태와 동일한 Animation이 재생중인 상태 -> 한 번 더 재생 처리 방지
        if (CurrentDirection == m_PrevAnimDirection && CurrentMainState == m_PrevMainState) return;

        // FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 12, -1); // Walk
        FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, m_AnimFPSTemp, -1); // Walk
        // FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 16, -1); // Attack 공격 속도 16
    }
        break;
    case ENEMY_MAINSTATE::ATTACK:
        break;
    case ENEMY_MAINSTATE::PUSHED_OUT: FlipbookRender()->Stop(L"PushedOut", 0, m_PushedOutSpriteIdxToShow); break; 
    case ENEMY_MAINSTATE::DIE:
    {
        if (m_PrevMainState == ENEMY_MAINSTATE::DIE) return; // 중복재생 방지
        
        // 이전 PushedOut 방향에 따라 Die Flipbook 고르기
        const UINT BaseIdx = GetDieFlipbookIdxBase();
        
        vector<UINT> TempVec = {0, 3, 6}; const UINT IdxOffset = PickRandom(TempVec);
        
        FlipbookRender()->Play(L"Die", BaseIdx + IdxOffset, 10.f, 1); // TODO : Die Motion 끝난 이후, Enemy는 FadeOut 처리하기
    }
        break;
    case ENEMY_MAINSTATE::END:
        break;
    }

    m_PrevMainState         = CurrentMainState;
    m_PrevAnimDirection     = CurrentDirection;
}

void CEnemyAnimHandler::SaveToLevelFile(FILE* _File)
{
}

void CEnemyAnimHandler::LoadFromLevelFile(FILE* _File)
{
}
