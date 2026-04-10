#include "pch.h"
#include "LevelMgr.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/08.CollisionMgr/CollisionMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/11.CollisionMatrixUI/CollisionMatrixUI.h"
#include "GameEngine/07.EditorUI/12.DebugLogUI/DebugLogUI.h"
#include "Header/components.h"
#include "Source/Manager/GameManager.h"

LevelMgr::LevelMgr()
    : m_LevelState(LEVEL_STATE::STOP)
{
}

LevelMgr::~LevelMgr()
{
}

void LevelMgr::Init()
{
    switch (m_LevelState)
    {
    case LEVEL_STATE::PLAY: DebugUtil::SetPermanentDebugLog("LevelState", "LEVEL_STATE : PLAY", Vec4(1.f, 0.f, 0.f, 1.f));
        break;
    case LEVEL_STATE::PAUSE: DebugUtil::SetPermanentDebugLog("LevelState", "LEVEL_STATE : PAUSE", Vec4(1.f, 0.f, 0.f, 1.f));
        break;
    case LEVEL_STATE::STOP: DebugUtil::SetPermanentDebugLog("LevelState", "LEVEL_STATE : STOP", Vec4(1.f, 0.f, 0.f, 1.f));
        break;
    }
}

void LevelMgr::Progress()
{
    // 실행할 레벨이 지정된게 없으면 return
    if (!m_CurLevel) return;
    
    // 이전에 등록된 모든 오브젝트들 제거
    m_CurLevel->Deregister();

    // 레벨의 상태가 Play 일 때만 Level의 Tick을 수행
    if (m_LevelState == LEVEL_STATE::PLAY)
        m_CurLevel->Tick(); // 레벨안에 있는 오브젝트들이 이번 DT 동안 할 일 수행
    
    m_CurLevel->FinalTick();

    // 충돌 검사 진행 -> TODO : Level이 각자의 CollisionMgr을 들고 있는 형태로 가져가면 어떨까
    if (m_LevelState == LEVEL_STATE::PLAY)
        CollisionMgr::GetInst()->Progress(m_CurLevel);
}

void LevelMgr::ChangeCurLevelState(LEVEL_STATE _NextState)
{
    if (m_LevelState == _NextState) return; // 이미 해당 State
    
    LEVEL_STATE PrevState = m_LevelState;
    m_LevelState = _NextState;

    // Stop -> Play
    if (PrevState == LEVEL_STATE::STOP && _NextState == LEVEL_STATE::PLAY)
    {
        CollisionMgr::GetInst()->OnLevelStopToPlay();
        
        // 원본 Asset 레벨의 복제본 레벨을 만들어서 현재 레벨로 가리킨다.
        m_CurLevel = m_SharedLevel->Clone();
        m_CurLevel->SetChanged();
        m_CurLevel->Begin();
        m_CurLevel->AfterLevelBegin();
    }

    // Play, Pause -> Stop
    else if (   (PrevState == LEVEL_STATE::PLAY || PrevState == LEVEL_STATE::PAUSE) && _NextState == LEVEL_STATE::STOP   )
    {
        // 원본 Asset 
        m_CurLevel = m_SharedLevel;
        m_CurLevel->SetChanged();
        
        // 모든 재생중이었던 Sound들 멈추기
        AssetMgr::GetInst()->StopAllSounds();
        
        GM->OnLevelPlayToStop();
        RenderMgr::GetInst()->OnLevelPlayToStop();
        AssetMgr::GetInst()->OnLevelPlayToStop();
        CollisionMgr::GetInst()->OnLevelPlayToStop();
    }

    
    switch (m_LevelState)
    {
    case LEVEL_STATE::PLAY: DebugUtil::SetPermanentDebugLog("LevelState", "LEVEL_STATE : PLAY", Vec4(1.f, 0.f, 0.f, 1.f));
        break;
    case LEVEL_STATE::PAUSE: DebugUtil::SetPermanentDebugLog("LevelState", "LEVEL_STATE : PAUSE", Vec4(1.f, 0.f, 0.f, 1.f));
        break;
    case LEVEL_STATE::STOP: DebugUtil::SetPermanentDebugLog("LevelState", "LEVEL_STATE : STOP", Vec4(1.f, 0.f, 0.f, 1.f));
        break;
    }
}

void LevelMgr::ChangeCurLevel(const Ptr<ALevel>& _NextLevel, bool _ChangeNextLevelStateToStop)
{
    GM->OnLevelChanged(m_SharedLevel.Get(), _NextLevel.Get());
    RenderMgr::GetInst()->OnLevelChanged(m_SharedLevel.Get(), _NextLevel.Get());
    AssetMgr::GetInst()->OnLevelChanged(m_SharedLevel.Get(), _NextLevel.Get());
    CollisionMgr::GetInst()->OnLevelChanged(m_SharedLevel.Get(), _NextLevel.Get());
    
    m_CurLevel = m_SharedLevel = _NextLevel;
    
    const wstring& FileName = GetFileNameWithoutExtension(m_CurLevel->GetKey());
    const string LevelNameStr = string(FileName.begin(), FileName.end());
    
    DebugUtil::SetPermanentDebugLog("CurrentLevel", "CURRENT LEVEL : " + LevelNameStr, Vec4(1.f, 0.f, 0.f, 1.f));

    if (_ChangeNextLevelStateToStop) // Editor에서 ChangeLevel 처리는 기본적으로 다음 LevelState를 Stop으로 처리 -> User Client의 Level 바꾸는건 이전 State를 계속해서 사용(Play)
        m_LevelState = LEVEL_STATE::STOP;
    else
    {
        ChangeLevelState(LEVEL_STATE::STOP);
        ChangeLevelState(LEVEL_STATE::PLAY);
    }
    
    EditorUI* pUI = EditorMgr::GetInst()->GetEditorUI("CollisionMatrixUI").Get();
    if (CollisionMatrixUI* MatUI = dynamic_cast<CollisionMatrixUI*>(pUI))
        MatUI->RefreshFromLevel();
    
    _NextLevel->SetChanged();
}
