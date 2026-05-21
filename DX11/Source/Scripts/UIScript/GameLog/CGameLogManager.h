#pragma once

class CGameLogManager : public CScript
{
private:

    vector<GameObjectRefHolder>     m_vecGameLogs{};
    vector<float>                   m_vecGameLogEachYPositions{};   // GameLog 각 위치의 초기 Y Position 값
    vector<int>                     m_vecGameLogSequence{};         // 현재 Log들의 순서 (차례로 밑에서부터 위로)
    
public:
    
    CGameLogManager();
    virtual ~CGameLogManager() override;

public:

    virtual void Init() override;
    virtual void Tick() override;
    
        
    
    CLONE(CGameLogManager);
    
public:
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
