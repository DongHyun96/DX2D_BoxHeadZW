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
    
public:

    CInvenScript();
    virtual ~CInvenScript() override;
    
    CLONE(CInvenScript)
    
    
public:

    virtual void Begin() override;
    
    virtual void Tick() override;

public:
    /// <summary>
    /// 해당 Structure을 소유하고 있는 중인지
    /// </summary>
    /// <param name="_StructureType"></param>
    /// <returns></returns>
    bool HasStructure(PLAYER_STRUCTURE_TYPE _StructureType) const { return m_mapStructureCount.at(_StructureType) > 0; }

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
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {};
    virtual void LoadFromLevelFile(FILE* _File) override {};
};
