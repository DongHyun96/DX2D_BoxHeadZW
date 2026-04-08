#include "pch.h"
#include "CEnemyScript.h"

#include "EnemyWalkStrategy/EnemyWalkStrategy.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "PerceptionHandler/CPerceptionHandler.h"
#include "Source/ScriptMgr.h"
#include "Source/AStar/AStarPathFinder.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/StatScript/CStatScript.h"

#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"

map<ENEMY_WALK_TYPE, Ptr<EnemyWalkStrategy>> CEnemyScript::s_mapWalkingStrategies = 
{
    { ENEMY_WALK_TYPE::CELL_PATH, new EnemyWalkThroughCellPathStrategy },
    { ENEMY_WALK_TYPE::STRAIGHT, new EnemyWalkStraightStrategy }
};

CEnemyScript::CEnemyScript()
    : CCharacterScript(SCRIPT_TYPE::ENEMYSCRIPT)
{
}

CEnemyScript::~CEnemyScript()
{
}

CEnemyScript::CEnemyScript(SCRIPT_TYPE _Type)
    : CCharacterScript(_Type)
{
}

void CEnemyScript::Init()
{
    CCharacterScript::Init();
    AddScriptParam(SCRIPT_PARAM::INT, &m_EnemyType, L"Enemy Type");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_AttackDamage, L"Attack Damage");
    
    m_MoveSpeedBase = 150.f;
    // m_MoveSpeedBase = 300.f;
}

void CEnemyScript::Begin()
{
    CCharacterScript::Begin();
    ADD_DYNAMIC_BEGIN_OVERLAP(CEnemyScript::BodyColliderOverlapped);
    ADD_DYNAMIC_OVERLAP(CEnemyScript::BodyColliderOverlapped);

    for (int FlipbookIdx = 0; FlipbookIdx < m_AttackFlipbookCount; ++FlipbookIdx)
    {
        FlipbookRender()->AddNotifyFlipbookEndEvent(L"Attack", FlipbookIdx, bind(&CEnemyScript::OnAttackFlipbookEndNotify, this));
        FlipbookRender()->AddNotifyFlipbookOnSpriteIdx(L"Attack", FlipbookIdx, 2, bind(&CEnemyScript::OnAttackNotify, this)); // Devil의 경우, 해당 처리 빼는 내용 추가해둠
    }
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
    HandleStateTransition();
    CCharacterScript::Tick(); // Handling Walk & UpdateCurrentFacedDirection involved
    HandleFadeOut();
    
    DebugUtil::SetPermanentDebugLog("Enemy State", "asdf", DEF_COLOR_RED);
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::WALK: DebugUtil::SetPermanentDebugLog("Enemy State", "State : WALK", DEF_COLOR_RED);
        break;
    case ENEMY_MAINSTATE::ATTACK: DebugUtil::SetPermanentDebugLog("Enemy State", "State : ATTACK", DEF_COLOR_RED);
        break;
    case ENEMY_MAINSTATE::PUSHED_OUT: DebugUtil::SetPermanentDebugLog("Enemy State", "State : PUSHED_OUT", DEF_COLOR_RED);
        break;
    case ENEMY_MAINSTATE::DIE: DebugUtil::SetPermanentDebugLog("Enemy State", "State : DIE", DEF_COLOR_RED);
        break;
    case ENEMY_MAINSTATE::END: DebugUtil::SetPermanentDebugLog("Enemy State", "State : END", DEF_COLOR_RED);
        break;
    }
}

void CEnemyScript::Move()
{
    Transform()->SetPrevRelativePos(Transform()->GetRelativePos()); // 이동 처리 직전에 이전 PrevPos 저장(blocking 처리용)
    
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::ATTACK:
        
    if (!IsValid(m_TargetObject)) m_MainState = ENEMY_MAINSTATE::WALK;
        
        break;
    case ENEMY_MAINSTATE::DIE: case ENEMY_MAINSTATE::END: return;
        
    case ENEMY_MAINSTATE::WALK:
    {
        s_mapWalkingStrategies[m_CurrentWalkType]->UseWalkStrategy(this);
    }
        break;
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
        UpdateAttackFacedDirection();
        // TODO : 공격방향을 향해 Direction 지정할 것
        // 근데 여기서도, Runner의 경우 16 direction 으로 처리가 됨
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

void CEnemyScript::HandleStateTransition()
{
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::PUSHED_OUT: case ENEMY_MAINSTATE::DIE: case ENEMY_MAINSTATE::END: return;
        
    case ENEMY_MAINSTATE::WALK:
    {
        CPerceptionHandler* PHandler = GetOwner()->GetScriptComponent<CPerceptionHandler>().Get();
        
        // Attack 반경에 들어온 오브젝트가 있다면 Attack 시도
        if (PHandler->GetFirstAttackAreaObject())
        {
            m_TargetObject = PHandler->GetFirstAttackAreaObject();
            m_MainState = ENEMY_MAINSTATE::ATTACK;
            return;
        }
        
        // 나머지는 Walk 상태 지정 처리
        
        if (m_CurrentWalkType == ENEMY_WALK_TYPE::STRAIGHT)
        {
            // TargetObject가 사망 또는 사라진 경우, 또는 현재 TargetObject가 Straight Walk 반경에서 벗어난 오브젝트인 경우
            if (!IsValid(m_TargetObject) || !PHandler->IsStraightThroughDetectionSetContainObject(m_TargetObject.Get()))
            {
                // CellPath Walk로 지정해서 다음 Target(Walking Strategy 초반에 설정이 된다)로 이동
                m_CurrentWalkType = ENEMY_WALK_TYPE::CELL_PATH;
                return;
            }
        }
        
        if (m_CurrentWalkType == ENEMY_WALK_TYPE::CELL_PATH)
        {
            // StraightThrough 영역에 들어온 Object가 있다면, 해당 GameObject로 Target 세팅 및 Walk Strategy 세팅
            if (GameObject* Object = PHandler->GetNearestStraightThroughDetectionEnteredObject())
            {
                m_TargetObject = Object;
                m_CurrentWalkType = ENEMY_WALK_TYPE::STRAIGHT;
            }
        }
    }
        return;
    case ENEMY_MAINSTATE::ATTACK:
    {
        // 이미 공격중이라면, 다른 처리 x -> 해당 공격모션 기다리기
        if (m_HasAttackStart) return;
        
        // 첫 공격 처리 여기로 들어옴
        m_HasAttackStart = true;
        
        // TODO : Devil의 경우 override한 StateTransition 함수에서 이 처리 빼기

        // Attack Collider Rotation 설정(방향 설정) / 켜주기 처리
        // Attack Animation 특정 Sprite idx에서 켜줄 예정 (바로 켜버리면 어색함)
        /*const Vec2 ToTarget = m_TargetObject->Transform()->GetRelativePosXY() - Transform()->GetRelativePosXY();
        GetOwner()->GetScriptComponent<CPerceptionHandler>()->ToggleDamagingCollider(true, GetVectorAngle(ToTarget));*/
    }
        return;
    }
}

void CEnemyScript::UpdateAttackFacedDirection()
{
    // TODO : Faced Direction 지정할 것 -> Runner의 경우 16방향에 대한 처리를 해야해서 virtual로 뚫어둠
    const Vec2 ToTarget = m_TargetObject->Transform()->GetRelativePosXY() - Transform()->GetRelativePosXY();
    m_CurrentFacedDirection = GetEightDirection(ToTarget.Normalized());
}

void CEnemyScript::MoveThroughCellPath()
{
    Vec3 Pos = Transform()->GetRelativePos();
    
    // 임시 Targeting -> 정상적인 게임 작동중에는 무조건 Target이 있음(Player가 살아 있을 동안)
    if (KEY_TAP(KEY::MRB))
    {
        const CellCoord myCellCoord = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(ToVec2(Pos));
        const CellCoord destCellCoord = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(KeyMgr::GetInst()->GetMouseWorldPos2D());
        AStarPathFinder::GetInst()->GetPath(myCellCoord, destCellCoord, m_CellPath); // return true or false
    }
    
    // 이동할 수 있는 경로가 없음
    if (m_CellPath.empty()) return;
    
    m_Velocity = Vec3::Zero;
    
    const Vec2 Destination = GM->GetBackgroundCellManager()->GetCellCoordToWorldPos(m_CellPath.top()); 
    const Vec2 Direction = Destination - ToVec2(Pos);
    
    const float DistToDest = Direction.Length();
    const float MoveDistThisFrame = m_MoveSpeedBase * DT;
    
    // 이번 이동 거리보다 남은 거리가 작다면 도착한 것으로 판정
    if (DistToDest <= MoveDistThisFrame) 
    {
        Transform()->SetRelativePos(ToVec3(Destination, Destination.y)); // Dest로 위치보정 처리
        m_CellPath.pop(); 
        return;        
    }
    
    m_Velocity = ToVec3(Direction / (DistToDest == 0.f ? FLT_EPSILON : DistToDest) ) * m_MoveSpeedBase;
    Pos += m_Velocity * DT;
    
    Transform()->SetRelativePos(Pos);
}

void CEnemyScript::OnDieFlipbookEndNotify()
{
    m_HasFadeInStart    = false;
    m_HasFadeOutStart   = true;    
    m_FadeInOutTime     = 0.f;
}

void CEnemyScript::OnAttackFlipbookEndNotify()
{
    // 공격 모션이 정상 종료 또는 Interrupt 당했을 때 해당 함수로 Callback이 들어옴
    m_HasAttackStart = false;
    
    // Attack Collider Rotation 설정(방향 설정) / 끄기 처리
    GetOwner()->GetScriptComponent<CPerceptionHandler>()->ToggleDamagingCollider(false);

    if (m_MainState == ENEMY_MAINSTATE::DIE || m_MainState == ENEMY_MAINSTATE::PUSHED_OUT) return;
    
    m_MainState = ENEMY_MAINSTATE::WALK;
}

void CEnemyScript::OnAttackNotify()
{
    const Vec2 ToTarget = m_TargetObject->Transform()->GetRelativePosXY() - Transform()->GetRelativePosXY();
    GetOwner()->GetScriptComponent<CPerceptionHandler>()->ToggleDamagingCollider(true, GetVectorAngle(ToTarget));
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
