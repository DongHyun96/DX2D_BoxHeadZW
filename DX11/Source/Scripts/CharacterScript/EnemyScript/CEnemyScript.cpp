#include "pch.h"
#include "CEnemyScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/StatScript/CStatScript.h"

#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"

CEnemyScript::CEnemyScript()
    : CCharacterScript(SCRIPT_TYPE::ENEMYSCRIPT)
{
}

CEnemyScript::~CEnemyScript()
{
}

void CEnemyScript::Init()
{
    CCharacterScript::Init();
    AddScriptParam(SCRIPT_PARAM::INT, &m_EnemyType, L"Enemy Type");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_AttackDamage, L"Attack Damage");
    
    m_MoveSpeedBase = 150.f;
}

void CEnemyScript::Begin()
{
    CCharacterScript::Begin();
    ADD_DYNAMIC_BEGIN_OVERLAP(CEnemyScript::BodyColliderOverlapped);
    ADD_DYNAMIC_OVERLAP(CEnemyScript::BodyColliderOverlapped);
}

void CEnemyScript::AfterLevelBegin()
{
    if (!GM->GetEnemyPooler(m_EnemyType))
    {
        if (GetOwner()->GetOwnerPoolComponent()) // 나 자신이 PoolComponent에서 생성된 GameObject라면 GM에 등록 처리
            GM->AddEnemyPooler(m_EnemyType, GetOwner()->GetOwnerPoolComponent());
    }
}

void CEnemyScript::Tick()
{
    CCharacterScript::Tick();
    HandleFadeOut();
}

void CEnemyScript::Move()
{
    Transform()->SetPrevRelativePos(Transform()->GetRelativePos()); // 이동 처리 직전에 이전 PrevPos 저장(blocking 처리용)
    
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::ATTACK: case ENEMY_MAINSTATE::DIE: case ENEMY_MAINSTATE::END: return;
    case ENEMY_MAINSTATE::WALK:
    {
        // TODO : 실질적인 Walk 처리 구현할 것
        if (GetOwner()->GetName() != L"Devil") return;
        
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
        return;
    case ENEMY_MAINSTATE::PUSHED_OUT: MovePushedOut(); break;
    }
}

void CEnemyScript::UpdateCurrentFacedDirection()
{
    switch (m_MainState)
    {
    // PushedOut과 Die의 경우, Flipbook 관련 방향 처리가 다르기 때문에 FacedDirection Update 필요 없이 따로 처리
    case ENEMY_MAINSTATE::PUSHED_OUT: case ENEMY_MAINSTATE::DIE: case ENEMY_MAINSTATE::END: return; 
    case ENEMY_MAINSTATE::WALK:
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
    }
        return;
    case ENEMY_MAINSTATE::ATTACK:
    {
        // TODO : 공격방향을 향해 Direction 지정할 것
    }
        return;
    }
}

void CEnemyScript::AfterPushedOutFin()
{
    ENEMY_MAINSTATE NextState = GetOwner()->GetScriptComponent<CStatScript>()->IsDead()
                                         ? ENEMY_MAINSTATE::DIE : ENEMY_MAINSTATE::WALK;
    SetMainState(NextState);
}

void CEnemyScript::HandleFadeOut()
{
    if (!m_HasFadeOutStart) return;
    
    m_FadeInOutTime += DT;

    // Tint Color를 수정함으로써, FadeOut 처리 해준다
    const float ColorAlpha = MappingToNewRange(m_FadeInOutTime, 0.f, m_FadeInOutTotalTime, 1.f, 0.f);
    Vec4 TintColor = DEF_COLOR_WHITE;
    TintColor.w = ColorAlpha;

    Ptr<AMaterial> DynamicMtrl = GetRenderCom()->CreateDynamicMaterial();
    DynamicMtrl->SetScalar(VEC4_0, TintColor);
    
    if (m_FadeInOutTime < m_FadeInOutTotalTime) return; // 아직 시간이 남음
    
    m_HasFadeOutStart = false;
    m_FadeInOutTime = 0.f;
    
    // Pool 에 돌아가는 처리
    // Owner GameObject가 Pool에서 생성된 GameObject라면, IsActive 해제시, 자동적으로 들어간다
    GetOwner()->SetActive(false);
}

void CEnemyScript::OnDieFlipbookEndNotify()
{
    m_HasFadeInStart    = false;
    m_HasFadeOutStart   = true;    
    m_FadeInOutTime     = 0.f;
}

void CEnemyScript::BodyColliderOverlapped(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (_OtherCollider->GetOwner()->GetScriptComponent<CPlayerScript>())
    {
        Transform()->UpdateTransformToPrevRelativePos();
    }
}

void CEnemyScript::SaveToLevelFile(FILE* _File)
{
    CCharacterScript::SaveToLevelFile(_File);

    UINT EnemyTypeToUINT = static_cast<UINT>(m_EnemyType);
    fwrite(&EnemyTypeToUINT, sizeof(UINT), 1, _File);
    
    fwrite(&m_AttackDamage, sizeof(float), 1, _File);
}

void CEnemyScript::LoadFromLevelFile(FILE* _File)
{
    CCharacterScript::LoadFromLevelFile(_File);
    
    UINT EnemyTypeToUINT{};
    fread(&EnemyTypeToUINT, sizeof(UINT), 1, _File);
    m_EnemyType = static_cast<ENEMY_TYPE>(EnemyTypeToUINT);
    
    fread(&m_AttackDamage, sizeof(float), 1, _File);    
}
