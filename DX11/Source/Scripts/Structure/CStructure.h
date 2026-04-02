#pragma once

class CStructure : public CScript
{
private:
    
    bool m_IsPreviewObject{}; // 설치할 위치 Preview Object 보여주기용인지
    
public:
    
    CStructure();
    virtual ~CStructure() override;
    CLONE(CStructure)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    virtual void BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
    bool BlockCharacterCollider(CCollider2D* _OtherCollider);
    
public:
    
    void SetIsPreviewObject(bool isPreviewObject) { m_IsPreviewObject = isPreviewObject; }
    bool GetIsPreviewObject() const { return m_IsPreviewObject; }
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
};
