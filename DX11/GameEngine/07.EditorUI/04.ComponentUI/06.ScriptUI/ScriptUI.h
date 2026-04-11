#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class CBackgroundTile;

enum class TILE_EDIT_BRUSH
{
	NONE,
	CELL_TAKEN,
	CELL_EMPTY,
	SPAWN_ADD,
	SPAWN_REMOVE,
};

class ScriptUI : public ComponentUI
{
private:
    
    unordered_map<void*, vector<char>> m_WStringInputBuffer{};
    
private:
    
    Ptr<CScript>    m_TargetScript;
    UINT            m_ItemHeight;
    
private: // BackgroundTile TakenCell Editing 관련
    
    static bool             s_BackgroundTileCellEditingEnabled;
    static CBackgroundTile* s_BackgroundTileEditingTarget;
    static TILE_EDIT_BRUSH  s_BackgroundTileBrush;
    static FIRST_SPAWN_LOC  s_BackgroundTileSelectedSpawnLoc;

public:
    ScriptUI();
    virtual ~ScriptUI() override;

    
public:
    void SetScript(CScript* _Script);

public:
    virtual void Tick_UI() override;
    
private:
    
    void TickScriptParams();
    void TickBackgroundTileEditingUI();

private:

    void AddItemHeight();
    
    virtual void OnRemoveScriptConfirmed(bool _Confirmed) override;
    
    
    
    
    
    
    
    
public: // BackgroundTile TakenCell Editing 관련
    static bool             IsBackgroundTileCellEditingEnabled()                { return s_BackgroundTileCellEditingEnabled; }
    static void             SetBackgroundTileCellEditingEnabled(bool _Enabled)  { s_BackgroundTileCellEditingEnabled = _Enabled; }
    static CBackgroundTile* GetBackgroundTileEditingTarget()                    { return s_BackgroundTileEditingTarget; }
    static TILE_EDIT_BRUSH  GetBackgroundTileBrush()                    { return s_BackgroundTileBrush; }
    static void             SetBackgroundTileBrush(TILE_EDIT_BRUSH _Brush)      { s_BackgroundTileBrush = _Brush; }
    static FIRST_SPAWN_LOC  GetBackgroundTileSelectedSpawnLoc()                 { return s_BackgroundTileSelectedSpawnLoc; }
    static void             SetBackgroundTileSelectedSpawnLoc(FIRST_SPAWN_LOC _Loc) { s_BackgroundTileSelectedSpawnLoc = _Loc; }

};
