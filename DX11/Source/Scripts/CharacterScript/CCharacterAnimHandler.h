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

    void RewindPushedOut(const Vec2& _PushedOutFaceDirection)
    {
        m_PrevAnimDirection = EDIRECTION::END; // 추후, Walk또는 Idle 상태로 전환 시, Transition 처리가 제대로 이루어지려면, 상태를 돌려놔야 함
        m_PushedOutSpriteIdxToShow = 0;

        switch (GetEightDirection(_PushedOutFaceDirection))
        {
        case EDIRECTION::UP:            m_PushedOutSpriteIdxToShow = GetRandom(0, 1) ? 3 : 4; break; // 얘네는 명확히 떨어지는 Sprite 방향이 없어서 Random하게 비슷한 방향 Sprite에서 뽑음
        case EDIRECTION::LEFT:          m_PushedOutSpriteIdxToShow = GetRandom(0, 1) ? 2 : 3; break;
        case EDIRECTION::DOWN:          m_PushedOutSpriteIdxToShow = GetRandom(0, 1) ? 1 : 2; break;
            
        case EDIRECTION::RIGHT:         m_PushedOutSpriteIdxToShow = 0; break;
        case EDIRECTION::RIGHT_UP:      m_PushedOutSpriteIdxToShow = 4; break;
        case EDIRECTION::UP_LEFT:       m_PushedOutSpriteIdxToShow = 3; break;
        case EDIRECTION::LEFT_DOWN:     m_PushedOutSpriteIdxToShow = 2; break;
        case EDIRECTION::DOWN_RIGHT:    m_PushedOutSpriteIdxToShow = 1; break;
        case EDIRECTION::END: break;
        }
    }
    
    // TODO : Die 처리 Flipbook 고르는 함수도 pure-virtual 함수로 두기
    
};
