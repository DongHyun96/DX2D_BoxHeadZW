#include "pch.h"
#include "CollisionMgr.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderRect.h"


CollisionMgr::CollisionMgr()
{
    
}

CollisionMgr::~CollisionMgr()
{
}

void CollisionMgr::Progress(const Ptr<ALevel>& _Level)
{
    UINT* pMatrix = _Level->GetCollisionMatrix();    
    
    for (UINT Row = 0; Row < MAX_LAYER; ++Row)
    {
        for (UINT Col = Row; Col < MAX_LAYER; ++Col)
        {
            if (false == (pMatrix[Row] & (1 << Col))) continue;
            CollisionBtwLayer(_Level->GetLayer(Row), _Level->GetLayer(Col));   
        }
    }
}

void CollisionMgr::CollisionBtwLayer(Layer* _Left, Layer* _Right)
{
    if (_Left == _Right) // 같은 layer 내에서의 충돌검사 판정
    {
        CollisionBtwSameLayer(_Left);
        return;
    }
    
    const vector<Ptr<GameObject>>& vecLeft  = _Left->GetAllObjects();    
    const vector<Ptr<GameObject>>& vecRight = _Right->GetAllObjects();

    for (const Ptr<GameObject>& leftObject : vecLeft)
    {
        if (!leftObject->GetCollider2D()) continue;
        
        for (const Ptr<GameObject>& rightObject : vecRight)
        {
            if (!rightObject->GetCollider2D()) continue;
            CheckCollisionAndNotify(leftObject, rightObject);
        }
    }
}

void CollisionMgr::CollisionBtwSameLayer(Layer* _Layer)
{
    const vector<Ptr<GameObject>>& vecAllObjects = _Layer->GetAllObjects();

    for (int i = 0; i < vecAllObjects.size(); ++i)
    {
        const Ptr<GameObject>& leftObject = vecAllObjects[i];
        if (!leftObject->GetCollider2D()) continue;
        
        for (int j = i + 1; j < vecAllObjects.size(); ++j)
        {
            const Ptr<GameObject>& rightObject = vecAllObjects[j];
            if (!rightObject->GetCollider2D()) continue;
            
            CheckCollisionAndNotify(leftObject, rightObject);
        }
    }
}

void CollisionMgr::CheckCollisionAndNotify(const Ptr<GameObject>& _LeftObject, const Ptr<GameObject>& _RightObject)
{
    // 두 충돌체의 고유 ID로 조합을 한 키값 생성
    COL_ID colid{};
    colid.LeftID    = _LeftObject->GetCollider2D()->GetEntityInstID();
    colid.RightID   = _RightObject->GetCollider2D()->GetEntityInstID();

    // 이전 Tick 충돌정보 불러오기
    map<ULONGLONG, bool>::iterator iter = m_mapColID.find(colid.ID);
    if (iter == m_mapColID.end())
    {
        m_mapColID.insert(make_pair(colid.ID, false));
        iter = m_mapColID.find(colid.ID);
    }

            
    // if (IsCollision(_LeftObject->Collider2D(), _RightObject->Collider2D()))          // Only for Rect vs Rect OBB Collision
    if (_LeftObject->GetCollider2D()->IsCollision(_RightObject->GetCollider2D()))       // 각 모양 및 AABB or OBB 충돌검사 알아서 선택되어 처리됨
    {
        if (iter->second) // 이전에도 충돌했었는지
        {
            _LeftObject->GetCollider2D()->Overlap(_RightObject->GetCollider2D());
            _RightObject->GetCollider2D()->Overlap(_LeftObject->GetCollider2D());
        }
        else // 이전에는 충돌하지 않았었다.
        {
            _LeftObject->GetCollider2D()->BeginOverlap(_RightObject->GetCollider2D());
            _RightObject->GetCollider2D()->BeginOverlap(_LeftObject->GetCollider2D());
        }
                
        iter->second = true;
    }
    else // 현재 충돌중이 아니다
    {
        // 이전 프레임에는 충돌 중이었다.
        if (iter->second)
        {
            _LeftObject->GetCollider2D()->EndOverlap(_RightObject->GetCollider2D());
            _RightObject->GetCollider2D()->EndOverlap(_LeftObject->GetCollider2D());
        }
                
        iter->second = false;
    }
}

bool CollisionMgr::IsCollision(const Ptr<CCollider2D>& _LeftCol, const Ptr<CCollider2D>& _RightCol)
{
    // Rect vs Rect OBB Collision testing
    
    if (!_LeftCol->GetOwner()->GetIsActive() || !_RightCol->GetOwner()->GetIsActive())  return false;    // Active가 꺼진 오브젝트
    if (_LeftCol->GetOwner()->IsDead()       || _RightCol->GetOwner()->IsDead())        return false;    // 곧 삭제 처리될 오브젝트

    Ptr<AMesh> pRectMesh = FIND_ASSET(AMesh, L"RectMesh");

    const Vtx* pVtx             = pRectMesh->GetVtxSysMem();
    const Matrix& matWorldLeft  = _LeftCol->GetWorldMat();
    const Matrix& matWorldRight =_RightCol->GetWorldMat();

    // 월드 공간상에서 충돌을 검사하기 위해서, RectMesh 모델을 각 충돌체의 월드행렬을 곱해서 정점을 충돌체 꼭지점에 배치시킨다.
    // 각 꼭지점끼리 빼서 두 충돌체의 표면 방향벡터를 각 충돌체로부터 2개씩 구한다.
    
    Vec3 Axis[4]{};
    
    // Left collider 축 초기화
    Axis[0] = XMVector3TransformCoord(pVtx[1].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);
    Axis[1] = XMVector3TransformCoord(pVtx[3].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);

    // Right Collider 축 초기화
    Axis[2] = XMVector3TransformCoord(pVtx[1].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);
    Axis[3] = XMVector3TransformCoord(pVtx[3].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);

    Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldRight) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldLeft);

    
    for (int i = 0; i < 4; ++i)
    {
        // 4개의 축 중에서, 하나를 투영 목적지로 정함
        // 원본값을 훼손하면 나중에 투영할 때 문제가 생기기 때문에, 정규화한 벡터를 따로 지역변수로 둠
        Vec3 vProjAxis = Axis[i];
        vProjAxis.Normalize();
        
        // 투영축으로 4개의 벡터를 투영시켜서 얻은 면적의 절반 길이를 구함
        float Dot{};
        
        for (const Vec3& Ax : Axis)
            Dot += fabs(vProjAxis.Dot(Ax));
        
        Dot *= 0.5f;
        
        // 두 충돌체 중심을 잇는 벡터도 투영
        float fCenter = fabs(vCenter.Dot(vProjAxis));
        
        if (fCenter > Dot) return false;
    }
    
    return true;
}
