#pragma once

class CColliderCircle : public CCollider2D
{

    friend class CColliderRect;
    friend class CColliderPoint;
    
private:

    float   m_Radius = 100.f;

public:
    
    CColliderCircle();
    virtual ~CColliderCircle() override;

    CLONE(CColliderCircle)
    
public:
    
    GET_SET(float, Radius)
    
    virtual void FinalTick() override;
    
private:
    
    virtual bool IsCollision(CColliderRect* _OtherRect) override;

    
    bool IsCollision(const Vec3& _Point);
    
    virtual bool IsCollision(CColliderPoint* _OtherPoint) override;
    
    virtual bool IsCollision(CColliderCircle* _OtherCircle) override;
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
