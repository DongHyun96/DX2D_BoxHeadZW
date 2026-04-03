#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

class CBackgroundTile;

class MainWindowDropDetectorUI : public EditorUI
{
private:
    
    bool                m_HasSceneRect{};
    ImVec2              m_SceneMin{};
    ImVec2              m_SceneMax{};
    bool                m_IsBackgroundTilePainting{};
    bool                m_BackgroundTilePaintValue{};
    bool                m_HasSelectedCell{};
    int                 m_SelectedCellX{};
    int                 m_SelectedCellY{};
    bool                m_HasLastPaintedCell{};
    int                 m_LastPaintedCellX{};
    int                 m_LastPaintedCellY{};

public:
    MainWindowDropDetectorUI();
    virtual ~MainWindowDropDetectorUI() override;

private:
    void Tick() override;
    void Tick_UI() override;
    
public:

    /*void SetSceneRect(const ImVec2& _Min, const ImVec2& _Max);
    void ClearSceneRect() { m_HasSceneRect = false; }
    bool ConsumeDrop(vector<DWORD_PTR>& _OutPayload, bool& _OutMulti);*/
    
private:
    
    Vec3 GetMouseWorldPosInSceneRect(const ImVec2& _Min, const ImVec2& _Max);
    bool TryGetMouseCellCoord(CBackgroundTile* _BackgroundTile, const ImVec2& _Min, const ImVec2& _Max, int& _OutCellX, int& _OutCellY);
    void TickBackgroundTileCellEditing(CBackgroundTile* _BackgroundTile, const ImVec2& _Min, const ImVec2& _Max);
    
public:
    
    Vec3 GetMouseWorldPosInSceneRect();
    
};
