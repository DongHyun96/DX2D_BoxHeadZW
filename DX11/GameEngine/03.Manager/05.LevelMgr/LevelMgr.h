#pragma once
#include "GameEngine/04.Asset/04.Level/ALevel.h"

class LevelMgr : public Singleton<LevelMgr>
{
    SINGLE(LevelMgr);

    friend class TaskMgr;
    
private:
    
    Ptr<ALevel> m_CurLevel{};
    Ptr<ALevel> m_SharedLevel{};
    LEVEL_STATE m_LevelState = LEVEL_STATE::STOP;

    
public:
    
    void Init();
    void Progress();
    
public:
    
    Ptr<ALevel> GetCurLevel() const { return m_CurLevel; }
    
    Ptr<GameObject> FindObjectByName(const wstring& _Name) const { return m_CurLevel->FindObjectByName(_Name); }
    
    LEVEL_STATE GetLevelState() const { return m_LevelState; }
    
private:
    
    void ChangeCurLevel(const Ptr<ALevel>& _NextLevel, bool _ChangeNextLevelStateToStop);
    void ChangeCurLevelState(LEVEL_STATE _NextState);
    
    
public:
    
    void CreateTestLevel();
    
};
