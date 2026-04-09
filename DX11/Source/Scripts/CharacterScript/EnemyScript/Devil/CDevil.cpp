#include "pch.h"
#include "CDevil.h"

#include "Source/ScriptMgr.h"
#include "CFlameLineHandler.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"

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

void CDevil::OnTakeDamage()
{
    CEnemyScript::OnTakeDamage();
}

void CDevil::HandleStateTransition()
{
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::ATTACK: case ENEMY_MAINSTATE::PUSHED_OUT: case ENEMY_MAINSTATE::DIE: case ENEMY_MAINSTATE::END: return;
        
    case ENEMY_MAINSTATE::WALK:
    {
        // TODO : 몇 초간 못 움직였는지 체킹할 것 -> 체크해서 제대로 못 움직였으면 Target null로 두어 가장 가까운 Target 지정하고 바로 공격 처리 들어갈 것 
        
        if (!IsValid(GetTargetObject())) return;
        
        // Transition to attack 시도
        m_TransitionToAttackTimer += DT;
        if (m_TransitionToAttackTimer > m_AttackTransitionWaitTime)
        {
            m_TransitionToAttackTimer = 0.f;
            m_AttackTransitionWaitTime = GetRandom(2.f, 7.5f); // 다음 공격용
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
    m_MainState = ENEMY_MAINSTATE::WALK; // 다시금 IDLE 상태격인 Walk로 돌아감
}

void CDevil::OnAttackAnimStart()
{
    const Vec2 ToTarget = GetTargetObject()->Transform()->GetRelativePosXY() - Transform()->GetRelativePosXY();
    m_FlameLineHandler->MakeFlameLine(GetVectorAngle(ToTarget), 75.f); // TODO : 오차값을 주는 처리도 조금은 넣어야 될듯?
    
    m_AttackSound->Play(1, 0.5f, true);    
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
