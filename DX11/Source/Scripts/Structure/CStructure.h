#pragma once

/// <summary>
/// TODO : Turret 종류의 클래스도 만들어서, 대신 Attack 부분만 전략패턴 써서 구현하면 좋지 않을까?
/// </summary>
class CStructure : public CScript
{
private:
    
    bool m_IsPreviewObject{}; // 설치할 위치 Preview Object 보여주기용인지
    
public:
    
    CStructure();
    virtual ~CStructure() override;
    CLONE(CStructure)
    
protected:
    
    CStructure(SCRIPT_TYPE _ScriptType);
    
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
