#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"
#include "GameEngine/07.EditorUI/01.Inspector/ObjectInspectorViewer.h"

class PrefabUI : public AssetUI
{
private:
    
    ObjectInspectorViewer m_ObjectViewer{};
    
public:
    
    PrefabUI();
    virtual ~PrefabUI() override;
    
public:
    
    void Tick_UI() override;
    
};
