#pragma once
#include "GameEngine/04.Asset/10.Sound/ASound.h"


class CStructure : public CScript
{
private:

    Ptr<ASound> m_InstallSound{};
    
    bool m_IsPreviewObject{}; // 설치할 위치 Preview Object 보여주기용인지
    
public:
    
    CStructure();
    CStructure(const CStructure& _Origin);
    virtual ~CStructure() override;
    CLONE(CStructure)
    
protected:
    
    CStructure(SCRIPT_TYPE _ScriptType);
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    virtual void BodyColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    virtual void BodyColliderOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
    bool BlockCharacterCollider(CCollider2D* _OtherCollider);
    
public:
    
    void SetIsPreviewObject(bool isPreviewObject) { m_IsPreviewObject = isPreviewObject; }
    bool GetIsPreviewObject() const { return m_IsPreviewObject; }
    
public:
    
    void PlayInstallSound() const
    {
        if (m_InstallSound) m_InstallSound->Play(1, 0.5f, true);
    }
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
