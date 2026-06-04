#pragma once

/// <summary>
/// Owned by Player object (탄알, 배낭, 힐 및 부스트 아이템, 
/// </summary>
class CInvenScript : public CScript
{
private:
    
    // 현재 소유중인 Structure 개수
    map<PLAYER_STRUCTURE_TYPE, int> m_mapStructureCount = 
    {
        {PLAYER_STRUCTURE_TYPE::BARRICADE,          0},    
        {PLAYER_STRUCTURE_TYPE::BARREL,             0},    
        {PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN, 0},    
        {PLAYER_STRUCTURE_TYPE::TURRET_MORTAR,      0},    
        {PLAYER_STRUCTURE_TYPE::TURRET_ROCKET,      0},    
    };

    // 현재 무기 남은 장탄수 (해금되지 않은 무기의 탄약도 소유하고 있을 수 있다)
    map<PLAYER_HANDSTATE, int> m_AmmoLeft = 
    {
        {PLAYER_HANDSTATE::UNARMED, 0}, // Dummy
        {PLAYER_HANDSTATE::PISTOL,  INT_MAX}, // Infinite    
        {PLAYER_HANDSTATE::UZI,     200},    
        {PLAYER_HANDSTATE::SHOTGUN, 80},    
        {PLAYER_HANDSTATE::MINIGUN, 400},    
        {PLAYER_HANDSTATE::ROCKET,  20},    
    };

private: // 수류탄 및 AirStrike 남은 개수
    
    int m_AirStrikeCount{};
    int m_GrenadeCount{};
    
public:

    CInvenScript();
    virtual ~CInvenScript() override;
    
    CLONE(CInvenScript)
    
    
public:

    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;
    

public:
    /// <summary>
    /// 해당 Structure을 소유하고 있는 중인지
    /// </summary>
    /// <param name="_StructureType"></param>
    /// <returns></returns>
    bool HasStructure(PLAYER_STRUCTURE_TYPE _StructureType) const { return m_mapStructureCount.at(_StructureType) > 0; }

    /// <summary>
    /// Structure가 한개라도 있는지 조사
    /// </summary>
    /// <returns></returns>
    bool HasAnyStructure() const;

    int GetCurrentStructureCount(PLAYER_STRUCTURE_TYPE _StructureType) const { return m_mapStructureCount.at(_StructureType); }

    /// <summary>
    /// 해당 StructureType 개수 하나 줄이기
    /// </summary>
    /// <returns> : 줄인 뒤 개수 </returns>
    int ReduceCurrentStructureCount(PLAYER_STRUCTURE_TYPE _StructureType, int _ReduceAmount = 1);

    /// <summary>
    /// 해당 StructureType 개수 하나 늘리기
    /// </summary>
    /// <returns> : 늘린 뒤 개수 </returns>
    int IncreaseCurrentStructureCount(PLAYER_STRUCTURE_TYPE _StructureType, int _IncreaseAmount = 1);
    
    int GetStructureCount(PLAYER_STRUCTURE_TYPE _StructureType) const;
    
    int GetCurrentAmmoCount(PLAYER_HANDSTATE _HandState) const { return m_AmmoLeft.at(_HandState); }
    void ReduceCurrentAmmoCount(PLAYER_HANDSTATE _HandState, int _ReduceAmount = 1);
    void IncreaseCurrentAmmoCount(PLAYER_HANDSTATE _HandState, int _IncreaseAmount = 1);

    int GetCurrentGrenadeCount() const { return m_GrenadeCount; }
    void ReduceCurrentGrenadeCount(int _ReduceAmount = 1);
    void IncreaseCurrentGrenadeCount(int _IncreaseAmount = 1);
    
    int GetCurrentAirStrikeCount() const { return m_AirStrikeCount; }
    void ReduceCurrentAirStrikeCount(int _ReduceAmount = 1);
    void IncreaseCurrentAirStrikeCount(int _IncreaseAmount = 1);
    
private:
    
    void SetCurrentGrenadeCount(int _GrenadeCount);
    void SetCurrentAirStrikeCount(int _AirStrikeCount);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {};
    virtual void LoadFromLevelFile(FILE* _File) override {};
};
