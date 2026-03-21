#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class MeshUI : public AssetUI
{
public:
    MeshUI();
    virtual ~MeshUI() override;
    
public:
    virtual void Tick_UI() override;

};
