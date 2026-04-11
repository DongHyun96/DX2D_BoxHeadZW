#pragma once

class CFirstSpawnLocManager : public CScript
{
private:

private:
    
    map<FIRST_SPAWN_LOC, CTransform*> m_mapFirstSpawnAreaTransform{};
    
public:
    CFirstSpawnLocManager();
    virtual ~CFirstSpawnLocManager() override;
    CLONE(CFirstSpawnLocManager);

public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    CTransform* GetFirstSpawnAreaTransform(FIRST_SPAWN_LOC _Loc) const { return m_mapFirstSpawnAreaTransform.at(_Loc); }
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
};
