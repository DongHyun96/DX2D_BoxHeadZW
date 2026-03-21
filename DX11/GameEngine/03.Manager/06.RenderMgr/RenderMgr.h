#pragma once
#include "GameEngine/02.Device/StructuredBuffer/StructuredBuffer.h"
#include "Header/Single.h"
#include "Module/Ptr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/02.Camera/CCamera.h"
#include "GameEngine/06.Component/04.Light2D/CLight2D.h"


class RenderMgr : public Singleton<RenderMgr>
{

    SINGLE(RenderMgr);

private:

    Ptr<CCamera>            m_MainCam{};
    Ptr<CCamera>            m_EditorCam{};
    
    Ptr<GameObject>         m_DbgObj{};             // 디버그 렌더링을 위한 Dummy GameObject (Level안에 있는 GameObject가 아니다)
    list<DbgInfo>           m_DbgInfoList{};        // 디버그 요청 정보

    vector<Ptr<CLight2D>>   m_vecLight2D{};         // 레벨 안에있는 모든 광원
    Ptr<StructuredBuffer>   m_Light2DBuffer{};      // 광원의 데이터를 입력받을 구조화버퍼
    
    bool                    m_bDebugRender = true;  // 디버그 랜더 기능 on / off
    
public:
    
    void AddDebugInfo(const DbgInfo& _Info)
    {
        if (m_bDebugRender) m_DbgInfoList.push_back(_Info);
    }
    
public:

    void Init();
    void Progress();
    
private:
    
    void Render_Start();
    void Render_End();
    void Render_Debug();
    
public:
    
    void RegisterMainCamera(const Ptr<CCamera>& _Cam) { m_MainCam = _Cam; }
    void RegisterEditorCamera(const Ptr<CCamera>& _Cam) { m_EditorCam = _Cam; }
    
    Ptr<CCamera> GetPOVCam() const; // 현재 바라보고 있는 카메라 (Play 중인 경우 MainCam, Editing 중인 경우 EditorCam)
    Ptr<CCamera> GetEditorCam() const { return m_EditorCam; }
    
    void RegisterLight2D(const Ptr<CLight2D>& _Light2D) { m_vecLight2D.push_back(_Light2D); }
};
