#include "pch.h"
#include "CTurret.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

const map<TURRET_TYPE, Ptr<TurretAttackStrategy>> CTurret::m_mapAttackStrategies = 
{
    { TURRET_TYPE::MG,      new Turret_MGAttackStrategy },    
    { TURRET_TYPE::MORTAR,  new Turret_MortarAttackStrategy },    
    { TURRET_TYPE::ROCKET,  new Turret_RocketAttackStrategy }    
};


CTurret::CTurret()
    : CStructure(SCRIPT_TYPE::TURRET)
{
}

CTurret::CTurret(const CTurret& _Origin)
    : CStructure(_Origin)
    , m_TurretType(_Origin.m_TurretType)
    , m_RotateSpriteInterval(_Origin.m_RotateSpriteInterval)
    , m_AttackIntervalTotalTime(_Origin.m_AttackIntervalTotalTime)
{
}

CTurret::~CTurret()
{
}

void CTurret::Init()
{
    CStructure::Init();
    
    AddScriptParam(SCRIPT_PARAM::INT, &m_TurretType, L"Turret Type");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_RotateSpriteInterval, L"Rotate interval speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_AttackIntervalTotalTime, L"Attack Interval");
}

void CTurret::Begin()
{
    CStructure::Begin();

    Ptr<CCollider2D> AttackCollider = GetOwner()->GetChild(0)->GetCollider2D();
    if (!AttackCollider)
    {
        DebugUtil::AddDebugLog("[CTurret::Begin] : Cannot find Child AttackCollider!", DEF_COLOR_RED, 20.f);
        return;
    }
    AttackCollider->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&CTurret::AttackColliderBeginOverlap));
    AttackCollider->AddDynamicEndOverlap(this, static_cast<COLLISION_EVENT>(&CTurret::AttackColliderEndOverlap));
    
    // Init Attack Strategy
    m_AttackStrategy = m_mapAttackStrategies.at(m_TurretType).Get();
    
    // Init Flipbook category
    FlipbookRender()->SetCurrentCategory(L"Turret");
}

void CTurret::Tick()
{
    CStructure::Tick();
    
    if (GetIsPreviewObject()) return;

    switch (m_CurrentTurretState)
    {
    case TURRET_STATE::IDLE: // 새로운 TargetObject 찾아보기
    {
        m_AttackIntervalTimeChecker = 0.f;
        
        // 반경 안에 존재하는 살아있는 Enemy 중 가장 가까운 Enemy, TargetCharacter로 지정
        float minDistSqrt   = FLT_MAX; 
        m_TargetEnemyObject = nullptr;
        
        for (GameObject* EnemyObject : m_setAttackRangeEnteredEnemies)
        {
            const float CurrentDist = Vec2::DistanceSquared(EnemyObject->Transform()->GetWorldPos2D(), Transform()->GetWorldPos2D());
            if (CurrentDist < minDistSqrt && EnemyObject->GetActive())
            {
                minDistSqrt         = CurrentDist;
                m_TargetEnemyObject = EnemyObject;
            }
        }

        // 첫 Target 방향으로 Rotate 처리 -> 이 때는 공격처리가 불가능하다
        if (m_TargetEnemyObject) m_CurrentTurretState = TURRET_STATE::ROTATE;
    }
        return;
    case TURRET_STATE::ROTATE:
    {
        // Animation 계속해서 Stop 상태로 두어서 재생하는 것 처럼 따로 처리
        // 이 Turret 때문에 새로운 기능 프레임워크 추가가 너무 지엽적임
        HandleRotateToTarget();
    }
        return;
    case TURRET_STATE::ATTACK:
    {
        // TargetCharacter가 사망 또는 범위 밖을 벗어남
        if (!m_TargetEnemyObject->GetActive() || !m_setAttackRangeEnteredEnemies.contains(m_TargetEnemyObject))
        {
            m_CurrentTurretState = TURRET_STATE::IDLE; // 새로운 Target 찾기 처리
            return;
        }
        
        // Target이 잡혔으면, 돌아가는 처리를 계속해서 해주어야 함
        
        
        m_AttackIntervalTimeChecker += DT;
        if (m_AttackIntervalTimeChecker < m_AttackIntervalTotalTime) break;

        // Time to attack
        m_AttackIntervalTimeChecker -= m_AttackIntervalTotalTime;
        m_AttackStrategy->UseAttackStrategy(m_TargetEnemyObject);
    }
        return;
    case TURRET_STATE::END: return;
    }
}

void CTurret::AttackColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // Enemy 종류가 아닌 경우 무시
    if (!_OtherCollider->GetOwner()->GetScriptComponent<CEnemyScript>()) return;
    
    m_setAttackRangeEnteredEnemies.insert(_OtherCollider->GetOwner());
    
    // DebugUtil::AddDebugLog(GetOwner()->GetName() + L" : Detected Enemy count : " + to_wstring(m_setAttackRangeEnteredEnemies.size()), DEF_COLOR_CYAN, 10.f);
    
}

void CTurret::AttackColliderEndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // Enemy 종류가 아닌 경우 무시
    if (!_OtherCollider->GetOwner()->GetScriptComponent<CEnemyScript>()) return;
    
    m_setAttackRangeEnteredEnemies.erase(_OtherCollider->GetOwner());
    
    // 공격 시도 또는 공격하는 중이었던 TargetObject가 반경에서 벗어난 상황,
    // 다시 공격할 대상 재탐색
    if (m_TargetEnemyObject == _OtherCollider->GetOwner())
        m_CurrentTurretState = TURRET_STATE::IDLE;
    
    // DebugUtil::AddDebugLog(GetOwner()->GetName() + L" : Detected Enemy count : " + to_wstring(m_setAttackRangeEnteredEnemies.size()), DEF_COLOR_CYAN, 10.f);
}

void CTurret::OnRotateAnimEnd()
{
    // TargetCharacter가 사망 또는 범위 밖을 벗어남
    if (!m_TargetEnemyObject->GetActive() || !m_setAttackRangeEnteredEnemies.contains(m_TargetEnemyObject))
    {
        m_CurrentTurretState = TURRET_STATE::IDLE; // 새로운 Target 찾기 처리
        return;
    }

    // 공격 시도
    m_CurrentTurretState = TURRET_STATE::ATTACK;
}

void CTurret::HandleRotateToTarget()
{
    m_RotateSpriteTimeChecker += DT;
    
    if (m_RotateSpriteTimeChecker < m_RotateSpriteInterval) return;
    m_RotateSpriteTimeChecker -= m_RotateSpriteInterval;
    
    const Vec2 ToTarget   = m_TargetEnemyObject->Transform()->GetWorldPos2D() - Transform()->GetWorldPos2D();
    float DestAngle       = atan2f(ToTarget.y, ToTarget.x);
    
    if (DestAngle < 0.f) DestAngle += XM_2PI;
    
    int DestSpriteIdx = MappingToNewRange(DestAngle, 0.f, XM_2PI, 0.f, 32.f);
    DestSpriteIdx     = min(DestSpriteIdx, 31); // 넘을 일은 없지만(정확히 360도를 찍어야 함) 미연에 idx 넘어가는 것 방지

    const int CurrentSpriteIdx = FlipbookRender()->GetCurAnimatingSpriteIdx();  
    
    // 이미 해당 방향을 바라보는 중
    if (DestSpriteIdx == CurrentSpriteIdx) return;    
    
    int Diff = DestSpriteIdx - CurrentSpriteIdx;
    
    // 정방향으로 16칸 이상 가야한다면, 반대로 도는게 빠름 | 역방향 또한 ㅇㅇ
    if      (Diff > 16)  Diff -= 32; 
    else if (Diff < -16) Diff += 32;

    int NextSpriteIdx = 0;
    
    if (Diff > 0)
    {
        // 정방향 (++) : 31에서 +1이 되면 0으로 순환
        NextSpriteIdx = (CurrentSpriteIdx + 1) % 32; 
    }
    else
    {
        // 역방향 (--) : 0에서 -1이 되면 31로 순환 (+32를 해준 뒤 모듈러 연산)
        NextSpriteIdx = (CurrentSpriteIdx - 1 + 32) % 32;
    }

    FlipbookRender()->Stop(NextSpriteIdx);
}

void CTurret::SaveToLevelFile(FILE* _File)
{
    CStructure::SaveToLevelFile(_File);
    
    fwrite(&m_TurretType,               sizeof(TURRET_TYPE), 1, _File);
    fwrite(&m_RotateSpriteInterval,     sizeof(float), 1, _File);
    fwrite(&m_AttackIntervalTotalTime,  sizeof(float), 1, _File);
    
}

void CTurret::LoadFromLevelFile(FILE* _File)
{
    CStructure::LoadFromLevelFile(_File);
    
    fread(&m_TurretType,                sizeof(TURRET_TYPE), 1, _File);
    fread(&m_RotateSpriteInterval,      sizeof(float), 1, _File);
    fread(&m_AttackIntervalTotalTime,   sizeof(float), 1, _File);
}
