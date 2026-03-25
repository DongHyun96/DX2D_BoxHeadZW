#pragma once

class CObstacle : public CScript
{
private:
    
public:
    
    CObstacle();
    virtual ~CObstacle() override;

    CLONE(CObstacle)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
private:
    
    void BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void EndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
};
