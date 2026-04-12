#pragma once

// #include "GameEngine/06.Component/Script/CScript.h"
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

enum class PLAYER_MAINSTATE;

/// <summary>
/// Basic Movement 및 MainState Holder 담당
/// </summary>
class CPlayerScript : public CCharacterScript
{
private:
    
    Vec2                m_PlayerToMousePos{};

private:    
    
    PLAYER_MAINSTATE    m_PlayerMainState{};    // Player MainState
    
private:
    
    class CPlayerStat* m_PlayerStat{};

public:
    
    CPlayerScript();
    virtual ~CPlayerScript() override;
    CLONE(CPlayerScript)
    
public:

    virtual void Init()             override;
    virtual void Begin()            override;
    virtual void AfterLevelBegin()  override;
    virtual void Tick()             override;
    
private:
    virtual void Move() override;
    virtual void UpdateCurrentFacedDirection() override;
    virtual void AfterPushedOutFin() override;
    
public:
    
    void SetMainState(PLAYER_MAINSTATE _MainState) { m_PlayerMainState = _MainState; }
    PLAYER_MAINSTATE GetMainState() const { return m_PlayerMainState; }
    
    PLAYER_HANDSTATE GetHandState() const;
    
    const Vec2& GetPlayerToMousePos() const { return m_PlayerToMousePos; }

private: // Enemy와 Player BodyCollider와의 충돌 처리 Handling

    // BeginOverlap과 Overlap 모두 이 함수로 들어옴
    void BodyColliderOverlapped(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
