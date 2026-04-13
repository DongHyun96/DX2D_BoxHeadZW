#pragma once
#include "GameEngine/04.Asset/10.Sound/ASound.h"

class CItem : public CScript
{
private:
    float m_LifeTime    = 10.f;
    float m_MaxLifeTime = 10.f;

private:
    
    Ptr<ASound>     m_EarnedSound{};
    Ptr<AMaterial>  m_DynamicMtrl{};
    
public:
    
    CItem();
    virtual ~CItem() override;
    CLONE(CItem);

public:

    virtual void AfterLevelBegin() override;
    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    void OnColliderBeginOverlap(CCollider2D* _Owner, CCollider2D* _Other);
    
public:
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
