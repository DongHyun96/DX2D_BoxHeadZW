
#include "pch.h"
#include "CGrenade.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

// const float CGrenade::s_Gravity = 9.8f;
const float CGrenade::s_Gravity = 980.f;
const float CGrenade::s_BounceDamping = 0.55f; // 바닥에 튕길 때, 잃는 에너지 ( 0 ~ 1)
const float CGrenade::s_GroundFriction = 0.8f; // 바닥에 튕길 때 x, y 축으로 감속되는 비율

CGrenade::CGrenade()
    : CScript(SCRIPT_TYPE::GRENADE)
{
}

CGrenade::~CGrenade()
{
}

void CGrenade::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CGrenade::HandleOverlap);
    ADD_DYNAMIC_OVERLAP(CGrenade::HandleOverlap);
    m_ScaleBase = Transform()->GetRelativeScale();
}

void CGrenade::AfterLevelBegin()
{
    if (!GM->GetGrenaderPooler())
    {
        if (CPoolComponent* PoolComponent = GetOwner()->GetOwnerPoolComponent())
            GM->SetGrenadePooler(PoolComponent);
    }
}

void CGrenade::Tick()
{
    m_PrevLogicalPos = m_LogicalPos;
    
    // Z축 (높이) 중력 적용
    m_Velocity.z -= s_Gravity * DT;

    // 논리적 위치 업데이트
    m_LogicalPos += m_Velocity * DT;

    // 지면 충돌 및 바운스 처리 (Z가 0 이하로 떨어질 때)
    if (m_LogicalPos.z <= 0.f)
    {
        m_LogicalPos.z = 0.f;

        if (m_BounceCount > 0)
        {
            // Z축 속도 반전 및 탄성 감쇠
            m_Velocity.z = -(m_Velocity.z * s_BounceDamping);

            // X, Y축 마찰력 적용
            m_Velocity.x *= s_GroundFriction;
            m_Velocity.y *= s_GroundFriction;

            m_BounceCount--;
        }
        else
        {
            // 튕김 종료: 수류탄 정지
            m_Velocity = Vec3::Zero;
            
            // GM->SpawnExplosionDome(Transform()->GetWorldPos(), 1.5f, 50.f, 50.f, this);
            GM->SpawnExplosion(Transform()->GetWorldPos(), 1.f, 500.f, 50.f, this);
            GetOwner()->SetActive(false);

            // SubGrenade 추가로 Spawn처리할 경우
            if (m_SpawnSubGrenade)
            {
                for (int i = 0; i < 4; ++i)
                {
                    const Vec2 Direction = GetSpreadVector(Vec2::UnitX, XM_PIDIV4 + i * XM_PIDIV2);
                    GM->SpawnGrenade(Transform()->GetWorldPos(), Direction, m_DamageAmount * 0.5f, 0, 150.f, 250.f, false, true);
                }
            }
        }
    }

    // 실제 Transform 동기화 처리
    
    // 논리적 Y 위치에 높이(Z)를 더하여 화면상 위쪽으로 띄워 그림
    Vec3 renderPos = m_LogicalPos;
    renderPos.y   += m_LogicalPos.z;
    renderPos.z = renderPos.y; // z ordering 처리

    // Transform 갱신 (Z-Order 정렬 기준값이 꼬이지 않도록 주의해야 합니다)
    Transform()->SetRelativePos(renderPos);
}

void CGrenade::SetSubGrenadeScale() const
{
    Transform()->SetRelativeScale(m_ScaleBase * 0.6f);
}

void CGrenade::SetMainGrenadeScale() const
{
    Transform()->SetRelativeScale(m_ScaleBase);
}

/*void CGrenade::HandleOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // 이전 위치로 위치 조정 처리
    m_LogicalPos = m_PrevLogicalPos;

    CTransform* pOwnerTrans = _OwnerCollider->Transform();
    CTransform* pOtherTrans = _OtherCollider->Transform();

    Vec3 ownerPos = pOwnerTrans->GetWorldPos();
    Vec3 otherPos = pOtherTrans->GetWorldPos();

    // 2. 충돌 법선(Normal) 벡터 구하기
    // [중요] 현재 BeginOverlap은 충돌 '발생 여부'만 알려줍니다. 
    // OBB 충돌(SAT 알고리즘 등) 환경에서 정확한 반사를 하려면, 부딪힌 면의 수직 벡터(Normal)를 알아야 합니다.
    // 만약 물리 엔진 단에서 충돌 Normal을 제공하지 않는다면, 임시로 중심점 간의 방향을 Normal로 사용합니다.
    
    Vec2 normal{};
    
    if (CColliderRect* ColliderRect = dynamic_cast<CColliderRect*>(_OtherCollider))
    {
        const Vec3 ClosestPoint = ColliderRect->GetCircleClosestPointContacted();
        normal = ToVec2(ownerPos - ClosestPoint);
    }
    else normal = ToVec2(ownerPos- otherPos);
    
    normal.Normalize(); 
    
    // 3. 반사 벡터 공식 적용: R = V - 2(V · N)N
    // Z축 속도는 보존하고 X, Y 평면 속도만 튕겨냅니다.
    const Vec2 currentVelXY = ToVec2(m_Velocity);
    
    float dotProduct = currentVelXY.Dot(normal);
    
    // 수류탄이 이미 다른 방향으로 밀려나고 있는 중첩 충돌(다중 충돌) 방지
    if (dotProduct > 0.f) return;

    m_Velocity.x = currentVelXY.x - (2.f * dotProduct * normal.x);
    m_Velocity.y = currentVelXY.y - (2.f * dotProduct * normal.y);

    // 벽에 부딪혔을 때도 약간의 에너지를 잃도록 처리 (선택 사항)
    m_Velocity.x *= s_GroundFriction;
    m_Velocity.y *= s_GroundFriction;
    
    // 필요 시 벽 바운스 횟수를 차감하거나 사운드를 재생할 수 있습니다.
}*/

void CGrenade::HandleOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    CTransform* pOwnerTrans = _OwnerCollider->Transform();
    CTransform* pOtherTrans = _OtherCollider->Transform();

    Vec3 ownerPos = pOwnerTrans->GetWorldPos();
    Vec3 otherPos = pOtherTrans->GetWorldPos();

    Vec2 normal{};
    
    // 1. OBB 사각형과의 충돌일 경우 완벽한 위치 보정 적용
    if (CColliderRect* ColliderRect = dynamic_cast<CColliderRect*>(_OtherCollider))
    {
        const Vec3 ClosestPoint = ColliderRect->GetCircleClosestPointContacted();
        
        // ClosestPoint에서 수류탄 중심을 향하는 벡터 (이것이 밀어낼 방향이자 반사 Normal입니다)
        Vec2 toCenter = ToVec2(ownerPos - ClosestPoint);
        float distance = toCenter.Length();
        
        if (distance > 0.0001f) // 중심이 완전히 겹친 Zero Divide 방지
        {
            normal = toCenter / distance; // Normalize
        }
        else
        {
            // 중심이 완벽히 겹쳤을 때의 예외 처리 (임의로 위로 튕겨냄)
            normal = Vec2(0.f, 1.f); 
        }

        // [중요] CColliderCircle 등에서 실제 수류탄의 콜라이더 반지름을 가져와야 합니다.
        // float radius = dynamic_cast<CColliderCircle*>(_OwnerCollider)->GetRadius();
        float radius = 15.f; // ★여기에 실제 수류탄 반지름 값을 대입하세요!
        
        // 파고든 깊이 계산
        float penetration = radius - distance;
        
        // 벽 안으로 파고들었다면 밖으로 밀어냅니다.
        if (penetration > 0.f)
        {
            m_LogicalPos.x += normal.x * penetration;
            m_LogicalPos.y += normal.y * penetration;
        }
    }
    else 
    {
        // 사각형이 아닌 다른 모양(원 vs 원 등)일 때는 안전하게 이전 위치 롤백 사용
        m_LogicalPos = m_PrevLogicalPos;
        
        normal = ToVec2(ownerPos - otherPos);
        normal.Normalize();
    }

    // 2. 반사 벡터 공식 적용: R = V - 2(V · N)N
    const Vec2 currentVelXY = ToVec2(m_Velocity);
    
    float dotProduct = currentVelXY.Dot(normal);
    
    // 수류탄이 이미 다른 방향으로 밀려나고 있는 중첩 충돌(다중 충돌) 방지
    if (dotProduct > 0.f) return;

    m_Velocity.x = currentVelXY.x - (2.f * dotProduct * normal.x);
    m_Velocity.y = currentVelXY.y - (2.f * dotProduct * normal.y);

    // 벽에 부딪혔을 때 마찰력 감쇠
    m_Velocity.x *= s_GroundFriction;
    m_Velocity.y *= s_GroundFriction;
}