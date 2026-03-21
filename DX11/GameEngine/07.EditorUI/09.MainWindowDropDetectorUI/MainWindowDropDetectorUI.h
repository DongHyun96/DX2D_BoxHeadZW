#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

class MainWindowDropDetectorUI : public EditorUI
{
private:
    
    bool                m_HasSceneRect{};
    ImVec2              m_SceneMin{};
    ImVec2              m_SceneMax{};

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
    
public:
    
    Vec3 GetMouseWorldPosInSceneRect();
    
};
