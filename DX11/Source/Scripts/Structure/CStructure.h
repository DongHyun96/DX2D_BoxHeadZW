#pragma once
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"


class CStructure : public CScript
{
private:
    
    // 맵에 현재 설치된 Structure들을 저장
    // Enemy의 Target setting을 할 때에, 사용을 할 수 있도록 한다
    static set<CStructure*> s_setInstalledStructures;
    
private:

    Ptr<ASound> m_InstallSound{};

private:
    
    bool m_IsPreviewObject{}; // 설치할 위치 Preview Object 보여주기용인지
    int m_CharacterBodyOverlapCount{};
    
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

public:

    // 구조물 같은 경우 아예 CreateObject 처리로 새로 만드는 중이다
    // virtual void InitSpawn() {}
    
private:
    
    void BodyColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void BodyColliderOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void BodyColliderEndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);

private:
    
    bool BlockCharacterCollider(CCollider2D* _OtherCollider);
    
public:
    
    void SetIsPreviewObject(bool isPreviewObject) { m_IsPreviewObject = isPreviewObject; }
    bool GetIsPreviewObject() const { return m_IsPreviewObject; }
    
    bool IsCharacterBodyOverlapping() const { return m_CharacterBodyOverlapCount > 0; }
    
public:
    
    void PlayInstallSound() const
    {
        if (m_InstallSound) m_InstallSound->Play(1, 0.5f, true);
    }

public:
    
    static void ClearInstalledInfo() { s_setInstalledStructures.clear(); }
    static void AddInstalledStructure(CStructure* _Structure) { s_setInstalledStructures.insert(_Structure); }
    static void RemoveInstalledStructure(CStructure* Structure) { s_setInstalledStructures.erase(Structure); }
    static const set<CStructure*>& GetInstalledStructures() { return s_setInstalledStructures; }
    
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
