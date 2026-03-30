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
    PLAYER_HANDSTATE    m_HandState{};          // 현재 손에 들고 있는 무기 종류 State
    
public:
    
    CPlayerScript();
    virtual ~CPlayerScript() override;
    CLONE(CPlayerScript)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    virtual void Move() override;
    virtual void UpdateCurrentFacedDirection() override;
    virtual void AfterPushedOutFin() override;
    
public:
    
    void SetHandState(PLAYER_HANDSTATE _HandState) { m_HandState = _HandState; }
    PLAYER_HANDSTATE GetHandState() const { return m_HandState; }
    
    void SetMainState(PLAYER_MAINSTATE _MainState) { m_PlayerMainState = _MainState; }
    PLAYER_MAINSTATE GetMainState() const { return m_PlayerMainState; }
    
    const Vec2& GetPlayerToMousePos() const { return m_PlayerToMousePos; }
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
