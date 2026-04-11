#pragma once
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

class CMummy : public CEnemyScript
{
private:

    // 다른 Mummy가 사망하고, 현재 Mummy를 Spawn시켰는지 체크 (계속해서 Spawn 처리되면 안되기 때문)
    bool m_SpawnedByMummy{};
    
    class CStatScript* m_Stat{};
    
public:
    CMummy();
    virtual ~CMummy() override;
    CLONE(CMummy)
    
public:
    
    virtual void Begin() override;

private:

    virtual void OnFadeOutEnd() override;
    
public:
    
    void SetSpawnedByMummy(bool _SpawnedByMummy) { m_SpawnedByMummy = _SpawnedByMummy; }
    
};
