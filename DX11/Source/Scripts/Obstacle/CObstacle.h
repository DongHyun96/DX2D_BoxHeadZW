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
    // void EndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
private:
    
    /// <summary>
    /// 캐릭터 Collider의 경우에 Blocking 처리  
    /// </summary>
    /// <returns> : 입력으로 들어온 Collider의 주인이 Character가 아니라면 return false </returns>
    bool BlockCharacterCollider(CCollider2D* _OtherCollider);
    
};
