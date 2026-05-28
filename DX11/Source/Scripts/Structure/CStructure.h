#pragma once
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"


class CStructure : public CScript
{
private:
    
    // 맵에 현재 설치된 Structure들을 저장
    // Enemy의 Target setting을 할 때에, 사용을 할 수 있도록 한다
    // static set<CStructure*> s_setInstalledStructures;
    static RandomizedSet<CStructure*> s_setInstalledStructures;
    
    // 맵에 설치된 설치물들의 위치 기록
    // 사용자에 의한 Remove 처리 시, 필요
    static map<Vec2, CStructure*> s_mapInstalledStructureLocation;
    
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
    static void AddInstalledStructure(CStructure* _Structure);
    
protected:
    
    static void RemoveInstalledStructure(CStructure* _Structure);
    
public:
    
    static const RandomizedSet<CStructure*>& GetInstalledStructures() { return s_setInstalledStructures; }
    static CStructure* GetInstalledStructure(const Vec2& _Pos);

public:
    
    /// <summary>
    /// 이 구조물 파괴 처리 
    /// </summary>
    /// <param name="_DestroyedByDamaged"> : Damage를 받아서 Destroy 처리가 되었는지 여부 </param>
    /// <returns> 파괴 성공 시, return true </returns>
    virtual bool DestroyStructure(bool _DestroyedByDamaged = true);
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
