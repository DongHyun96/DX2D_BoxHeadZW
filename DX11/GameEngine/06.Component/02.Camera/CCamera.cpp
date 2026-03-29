#include "pch.h"
#include "CCamera.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/06.RenderMgr/RenderMgr.h"

CCamera::CCamera()
    : Component(COMPONENT_TYPE::CAMERA)
    , m_LayerCheck(0)
{
}

CCamera::~CCamera()
{
}

void CCamera::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_LayerCheck,    sizeof(UINT),       1, _File);      
    fwrite(&m_Type,          sizeof(PROJ_TYPE),  1, _File);       
    fwrite(&m_Far,           sizeof(float),      1, _File);            
    fwrite(&m_Width,         sizeof(float),      1, _File);          
    fwrite(&m_AspectRatio,   sizeof(float),      1, _File);    
    fwrite(&m_FOV,           sizeof(float),      1, _File);            
    fwrite(&m_OrthoScale,    sizeof(float),      1, _File);     
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
    fread(&m_LayerCheck,    sizeof(UINT),       1, _File);      
    fread(&m_Type,          sizeof(PROJ_TYPE),  1, _File);       
    fread(&m_Far,           sizeof(float),      1, _File);            
    fread(&m_Width,         sizeof(float),      1, _File);          
    fread(&m_AspectRatio,   sizeof(float),      1, _File);    
    fread(&m_FOV,           sizeof(float),      1, _File);            
    fread(&m_OrthoScale,    sizeof(float),      1, _File);
    
    // 강제로 z값 맞추기
}

void CCamera::LayerCheck(int _Idx)
{
    m_LayerCheck ^= (1 << _Idx);
}

void CCamera::Init()
{
    // Clipping Plane -> 1 ~ 10000
    // -2500 ~ 2500 -> 맵 z 범위 (또는 y 범위)
    // -5000 ~ 5000
    if (GetOwner()->GetName() == L"UICamera")
        RenderMgr::GetInst()->RegisterUICamera(this);
}

void CCamera::Begin()
{
    // RenderMgr에 카메라(본인)을 등록
    if (GetOwner()->GetName() == L"MainCamera")
        RenderMgr::GetInst()->RegisterMainCamera(this);
}

void CCamera::FinalTick()
{
    // View 행렬 계산
    m_matView = XMMatrixInverse(nullptr, Transform()->GetWorldMatrix());
    
    /*
    // 카메라의 위치
    Vec3 vPos = Transform()->GetPos();
    
    // 이동 (카메라 위치를 원점으로 되돌리는 만큼의 이동행렬)
    Matrix matTrans = XMMatrixIdentity();
    matTrans._41    = -vPos.x;
    matTrans._42    = -vPos.y;
    matTrans._43    = -vPos.z;
    
    m_matView = matTrans;
    
    /*Vec3 RotReverse = -Transform()->GetRotation();
    
    Matrix RotReverseMat =  XMMatrixRotationZ(RotReverse.z) *
                            XMMatrixRotationY(RotReverse.y) *
                            XMMatrixRotationX(RotReverse.x);
    
    m_matView *= RotReverseMat;#1#
    
    // View 행렬 회전
    // - 카메라가 바라보는 방향을 z 축이 되도록 회전하는 부분이 추가
    // 월드상의 카메라의 Right, Up, Front 방향 벡터를 가져와서 회전행렬을 곱하면 이게 다시 X축, Y축, Z축이 되는 회전행렬을 구해야 함
    
    
    Matrix matRot = XMMatrixIdentity();

    Vec3 vR = Transform()->GetDir(DIR::RIGHT);
    Vec3 vU = Transform()->GetDir(DIR::UP);
    Vec3 vF = Transform()->GetDir(DIR::FRONT);
    
    matRot._11 = vR.x; matRot._12 = vU.x; matRot._13 = vF.x;
    matRot._21 = vR.y; matRot._22 = vU.y; matRot._23 = vF.y;
    matRot._31 = vR.z; matRot._32 = vU.z; matRot._33 = vF.z;

    // 카메라가 원점인 공간으로 이동, 카메라가 바라보는 방향을 z축으로 회전하는 회전을 적용
    m_matView = matTrans * matRot;
    */
    
    // 투영(Projection) 행렬 계산
    m_matProj = (m_Type == PROJ_TYPE::ORTHOGRAPHIC) ? XMMatrixOrthographicLH(m_Width * m_OrthoScale, (m_Width / m_AspectRatio) * m_OrthoScale, 1.f, m_Far) :
                                                         XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
}

void CCamera::SortObject()
{
    for (auto& Pair : m_mapDomainGameObject)
        Pair.second.clear();
    
    // 렌더링할 물체들을 정렬한다
    Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
    
    if (!pCurLevel) return;

    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        if (false == (m_LayerCheck & (1 << i))) continue;

        // 레이어에 소속된 모든 오브젝트를 가져온다
        Layer* pLayer = pCurLevel->GetLayer(i);
        const vector<Ptr<GameObject>>& vecObjects = pLayer->GetAllObjects();

        for (const Ptr<GameObject>& object : vecObjects)
        {
            // 오브젝틀가 렌더링을 할 수 있는 상태인지 확인
            if (!object->GetRenderCom() ||
                !object->GetRenderCom()->GetMesh() ||
                !object->GetRenderCom()->GetMaterial())
                continue;
            
            RENDER_DOMAIN Domain = object->GetRenderCom()->GetMaterial()->GetDomain();

            m_mapDomainGameObject[Domain].push_back(object.Get());
        }
    }
}

void CCamera::Render()
{
    g_Trans.matView = m_matView;
    g_Trans.matProj = m_matProj;

    // Domain 순서대로 렌더링 진행
    for (const auto& Pair : m_mapDomainGameObject)
        for (const Ptr<GameObject>& GameObject : Pair.second)
            GameObject->Render();
}
