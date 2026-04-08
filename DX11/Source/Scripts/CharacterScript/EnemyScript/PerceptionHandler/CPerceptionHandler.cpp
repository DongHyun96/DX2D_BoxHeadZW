#include "pch.h"
#include "CPerceptionHandler.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

enum class ENEMY_WWALK_TYPE;

CPerceptionHandler::CPerceptionHandler()
    : CScript(SCRIPT_TYPE::PERCEPTIONHANDLER)
{
}

CPerceptionHandler::~CPerceptionHandler()
{
}

void CPerceptionHandler::Begin()
{
    // 자신의 자식 오브젝트 중, Attack Collider 오브젝트 저장
    m_AttackColliderObject = GetOwner()->GetChildByName(L"AttackColliderObject").Get();
    
    Ptr<GameObject> AttackRangeObj = GetOwner()->GetChildByName(L"AttackRangeDetectionObject");
    
    AttackRangeObj->GetCollider2D()->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&CPerceptionHandler::OnAttackAreaColliderBeginOverlap));
    AttackRangeObj->GetCollider2D()->AddDynamicEndOverlap(this, static_cast<COLLISION_EVENT>(&CPerceptionHandler::OnAttackAreaColliderEndOverlap));
    
    Ptr<GameObject> MoveStraightDetectionObj = GetOwner()->GetChildByName(L"MoveStraightDetectionObject");
    
    MoveStraightDetectionObj->GetCollider2D()->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&CPerceptionHandler::OnStraightThroughColliderBeginOverlap));
    MoveStraightDetectionObj->GetCollider2D()->AddDynamicEndOverlap(this, static_cast<COLLISION_EVENT>(&CPerceptionHandler::OnStraightThroughColliderEndOverlap));
    
    // 자기자신의 MainEnemyScript 초기화
    m_MainEnemyScript = GetOwner()->GetScriptComponent<CEnemyScript>().Get();
}

void CPerceptionHandler::Tick()
{
    // Valid한 TargetObject가 없을 때, 여기서 Perception에 걸린 상태에 따른 TargetObject 및 State transition을 시도할 수 있다
    if (!IsValid(m_MainEnemyScript->GetTargetObject()))
    {
        m_MainEnemyScript->SetTargetObject(nullptr);
        
        // 새로운 TargetObject 탐색 -> 우선순위(1. Attack Area 2. StraightThrough)
        if (!m_setAttackAreaEnteredObjects.empty())
        {
            // FSM setting을 Attack으로 준다
            m_MainEnemyScript->SetTargetObject(*(m_setAttackAreaEnteredObjects.begin()));
            m_MainEnemyScript->SetMainState(ENEMY_MAINSTATE::ATTACK);
            return;
        }
        
        if (!m_setStraightThroughDetectionEnteredObjects.empty())
        {
            // 후보군 중, 가장 가까운 후보군으로 이동하기
            float MinDist = FLT_MAX;
            GameObject* Target{};
            for (GameObject* Object : m_setStraightThroughDetectionEnteredObjects)
            {
                float CurDist = Vec2::DistanceSquared(Object->Transform()->GetRelativePosXY(), Transform()->GetRelativePosXY());
                if (CurDist < MinDist)
                {
                    MinDist = CurDist;
                    Target = Object;
                }
            }
            
            m_MainEnemyScript->SetTargetObject(Target);
            m_MainEnemyScript->SetCurrentWalkType(ENEMY_WALK_TYPE::STRAIGHT);
        }
    }
}

void CPerceptionHandler::OnStraightThroughColliderBeginOverlap(CCollider2D* _StraightThroughCollider, CCollider2D* _OtherCollider)
{
    // New Sight received (Tick보다 여기가 더 일찍 들어오게 됨)
    
    m_setStraightThroughDetectionEnteredObjects.insert(_OtherCollider->GetOwner());
    
    // Valid한 TargetObject가 없는 경우
    if (!IsValid(m_MainEnemyScript->GetTargetObject()))
    {
        //StraightThrough 목록 중 가장 가까운 오브젝트를 TargetObject로 세팅하고 WalkStrategy를 바꾼다
        m_MainEnemyScript->SetTargetObject(_OtherCollider->GetOwner());
        
    }
    
    // TargetObject가 존재하는 경우
    // 현재 공격중이라면 return
    if (m_MainEnemyScript->GetMainState() == ENEMY_MAINSTATE::ATTACK) return;
    
    
    // TargetObject와 같은 오브젝트인 경우 (Walk Strategy만 staright 처리)
    if (m_MainEnemyScript->GetTargetObject() == _OtherCollider->GetOwner())
    {
        m_MainEnemyScript->SetCurrentWalkType(ENEMY_WALK_TYPE::STRAIGHT);
        return;
    }
    
    const float CurDistToTarget = Vec2::DistanceSquared(m_MainEnemyScript->GetTargetObject()->Transform()->GetRelativePosXY(), Transform()->GetRelativePosXY());
    const float NewDist = Vec2::DistanceSquared(_OtherCollider->Transform()->GetRelativePosXY(), Transform()->GetRelativePosXY());
    
    // TargetObject와 다른 오브젝트이고, 거리가 더 가까운 경우, 새로운 TargetObject로 설정 처리
    if (NewDist < CurDistToTarget)
    {
        m_MainEnemyScript->SetTargetObject(_OtherCollider->GetOwner());
        m_MainEnemyScript->SetCurrentWalkType(ENEMY_WALK_TYPE::STRAIGHT);        
    }
}

void CPerceptionHandler::OnStraightThroughColliderEndOverlap(CCollider2D* _StraightThroughCollider, CCollider2D* _OtherCollider)
{
    // Lose sight
    m_setStraightThroughDetectionEnteredObjects.erase(_OtherCollider->GetOwner());

    // 현재 공격이 진행중이라면 return
    if (m_MainEnemyScript->GetMainState() == ENEMY_MAINSTATE::ATTACK) return;

    // MainState가 공격이 아니고, TargetObject와 같은 오브젝트인 경우 -> Target Lose sight
    if (m_MainEnemyScript->GetTargetObject() == _OtherCollider->GetOwner())
    {
        m_MainEnemyScript->SetTargetObject(nullptr);
    }
}

void CPerceptionHandler::OnAttackAreaColliderBeginOverlap(CCollider2D* _AttackAreaCollider, CCollider2D* _OtherCollider)
{
    // New Sight Received
    m_setAttackAreaEnteredObjects.insert(_OtherCollider->GetOwner());

    // 현재의 TargetObject가 valid하지 않다면, 새로이 TargetObject 지정 및 Attack 상태 지정
    if (!IsValid(m_MainEnemyScript->GetTargetObject()))
    {
        m_MainEnemyScript->SetTargetObject(_OtherCollider->GetOwner());
        m_MainEnemyScript->SetMainState(ENEMY_MAINSTATE::ATTACK);
        return;
    }

    // 현재 TargetObject가 Valid한 상황
    // Attack상태전환만 따로 처리
    m_MainEnemyScript->SetMainState(ENEMY_MAINSTATE::ATTACK);
}

void CPerceptionHandler::OnAttackAreaColliderEndOverlap(CCollider2D* _AttackAreaCollider, CCollider2D* _OtherCollider)
{
    // Lose sight
    m_setAttackAreaEnteredObjects.erase(_OtherCollider->GetOwner());
    
    // 현재의 공격대상이 Attack 반경을 벗어난 상황 (사망이든 뭐든)
    if (m_MainEnemyScript->GetTargetObject() == _OtherCollider->GetOwner())
    {
        // 공격 반경에 들어있는 다른 오브젝트가 있다면, 해당 오브젝트 공격
        if (!m_setAttackAreaEnteredObjects.empty())
        {
            m_MainEnemyScript->SetTargetObject(*(m_setAttackAreaEnteredObjects.begin()));
            m_MainEnemyScript->SetMainState(ENEMY_MAINSTATE::ATTACK);
            return;
        }

        // 새로운 Target 찾기로 넘어갈 것
        m_MainEnemyScript->SetTargetObject(nullptr);
        
    }
    
    
}
