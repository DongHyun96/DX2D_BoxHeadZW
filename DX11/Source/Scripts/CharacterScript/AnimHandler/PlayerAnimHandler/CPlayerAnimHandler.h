#pragma once
#include "Source/Scripts/CharacterScript/AnimHandler/CCharacterAnimHandler.h"


class CPlayerAnimHandler : public CCharacterAnimHandler
{
private:

    class CPlayerScript*    m_MainPlayerScript{};
    bool                    m_WalkingBackward{};

    
private: /* Animation FSM에서(UpdateAction) transition 조건에 필요한 변수들 */
	
    // TODO : 이전 상태까지도 체크를 해서 만약 들어온 Velocity와 State 모두 이전 Tick과 같다면, Anim Play 처리를 한 번더 처리 하면 안됨

    PLAYER_MAINSTATE m_PrevMainState    = PLAYER_MAINSTATE::END;
    PLAYER_HANDSTATE m_PrevHandState    = PLAYER_HANDSTATE::END;
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
    
    /// <summary>
    /// 바라보는 방향과 움직이는 방향을 비교해서 뒤로 걷는 중인지 판단
    /// </summary>
    void UpdateWalkingBackward();
    virtual void UpdateAnimTransition() override;

    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
