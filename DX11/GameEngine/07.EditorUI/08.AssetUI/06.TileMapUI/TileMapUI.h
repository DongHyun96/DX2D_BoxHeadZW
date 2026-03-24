#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"


class TileMapUI : public AssetUI
{
    enum class AtlasCreateMode
    {
        NEWNAME,
        REUSE
    };
    
    enum class PaintMode
    {
        BRUSH,
        RECT_FILL,
        BUCKET_FILL
    };

private:

    PaintMode m_PaintMode{};
    
    bool m_RectDragging{};
    int  m_RectStartRow = -1, m_RectStartCol = -1;
    int  m_RectCurRow   = -1, m_RectCurCol   = -1;
    
private:
    
    ATileMap* m_CachedTarget{};     // 타겟 변경 감지
    
    int m_Row = 10;
    int m_Col = 10;
    Vec2 m_CellSize = Vec2(32.f, 32.f); // UI 셀 사이즈 + TileSize 동기화 용
    
private:
    
    vector<Ptr<ASprite>> m_Palette{};
    int m_SelectedPalette = -1;

private:
    
    vector<Ptr<ASprite>> m_WorkingCells{}; // UI용 임시 그리드 (atlas 혼합 허용)
    int m_LastPaintIdx = -1;               // 드래그 연속 칠하기용

private:
    
    float m_Zoom = 1.f;
    float m_MinZoom = 0.1f;
    float m_MaxZoom = 4.f;
    bool  m_OpenDetachedCanvasWindow{};

private:
    
    bool    m_OpenAtlasNameModal = false;
    bool    m_PendingAtlasSave = false;

    string  m_AtlasNameInput = "TileMapAtlas";
    bool    m_AtlasNameConflict = false;
    string  m_AtlasNameConflictMsg;

    // 모달에서 확정될 때 쓰기 위한 스냅샷
    vector<Ptr<ASprite>>    m_PendingCells;
    int                     m_PendingRow = 0;
    int                     m_PendingCol = 0;
    Vec2                    m_PendingCellSize = Vec2::Zero;
    
private: // 타일 랜덤배치 관련
    
    float m_ScatterChancePercent = 15.f;   // 빈도수(%)
    bool  m_ScatterUseCountMode = false;   // false: 확률, true: 개수
    int   m_ScatterCount = 30;
    bool  m_ScatterUseFixedSeed = false;
    int   m_ScatterSeed = 0;

    unordered_set<ASprite*> m_ScatterReplaceAllow; // 교체 허용 대상(빈 칸은 항상 허용)
    
public:
    TileMapUI();
    virtual ~TileMapUI() override;
    
public:
    
    virtual void Tick_UI() override;
    
private:
    void SyncFromAsset(const Ptr<ATileMap>& tile);
    void ResizeGrid(int newRow, int newCol);
    void DrawPalette();
    void DrawTileCanvas();
    void DrawDetachedTileCanvasWindow();
    void PaintCell(int row, int col, const Ptr<ASprite>& sp);
    void EraseCell(int row, int col);
    void BakeAndSaveTileMap();

    // 공용 atlas 만들기용
    bool BuildSharedAtlasIfNeeded
    (
        const vector<Ptr<ASprite>>&             usedSprites,
        Ptr<ATexture>&                          outAtlas,
        unordered_map<ASprite*, Ptr<ASprite>>&  outRemap,
        const wstring&                          baseName,
        AtlasCreateMode                         mode,
        vector<wstring>&                        outSpriteKeys,
        string&                                 outErrMsg
    );
    
private:
    
    void DrawAtlasNameModal();
    
    void CleanupGeneratedAssets(ATileMap* tile);
    
private:
    
    void BucketFill(int startRow, int startCol, const Ptr<ASprite>& fillSprite);
    
private: // 타일 랜덤배치 관련
    
    void DrawScatterToolUI();
    bool CanScatterOn(const Ptr<ASprite>& current) const;
    void ScatterSelectedSprite(bool _bReplaceAll);
    
};
