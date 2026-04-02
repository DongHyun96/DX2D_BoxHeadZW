#pragma once

/// <summary>
/// 구조물 설치 Handler 클래스
/// </summary>
class CStructureHandler : public CScript
{
private:
    
    map<PLAYER_STRUCTURE_TYPE, APrefab*>    m_mapStructureTypePrefabs{};
    map<PLAYER_STRUCTURE_TYPE, GameObject*> m_mapStructureTypePreviewObjects{};    
    
private:

    class CPlayerScript*    m_MainPlayerScript{};
    class CInvenScript*     m_InvenScript{};
    
public:
    
    CStructureHandler();
    virtual ~CStructureHandler() override;
    CLONE(CStructureHandler)

public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
