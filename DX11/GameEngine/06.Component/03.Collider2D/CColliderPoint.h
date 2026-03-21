#pragma once

class CColliderPoint : public CCollider2D
{
public:
    
    CColliderPoint();
    virtual ~CColliderPoint() override;

    CLONE(CColliderPoint)
    
public:
    
    virtual void FinalTick() override;
    
private:
    
    virtual bool IsCollision(CColliderRect* _OtherRect) override;
    virtual bool IsCollision(CColliderPoint* _OtherPoint) override;
    virtual bool IsCollision(CColliderCircle* _OtherCircle) override;
    
    
    
};
