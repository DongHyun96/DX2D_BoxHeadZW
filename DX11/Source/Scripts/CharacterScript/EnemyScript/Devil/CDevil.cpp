#include "pch.h"
#include "CDevil.h"

#include "Source/ScriptMgr.h"
#include "CFlameLineHandler.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/EnemyScript/EnemyWalkStrategy/EnemyWalkStrategy.h"

CDevil::CDevil()
    : CEnemyScript(SCRIPT_TYPE::DEVIL)
{
}

CDevil::~CDevil()
{
}

void CDevil::Init()
{
    CEnemyScript::Init();
    AddScriptParam(SCRIPT_PARAM::SOUND, &m_AttackSound, L"Attack Sound");
}

void CDevil::Begin()
{
    CEnemyScript::Begin();

    m_FlameLineHandler = GetOwner()->GetChild(0)->GetScriptComponent<CFlameLineHandler>().Get();
    
    for (int i = 0; i < 8; ++i)
    {
        // Sprite 중간 지점에서의 Attack Notify를 받지 않음(해당 함수에서는 CPerceptionHandler가 들고 있는 AttackDamage 반경 Collider를 켜고 끄는 처리를 한다)
        FlipbookRender()->RemoveNotifyFlipbookOnSpriteIdx(L"Attack", i);
        
        // 대신 Attack Flipbook 시작 시, 불쏘시개 Effect 스폰 처리 함수
        FlipbookRender()->AddNotifyFlipbookStartEvent(L"Attack", i, bind(&CDevil::OnAttackAnimStart, this));
    }

    m_AttackTransitionWaitTime = GetRandom(10.f, 20.f);
}

void CDevil::Tick()
{
    CEnemyScript::Tick();
}

void CDevil::InitSpawn()
{
    CEnemyScript::InitSpawn();
    m_SameCoordStayTime = 0.f;

    m_FlameLineHandler->InitWaitState();

    m_AttackTransitionWaitTime = 0.f;
    m_TransitionToAttackTimer  = 0.f;
    m_LastDamageCauser         = nullptr;
}

void CDevil::OnTakeDamage(GameObject* _DamageCauser)
{
    // if (GetRandom(0, 1)) SetTargetObject()
    m_LastDamageCauser = _DamageCauser;
}

void CDevil::AfterPushedOutFin()
{
    CEnemyScript::AfterPushedOutFin();
    
    // Die 상태가 아니라면 80%의 확률로 피격을 준 Target으로 지정해서 공격 처리를 시도한다
    if (m_MainState != ENEMY_MAINSTATE::DIE)
    {
        if (IsValid(m_LastDamageCauser))
        {
            m_FlameLineHandler->InitWaitState();
            if (GetRandom(0.f, 1.f) > 0.2f)
            {
                SetMainState(ENEMY_MAINSTATE::ATTACK);
                SetTargetObject(m_LastDamageCauser);
            }
        }
    }
}

void CDevil::HandleStateTransition()
{
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::ATTACK: case ENEMY_MAINSTATE::PUSHED_OUT: case ENEMY_MAINSTATE::END: case ENEMY_MAINSTATE::DIE: return;
    case ENEMY_MAINSTATE::WALK:
    {
        if (!IsValid(GetTargetObject())) return;
        
        // 몇 초간 제대로 못 움직였으면 Target null로 두어 가장 가까운 Target 지정하고 바로 공격 처리 들어갈 것
        const CellCoord CurCell = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(Transform()->GetRelativePosXY());
        if (CurCell == m_PrevCellCoord)
        {
            static const float BlockedTimeWaitTotal = 2.5f; 
            m_SameCoordStayTime += DT;
            
            if (m_SameCoordStayTime > BlockedTimeWaitTotal) // 넘어가면 막혔다고 판단, 새로운 Target 지정해서 바로 공격 시도
            {
                GameObject* NewTarget = EnemyWalkStrategy::FindNearestTargetFromAllObjects(this);
                SetTargetObject(NewTarget); // 여기서 Attack 처리를 TargetObject 쪽으로 하는 것이 아닌, 현재 이동 방면으로 처리하면 좋음
                SetMainState(ENEMY_MAINSTATE::ATTACK);
            }
            
        }
        m_PrevCellCoord = CurCell;
        
        // Transition to attack 시도
        m_TransitionToAttackTimer += DT;
        if (m_TransitionToAttackTimer > m_AttackTransitionWaitTime)
        {
            SetMainState(ENEMY_MAINSTATE::ATTACK);
        }
    }
        return;
    }
}

void CDevil::OnAttackFlipbookEndNotify()
{
    // 공격 모션이 정상 종료 또는 Interrupt 당했을 때 해당 함수로 Callback이 들어옴
    if (m_MainState == ENEMY_MAINSTATE::DIE || m_MainState == ENEMY_MAINSTATE::PUSHED_OUT) return;
    SetMainState(ENEMY_MAINSTATE::WALK); // 다시금 IDLE 상태격인 Walk로 돌아감
    
}

void CDevil::OnAttackAnimStart()
{
    const Vec2 ToTarget = GetTargetObject()->Transform()->GetRelativePosXY() - Transform()->GetRelativePosXY();
    
    // 70%의 확률로 정방향 공격
    const float DirectAngle = GetVectorAngle(ToTarget);
    if (GetRandom(0.f, 1.f) > 0.3f)
        m_FlameLineHandler->MakeFlameLine(DirectAngle, 75.f);
    else m_FlameLineHandler->MakeFlameLine(DirectAngle + GetRandom(-XM_PIDIV4 * 0.5f, XM_PIDIV4 * 0.5f), 75.f); // 나머지 50%의 확률로 살짝 어긋난 방향으로 공격 시도
    
    m_AttackSound->Play(1, 0.5f, true);    
}

void CDevil::SetMainState(ENEMY_MAINSTATE _MainState)
{
    ENEMY_MAINSTATE PrevMainState = m_MainState;
    CEnemyScript::SetMainState(_MainState);
    
    // 이전 상태가 Walk였고 Attack으로 넘어가는 시점이라면, Walk 상태에서의 사용하는 변수 초기화를 해둔다
    if (PrevMainState == ENEMY_MAINSTATE::WALK && _MainState == ENEMY_MAINSTATE::ATTACK) 
    {
        m_SameCoordStayTime        = 0.f;
        m_TransitionToAttackTimer  = 0.f;
        m_AttackTransitionWaitTime = GetRandom(2.f, 5.f); // 다음 공격용    
    }
    
}

void CDevil::SaveToLevelFile(FILE* _File)
{
    CEnemyScript::SaveToLevelFile(_File);
    SaveAssetRef(_File, m_AttackSound.Get());
}

void CDevil::LoadFromLevelFile(FILE* _File)
{
    CEnemyScript::LoadFromLevelFile(_File);
    m_AttackSound = LoadAssetRef<ASound>(_File);
}
