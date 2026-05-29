#pragma once
#include "GameEngine/05.GameObject/GameObjectRefHolder.h"

/// <summary>
/// 구조물 설치 Handler 클래스
/// </summary>
class CStructureHandler : public CScript
{
private:
    
    const float PREVIEW_ALPHA = 0.6f;
    
private:

    PLAYER_STRUCTURE_TYPE m_CurrentStructureHolding{}; // 현재 설치하려는 물체 Type (HandState Default 상태에서 다른 설치물 설치 시도로 넘어갈 수 있다)
    
    map<PLAYER_STRUCTURE_TYPE, APrefab*>    m_mapStructureTypePrefabs{};
    map<PLAYER_STRUCTURE_TYPE, GameObject*> m_mapStructureTypePreviewObjects{};    
    
private:

    class CPlayerScript*    m_MainPlayerScript{};
    class CInvenScript*     m_InvenScript{};

private:
    
    static set<UINT> s_setTurretHitScanLayers;

private:
    
    GameObjectRefHolder m_ToolTipGORef{};
    class CStructureInstruction* m_ToolTip{};
    
public:
    
    CStructureHandler();
    virtual ~CStructureHandler() override;
    CLONE(CStructureHandler)

public:

    virtual void Init() override;
    virtual void AfterLevelGameObjectGuidTableInit() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    /// <summary>
    /// 현재 설치할 설치물 종류 Sawp 처리
    /// </summary>
    void UpdateCurrentStructureHolding();

    
    /// <summary>
    /// 다음 StructureType으로 전환 시도
    /// </summary>
    /// <returns> : 자기자신 Type으로 돌아오거나, </returns>
    void UpdateToNextStructureTypeHolding();
    void UpdateToPrevStructureTypeHolding();
    
private:
    
    /// <summary>
    /// 해당 Cell 위치에 Structure가 존재한다면, 우클릭을 통해 제거 처리 
    /// </summary>
    void UpdateRemoveStructure(const Vec2& _CurrentMouseCellPos);
    
    /// <summary>
    /// 현재 StructureHoldingType 실제로 Spawn 처리 시도
    /// </summary>
    void UpdateSpawnStructure(const Vec2& _PreviewPos, bool _Available);

    /// <summary>
    /// Preview 실물 오브젝트가 없다면, 오브젝트들 생성처리
    /// </summary>
    void CreateStructureHoldingPreviewIfNecessary();
    
private:
    
    /// <summary>
    /// Preview 오브젝트 업데이트 
    /// </summary>
    /// <param name="_PreviewPos"> : </param>
    /// <param name="_Available"></param>
    void UpdatePreviewStructureObject(const Vec2& _PreviewPos, bool _Available);
    
public:
    
    static const set<UINT>& GetTurretHitScanLayers() { return s_setTurretHitScanLayers; }
    
    /// <summary>
    /// 현재 StructureHolding에 맞게끔 UI 초기 세팅
    /// </summary>
    void UpdateUIToCurrentStructureHoldingType() const;
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
