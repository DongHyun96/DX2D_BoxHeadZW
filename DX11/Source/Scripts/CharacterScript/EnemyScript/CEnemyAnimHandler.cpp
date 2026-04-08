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
    // AddScriptParam(SCRIPT_PARAM::INT, &m_AnimFPSTemp, L"AnimFPS", true);
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

        // TODO : Runner의 경우, 근접 Straight Move일 때, 달려와야 함 (16 direction 처리로 해야함
        // FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 12, -1); // Walk
        FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 9, -1); // Walk
        // FlipbookRender()->Play(AnimCategory, FlipBookIndexByDirection, 16, -1); // Attack 공격 속도 16
    }
        break;
    case ENEMY_MAINSTATE::ATTACK:
    {
        
        // 이전 상태와 동일한 Animation이 재생 중인 상태 -> 한 번 더 재생 처리 방지
        // TODO : Runner의 경우, 공격 방향이 16방향 -> 이 방향으로 Flipbook 재생을 처리를 해야 함
        const int FlipBookIndexByDirection = static_cast<int>(CurrentDirection);
        if (CurrentDirection == m_PrevAnimDirection && CurrentMainState == m_PrevMainState)
        {
            // 같은 방향에 같은 모션인데, 해당 모션 재생이 모두 끝난 경우, 다시 재생시켜주어야 한다
            if (FlipbookRender()->GetIsStopped()) FlipbookRender()->Play(L"Attack", FlipBookIndexByDirection, 8.f, 1); // Attack 모션 시작
            return;
        }
        
        // 다른 방향 또는 MainState가 바뀌었을 때, 새로운 공격 방향으로 Attack 모션을 재생시킨다
        FlipbookRender()->Play(L"Attack", FlipBookIndexByDirection, 8.f, 1); // Attack 모션 시작
    }
        break;
    case ENEMY_MAINSTATE::PUSHED_OUT: FlipbookRender()->Stop(L"PushedOut", 0, m_PushedOutSpriteIdxToShow); break; 
    case ENEMY_MAINSTATE::DIE:
    {
        if (m_PrevMainState == ENEMY_MAINSTATE::DIE) return; // 중복재생 방지
        
        // 이전 PushedOut 방향에 따라 Die Flipbook 고르기
        const UINT BaseIdx = GetDieFlipbookIdxBase();
        vector<UINT> TempVec = {0, 3, 6}; const UINT IdxOffset = PickRandom(TempVec);
        const UINT PickedIdx = BaseIdx + IdxOffset;
        
        FlipbookRender()->Play(L"Die", PickedIdx, 10.f, 1); 
        
        // Die Flipbook 재생 모두 끝난 후, FadeOut -> Pool로 다시 돌아가는 처리
        FlipbookRender()->AddNotifyFlipbookEndEvent(L"Die", PickedIdx, bind(&CEnemyScript::OnDieFlipbookEndNotify, m_MainEnemyScript));
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
