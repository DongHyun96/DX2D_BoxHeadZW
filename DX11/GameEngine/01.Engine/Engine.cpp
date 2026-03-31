#include "pch.h"
#include "Engine.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/06.RenderMgr/RenderMgr.h"
#include "GameEngine/03.Manager/07.TaskMgr/TaskMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"


Engine::Engine()
    : m_hInst(nullptr)
    , m_hWnd(nullptr)
    , m_Resolution{}
{
}

Engine::~Engine()
{
    if (m_FMODSystem) m_FMODSystem->release();
}

HRESULT Engine::Progress()
{
    TimeMgr::GetInst()->Tick(); // DeltaTime 계산
    KeyMgr::GetInst()->Tick();  // Key상태 계산

    // LevelMgr 업데이트
    LevelMgr::GetInst()->Progress();
    
    // RenderMgr 렌더링
    RenderMgr::GetInst()->Progress();
    
    TimeMgr::GetInst()->Render();

    // EditorMgr
    if (m_EditorMode) EditorMgr::GetInst()->Progress();
    
    // 렌더타겟에 그려진 그림을, 윈도우 비트맵으로 복사
    Device::GetInst()->Present();
    
    if (m_FMODSystem) m_FMODSystem->update();
    
    // 다음 프레임에 적용될 작업 처리
    TaskMgr::GetInst()->Progress();
    
    return S_OK;
}
