#pragma once

class CCharacterAnimHandler : public CScript
{
private:

    const float   m_PushedOutTotalTime = 1.5f; // PushedOut 멈춰있는 모습 처리로 보여질 총 시간
    float         m_PushedOutTime{};
    
public:
    
    CCharacterAnimHandler(enum SCRIPT_TYPE _ScriptType);
    virtual ~CCharacterAnimHandler() override;
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    /// <summary>
    /// AnimTransition FSM 구현하기 (매 Tick마다 호출되는 함수) 
    /// </summary>
    virtual void UpdateAnimTransition() = 0;
    
};
