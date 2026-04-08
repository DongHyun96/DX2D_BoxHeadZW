#pragma once

class CCharacterAnimHandler : public CScript
{
protected: // PushedOut 관련

    int             m_PushedOutSpriteIdxToShow{};
    
protected:
        
    
protected:
    
    EDIRECTION m_PrevAnimDirection = EDIRECTION::END;
    
public:
    
    CCharacterAnimHandler(SCRIPT_TYPE _ScriptType);
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
    
public:

    void RewindPushedOut(const Vec2& _PushedOutFaceDirection);

protected:
    
    UINT GetDieFlipbookIdxBase() const;
    
    
};
