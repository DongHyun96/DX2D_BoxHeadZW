#include "pch.h"
#include "CEnemyAnimHandler.h"


#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"
#include "Source/Scripts/CharacterScript/EnemyScript/Runner/CRunner.h"

map<ENEMY_ANIMTRANS_TYPE, Ptr<EAnimTransitionStrategy>> CEnemyAnimHandler::s_mapAnimTransitionStrategies = 
{
    { ENEMY_ANIMTRANS_TYPE::COMMON, new CommonEnemyTransitionStrategy },
    { ENEMY_ANIMTRANS_TYPE::RUNNER, new RunnerTransitionStrategy }
};

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
    
    // Init Transition Strategies
    if (CRunner* Runner = GetOwner()->GetScriptComponent<CRunner>().Get())
    {
        RunnerTransitionStrategy* RunnerStrategy = (static_cast<RunnerTransitionStrategy*>(s_mapAnimTransitionStrategies[ENEMY_ANIMTRANS_TYPE::RUNNER].Get()))->Clone();
        RunnerStrategy->SetRunner(Runner);
        m_TransitionStrategy = RunnerStrategy; 
        
    }
    else m_TransitionStrategy = s_mapAnimTransitionStrategies[ENEMY_ANIMTRANS_TYPE::COMMON];
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
        m_TransitionStrategy->UseWalkStateTransitionStrategy(this);
    }
        break;
    case ENEMY_MAINSTATE::ATTACK:
    {
        m_TransitionStrategy->UseAttackStateTransitionStrategy(this);
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
