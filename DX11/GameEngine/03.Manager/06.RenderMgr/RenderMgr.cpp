#include "pch.h"
#include "RenderMgr.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "Source/Scripts/UIScript/CText.h"

RenderMgr::RenderMgr()
{
}

RenderMgr::~RenderMgr()
{
}

void RenderMgr::Init()
{
    // DebugRender 전용 object 초기화
    m_DbgObj = new GameObject;
    m_DbgObj->AddComponent(new CTransform);
    m_DbgObj->AddComponent(new CMeshRender);
    m_DbgObj->MeshRender()->SetMaterial(FIND_ASSET(AMaterial, L"DbgMtrl"));
    
    m_Light2DBuffer = new StructuredBuffer;
}

void RenderMgr::Progress()
{
    if (KEY_TAP(KEY::F9)) m_bDebugRender = !m_bDebugRender;
    
    // 렌더링 시작전에 할 일 처리
    Render_Start();

    LEVEL_STATE CurrentLevelState = LevelMgr::GetInst()->GetLevelState();
    
    // Level의 상태가 Play 상태면, 등록된 MainCam으로 렌더링
    if (CurrentLevelState == LEVEL_STATE::PLAY)
    {
        // 카메라 기반 렌더링
        // 카메라가 세팅되어있지 않다면 rendering 불가
        if (m_MainCam.Get())
        {
            // 카메라를 이용해서 Render 처리
            // m_MainCam->SortObject(); // RenderDomain 별 정렬
            m_MainCam->Render(true);
        }
    }
    else // Level의 상태가 Pause, Stop, 상태면 등록된 EditorCam으로 렌더링
    {
        // 카메라 기반 렌더링
        // 카메라가 세팅되어있지 않다면 rendering 불가
        if (!m_EditorCam) return;

        // 카메라를 이용해서 Render 처리
        // m_EditorCam->SortObject(); // RenderDomain 별 정렬
        m_EditorCam->Render(false);
    }
    
    // UI 카메라 나중에 렌더링 (UI가 월드 위에 그려지도록) -> Play, Stop 상태 관련 없이 항상 존재하면 렌더링 처리 (Stop 상태에서 UI 편집을 해야하기 때문)
    if (m_UICam.Get() && m_UICam->GetOwner()->GetActive())
    {
        // m_UICam->SortObject();
        m_UICam->RenderGameUI();
    }
    
    // 디버그 렌더링 요청 처리
    if (m_bDebugRender) Render_Debug();
    
    Render_End();
}

void RenderMgr::OnLevelBegin()
{
}

void RenderMgr::OnLevelPlayToStop()
{
    m_mapDomainGameObject.clear();
    
    // UICam 다시 찾아서 세팅하기 (이미 다시 SharedLevel로 돌아온 상황)
    RegisterUICamera(LevelMgr::GetInst()->GetCurLevel()->GetUICamera());
    GameObject* UICamObject = LevelMgr::GetInst()->GetCurLevel()->FindObjectByName(L"UICamera").Get();
    if (UICamObject) m_UICam = UICamObject->Camera();
    else m_UICam = nullptr;
    
}

void RenderMgr::OnLevelChanged(ALevel* _PrevLevel, ALevel* _NextLevel)
{
    m_mapDomainGameObject.clear();
    
    // UICam 다시 찾아서 세팅하기 
    GameObject* UICamObject = LevelMgr::GetInst()->GetCurLevel()->FindObjectByName(L"UICamera").Get();
    if (UICamObject) m_UICam = UICamObject->Camera();
    else m_UICam = nullptr;
}

void RenderMgr::Render_Start()
{
    // 타겟 설정
    Device::GetInst()->OMSetTarget();
    
    // 렌더타겟 클리어
    Device::GetInst()->ClearTarget();
    
    // Sampler 재 바인딩(ImGui 쪽에서 변경가능성)
    Device::GetInst()->BindingSampler();
    
    // 등록받은 Light2D의 광원 정보를 구조화 버퍼에 담는다.
    // 구조화버퍼를 특정 t 레지스터에 바인딩 한다.
    
    vector<Light2DInfo> vecInfo{};
    for (const Ptr<CLight2D>& lightComponent : m_vecLight2D)
        vecInfo.push_back(lightComponent->GetInfo());
    
    if (!vecInfo.empty())
    {
        // 구조화버퍼 공간이 모자라면 재확장 및 데이터 전달
        if (vecInfo.size() > m_Light2DBuffer->GetElementCount())
            m_Light2DBuffer->Create(sizeof(Light2DInfo), vecInfo.size(), SB_TYPE::SRV_ONLY, true, vecInfo.data());
        
        else // 공간이 여유가 있으면 바로 광원데이터 전달
            m_Light2DBuffer->SetData(vecInfo.data(), sizeof(Light2DInfo) * vecInfo.size());
        
        // t12 레지스터로 바인딩
        m_Light2DBuffer->Binding(12);
    }
    
    g_Global.Light2DCount = m_vecLight2D.size();

    // Global 데이터를 상수버퍼를 통해서 b2 레지스터에 바인딩
    Device::GetInst()->GetCB(CB_TYPE::GLOBAL)->SetData(&g_Global);
    Device::GetInst()->GetCB(CB_TYPE::GLOBAL)->Binding();
    Device::GetInst()->GetCB(CB_TYPE::GLOBAL)->Binding_CS();
}

void RenderMgr::Render_End()
{
    // 구조화버퍼 Clear
    // 등록받았던 광원들 해제
    m_Light2DBuffer->Clear();
    m_vecLight2D.clear();
}

void RenderMgr::Render_Debug()
{
    // POV 카메라의 행렬을 기본으로 세팅해준다.
    Ptr<CCamera> pPOVCam = GetPOVCam();
    if (pPOVCam.Get())
    {
        g_Trans.matView = pPOVCam->GetViewMat();
        g_Trans.matProj = pPOVCam->GetProjMat();
    }

    list<DebugInfo>::iterator iter = m_DbgInfoList.begin();
    while (iter != m_DbgInfoList.end())
    {
        DebugInfo& info = *iter;

        // UICamera를 사용해야 하는 경우 처리
        if (info.UseUICamera && m_UICam.Get())
        {
            g_Trans.matView = m_UICam->GetViewMat();
            g_Trans.matProj = m_UICam->GetProjMat();
        }
        else if (pPOVCam.Get())
        {
            // 루프 내에서 다시 POVCam으로 돌려놓아야 함 (다른 디버그 정보들이 섞여있을 수 있으므로)
            g_Trans.matView = pPOVCam->GetViewMat();
            g_Trans.matProj = pPOVCam->GetProjMat();
        }

        wstring MeshName{};
        
        switch (info.Shape)
        {
        case DEBUG_SHAPE::RECT:   MeshName = L"RectMesh_LineStrip";   break; 
        case DEBUG_SHAPE::CIRCLE: MeshName = L"CircleMesh_LineStrip"; break;
        case DEBUG_SHAPE::LINE:   MeshName = L"LineMesh_LineStrip";   break;
        case DEBUG_SHAPE::CUBE:   MeshName = L"CubeMesh";             break;
        case DEBUG_SHAPE::SPHERE: MeshName = L"SphereMesh";           break;
        default: break;
        }

        m_DbgObj->MeshRender()->SetMesh(FIND_ASSET(AMesh, MeshName));

        if (info.matWorld == XMMatrixIdentity())
        {
            m_DbgObj->Transform()->SetRelativePos(info.Pos);
            m_DbgObj->Transform()->SetRelativeScale(info.Scale);
            m_DbgObj->Transform()->SetRelativeRot(info.Rotation);
            m_DbgObj->Transform()->FinalTick(); // World 행렬 setting 처리
        }
        else m_DbgObj->Transform()->SetWorldMatrix(info.matWorld);

        m_DbgObj->MeshRender()->GetMaterial()->SetScalar(SCALAR_PARAM::VEC4_0, info.Color);

        const DS_TYPE DepthStencilType = (info.EnableDepthTest) ? DS_TYPE::LESS : DS_TYPE::NO_TEST_NO_WRITE;
        m_DbgObj->MeshRender()->GetMaterial()->GetShader()->SetDSType(DepthStencilType);
        
        m_DbgObj->Render(); // Render 요청

        info.Age += E_DT;
        
        if (info.Age > info.Life) iter = m_DbgInfoList.erase(iter);
        else ++iter;
    }
}

Ptr<CCamera> RenderMgr::GetPOVCam() const
{
    return LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP ? m_EditorCam : m_MainCam;
}
