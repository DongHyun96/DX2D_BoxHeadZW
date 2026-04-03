#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class CBackgroundTile;

class ScriptUI : public ComponentUI
{
private:
    
    unordered_map<void*, vector<char>> m_WStringInputBuffer{};
    
private:
    
    Ptr<CScript>    m_TargetScript;
    UINT            m_ItemHeight;


public:
    ScriptUI();
    virtual ~ScriptUI() override;

public:
    static bool IsBackgroundTileCellEditingEnabled() { return s_BackgroundTileCellEditingEnabled; }
    static void SetBackgroundTileCellEditingEnabled(bool _Enabled) { s_BackgroundTileCellEditingEnabled = _Enabled; }
    static CBackgroundTile* GetBackgroundTileEditingTarget() { return s_BackgroundTileEditingTarget; }
    static bool GetBackgroundTileBrushTakenValue() { return s_BackgroundTileBrushTakenValue; }
    static void SetBackgroundTileBrushTakenValue(bool _Taken) { s_BackgroundTileBrushTakenValue = _Taken; }
    
public:
    void SetScript(CScript* _Script);

public:
    virtual void Tick_UI() override;
    
private:
    
    void TickScriptParams();
    void TickBackgroundTileEditingUI();

private:
    static bool s_BackgroundTileCellEditingEnabled;
    static CBackgroundTile* s_BackgroundTileEditingTarget;
    static bool s_BackgroundTileBrushTakenValue;

    void AddItemHeight();
    
    virtual void OnRemoveScriptConfirmed(bool _Confirmed) override;

};
