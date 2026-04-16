#include "pch.h"
#include "CCamera.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/06.RenderMgr/RenderMgr.h"
#include "GameEngine/06.Component/RenderComponent/02.BillboardRender/BillboardRenderInstancing.h"

#include "GameEngine/06.Component/RenderComponent/03.SpriteRender/CSpriteRender.h"
#include "GameEngine/06.Component/RenderComponent/03.SpriteRender/SpriteRenderInstancing.h"
#include "GameEngine/06.Component/RenderComponent/04.FlipbookRender/FlipbookRenderInstancing.h"

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
    
    fwrite(&m_bIsFirstMainCamera,   sizeof(bool), 1, _File);
    fwrite(&m_bIsUICamera,          sizeof(bool), 1, _File);
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

    fread(&m_bIsFirstMainCamera,  sizeof(bool), 1, _File);
    fread(&m_bIsUICamera,         sizeof(bool), 1, _File);
    
    // 강제로 z값 맞추기
}

void CCamera::LayerCheck(int _Idx)
{
    m_LayerCheck ^= (1 << _Idx);
}

bool CCamera::SetAsFirstMainCamera(bool _bAsMainCamera)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;

    if (!_bAsMainCamera)
    {
        m_bIsFirstMainCamera = false;
        return true;
    }
    
    // MainCamera로 세팅하려는 상황
    // 둘 중 하나의 상태로만 처리가 가능하다
    m_bIsUICamera        = false;
    m_bIsFirstMainCamera = true;
    
    return true;
}

bool CCamera::SetAsUICamera(bool _bAsUICamera)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;

    if (!_bAsUICamera)
    {
        m_bIsUICamera = false;
        if (RenderMgr::GetInst()->GetUICamera() == this)
            RenderMgr::GetInst()->RegisterUICamera(nullptr); // 현재 UICamera가 이 객체였다면, UICamera 지우기
        return true;
    }
    
    // 둘 중 하나의 상태로만 처리가 가능하다
    m_bIsUICamera        = true;
    m_bIsFirstMainCamera = false;
    
    // MainCamera와 다르게, Stop 상황에서도 UI 카메라를 볼 수 있어야 UI 요소를 편집할 수 있기 때문에 UI 카메라 교체 시, 렌더매니저에도 등록을 바로 해준다.
    RenderMgr::GetInst()->RegisterUICamera(this);
    
    return true;
}

void CCamera::Init()
{
    // MainCamera 등록은 LevelBegin 때에 처리
    // UICamera는 어캄? -> 글쎄다
    
    // Clipping Plane -> 1 ~ 10000
    // -2500 ~ 2500 -> 맵 z 범위 (또는 y 범위)
    // -5000 ~ 5000
}

void CCamera::Begin()
{
    // UI 카메라와 MainCamera RenderMgr에 새로이 등록 처리
    if (m_bIsFirstMainCamera) RenderMgr::GetInst()->RegisterMainCamera(this);
    if (m_bIsUICamera)        RenderMgr::GetInst()->RegisterUICamera(this);
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

    // 2D Frustum Culling용 ViewRect 계산
    if (m_Type == PROJ_TYPE::ORTHOGRAPHIC)
    {
        Vec2 vPos = ToVec2(Transform()->GetWorldPos());
        float fWidth = m_Width * m_OrthoScale;
        float fHeight = (m_Width / m_AspectRatio) * m_OrthoScale;

        m_ViewRectMin = Vec2(vPos.x - fWidth * 0.5f, vPos.y - fHeight * 0.5f);
        m_ViewRectMax = Vec2(vPos.x + fWidth * 0.5f, vPos.y + fHeight * 0.5f);
    }
}

/*void CCamera::SortObject()
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
            // 오브젝트가 렌더링을 할 수 있는 상태인지 확인
            if (!object->GetRenderCom() ||
                !object->GetRenderCom()->GetMesh() ||
                !object->GetRenderCom()->GetMaterial())
                continue;
            
            RENDER_DOMAIN Domain = object->GetRenderCom()->GetMaterial()->GetDomain();

            m_mapDomainGameObject[Domain].push_back(object.Get());
        }
    }
}*/

void CCamera::Render(bool _bUseRenderDomainSort)
{
    g_Trans.matView = m_matView;
    g_Trans.matProj = m_matProj;

    if (!_bUseRenderDomainSort)
    {
        Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
        if (!pCurLevel) return;

        FlipbookRenderInstancing::BeginInstancing();
        BillboardRenderInstancing::BeginInstancing();
        SpriteRenderInstancing::BeginInstancing();

        for (UINT i = 0; i < MAX_LAYER; ++i)
        {
            if (false == (m_LayerCheck & (1 << i))) continue;

            // 레이어에 소속된 모든 오브젝트를 가져온다
            Layer* pLayer = pCurLevel->GetLayer(i);
            const vector<Ptr<GameObject>>& vecObjects = pLayer->GetAllObjects();

            COMPONENT_TYPE ComType = COMPONENT_TYPE::END;
            for (const Ptr<GameObject>& object : vecObjects)
            {
                // 오브젝틀가 렌더링을 할 수 있는 상태인지 확인
                if (!object->GetRenderCom() || !object->GetRenderCom()->GetMesh() || !object->GetRenderCom()->GetMaterial())
                    continue;

                // 2D Frustum Culling
                if (m_Type == PROJ_TYPE::ORTHOGRAPHIC && !object->GetRenderCom()->IsInViewRect(m_ViewRectMin, m_ViewRectMax))
                    continue;

                object->Render();

                /*if (object->GetRenderCom()->GetMaterial()->GetDomain() == RENDER_DOMAIN::DOMAIN_TRANSPARENT)
                {
                    CFlipbookRender::FlushInstancing();
                    CBillboardRender::FlushInstancing();
                    CSpriteRender::FlushInstancing();
                }*/
            }
        }
        
        FlipbookRenderInstancing::FlushInstancing();
        BillboardRenderInstancing::FlushInstancing();
        SpriteRenderInstancing::FlushInstancing();
        return;
    }

    // SortObject();
    FlipbookRenderInstancing::BeginInstancing();
    BillboardRenderInstancing::BeginInstancing();
    SpriteRenderInstancing::BeginInstancing();

    // Domain 순서대로 렌더링 진행
    for (const auto& Pair : RenderMgr::GetInst()->GetDomainGameObjects())
    {
        for (const Ptr<GameObject>& GameObject : Pair.second)
        {
            // 레이어 체크
            if (!(m_LayerCheck & (1 << GameObject->GetLayerIdx())))
                continue;

            // 2D Frustum Culling (Domain 순회 시에도 동일하게 적용)
            if (m_Type == PROJ_TYPE::ORTHOGRAPHIC && !GameObject->GetRenderCom()->IsInViewRect(m_ViewRectMin, m_ViewRectMax))
                continue;

            GameObject->Render();
        }

        /*if (Pair.first == RENDER_DOMAIN::DOMAIN_TRANSPARENT)
        {
            CFlipbookRender::FlushInstancing();
            CBillboardRender::FlushInstancing();
            CSpriteRender::FlushInstancing();
        }

        if (Pair.first == RENDER_DOMAIN::DOMAIN_TRANSPARENT_EFFECT)
        {
            CFlipbookRender::FlushInstancing();
            CBillboardRender::FlushInstancing();
            CSpriteRender::FlushInstancing();
        }*/
    }

    FlipbookRenderInstancing::FlushInstancing();
    BillboardRenderInstancing::FlushInstancing();
    SpriteRenderInstancing::FlushInstancing();

}

void CCamera::RenderGameUI()
{
    g_Trans.matView = m_matView;
    g_Trans.matProj = m_matProj;
    
    Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
    if (!pCurLevel) return;

    FlipbookRenderInstancing::BeginInstancing();
    BillboardRenderInstancing::BeginInstancing();
    SpriteRenderInstancing::BeginInstancing();

    
    // UI 레이어에 소속된 모든 오브젝트를 가져온다
    Layer* pLayer = pCurLevel->GetLayer(MAX_LAYER - 1);
    const vector<Ptr<GameObject>>& vecObjects = pLayer->GetAllObjects();

    COMPONENT_TYPE ComType = COMPONENT_TYPE::END;
    for (const Ptr<GameObject>& object : vecObjects)
    {
        // Text Object 인 경우
        if (object->GetIsTextObject())
        {
            object->Render();
            continue;
        }

        // 일반적인 Renderer Rendering 처리 상황 
        
        // 오브젝트가 렌더링을 할 수 있는 상태인지 확인
        if (!object->GetRenderCom() || !object->GetRenderCom()->GetMesh() || !object->GetRenderCom()->GetMaterial()) continue;

        // 2D Frustum Culling
        if (m_Type == PROJ_TYPE::ORTHOGRAPHIC && !object->GetRenderCom()->IsInViewRect(m_ViewRectMin, m_ViewRectMax))
            continue;

        object->Render();

        /*if (object->GetRenderCom()->GetMaterial()->GetDomain() == RENDER_DOMAIN::DOMAIN_TRANSPARENT)
        {
            CFlipbookRender::FlushInstancing();
            CBillboardRender::FlushInstancing();
            CSpriteRender::FlushInstancing();
        }*/
    }
        
    FlipbookRenderInstancing::FlushInstancing();
    BillboardRenderInstancing::FlushInstancing();
    SpriteRenderInstancing::FlushInstancing();
    
}

/*void CCamera::Render()
{
    g_Trans.matView = m_matView;
    g_Trans.matProj = m_matProj;

    CFlipbookRender::BeginInstancing();
    
    // Domain 순서대로 렌더링 진행
    for (const auto& Pair : m_mapDomainGameObject)
    {
        for (const Ptr<GameObject>& GameObject : Pair.second)
            GameObject->Render();
        
        if (Pair.first == RENDER_DOMAIN::DOMAIN_TRANSPARENT)
            CFlipbookRender::FlushInstancing();
    }
    
    CFlipbookRender::FlushInstancing();
}*/
