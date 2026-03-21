#pragma once
#include "GameEngine/06.Component/Script/CScript.h"

class CMonsterScript : public CScript
{
private:
    
    float m_Time{};
    
public:
    
    CMonsterScript();
    virtual ~CMonsterScript() override;
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    CLONE(CMonsterScript)
    
private:
    
    void BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void EndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
};
