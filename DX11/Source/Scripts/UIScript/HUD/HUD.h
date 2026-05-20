#pragma once

struct AmmoCountUIArea : public Entity
{
    class CText*                            MainAmmoCount{};
    CText*                                  GrenadeCount{};
    CText*                                  AirStrikeCount{};

    map<PLAYER_HANDSTATE, GameObject*>      WeaponIcons{}; 
    GameObject*                             RoundAmmoIcon{};
    
    map<PLAYER_STRUCTURE_TYPE, GameObject*> StructureIcons{};
    
    class CProgressBar*                     HPBar{};
    
public:

    void UpdateToGun(PLAYER_HANDSTATE _HandState, int _MainAmmoCount);
    void UpdateCurrentAmmoCount(int _AmmoCount);
    void UpdateToStructure(PLAYER_STRUCTURE_TYPE _Type, int _Count);
};

struct RoundIndicators : public Entity
{
    CText* RoundText{}; // Round 1 Round 2
    CText* RoundWaitText{}; // Waiting for next round
    CText* RoundWaitTimeText{}; // time text
    
    class CUIAnimationGroup* RoundIndicatorAnimGroup{}; // Round Anim Group

public:
    
    /// <summary>
    /// RoundHandler에서 Round 변화 시, Callback 받는 함수 -> RoundState에 맞는 UI 처리 
    /// </summary>
    void OnRoundStateChanged(ROUND_STATE _NextRoundState);
    
    void SetRoundWaitingDisplaySec(float RoundWaitTime);
    
};