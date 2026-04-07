#pragma once
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"


class CStructure : public CScript
{
private:
    
    // 맵에 현재 설치된 Structure들의 위치정보
    // Enemy의 Target setting을 할 때에, 사용을 할 수 있도록 한다
    static set<CellCoord> s_InstalledCoord;
    
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
    
    static void ClearInstalledCoordInfo() { s_InstalledCoord.clear(); }
    static void AddInstalledCoord(const CellCoord& _CellCoord) { s_InstalledCoord.insert(_CellCoord); }
    static void RemoveInstalledCoord(const CellCoord& _CellCoord) { s_InstalledCoord.erase(_CellCoord); }
    
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
