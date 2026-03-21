#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class CameraUI : public ComponentUI
{
public:

    CameraUI();
    virtual ~CameraUI() override;
    
public:
    virtual void Tick_UI() override;
    
};
