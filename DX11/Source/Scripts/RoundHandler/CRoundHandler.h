#pragma once

class CRoundHandler : public CScript
{
private:

    ROUND_STATE m_RoundState{};
    
private:
    
    
    
public:
    
    CRoundHandler();
    virtual ~CRoundHandler() override;
    CLONE(CRoundHandler);
public:
    void Tick() override;

private:
    
    void HandleTransition();
    
public:
    
    void SetRoundState(ROUND_STATE _RoundState);
    ROUND_STATE GetRoundState() const { return m_RoundState; }
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
