#pragma once

class CPlayerAnimHandler : public CScript
{
private:

    class CPlayerScript* m_MainPlayerScript{};
    
    EDIRECTION  m_AnimDirection = EDIRECTION::DOWN;
    bool        m_WalkingBackward{};
    
    
private: /* Animation FSM에서(UpdateAction) transition 조건에 필요한 변수들 */
	
    // TODO : 이전 상태까지도 체크를 해서 만약 들어온 Velocity와 State 모두 이전 Tick과 같다면, Anim Play 처리를 한 번더 처리 하면 안됨
    
    EDIRECTION m_PrevAnimDirection = EDIRECTION::END;
    bool m_PrevWalkingBackward{};
    
public:

    CPlayerAnimHandler();
    virtual ~CPlayerAnimHandler() override;
    CLONE(CPlayerAnimHandler)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
    
private:
    
    void UpdateAnimDirection(const Vec2& _PlayerToMousePos);

    /// <summary>
    /// 바라보는 방향과 움직이는 방향을 비교해서 뒤로 걷는 중인지 판단
    /// </summary>
    void UpdateWalkingBackward(const Vec2& _PlayerToMousePos);
    
    
    void UpdateAnimTransition();
    
    
public:
    
    void SaveToLevelFile(FILE* _File) override {};
    void LoadFromLevelFile(FILE* _File) override {};
};
