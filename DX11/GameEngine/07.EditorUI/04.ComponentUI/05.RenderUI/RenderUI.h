#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"


class RenderUI : public ComponentUI
{
private:

    string m_Title{};
    
public:
    
    RenderUI(COMPONENT_TYPE _Type, const string& _UIName);
    virtual ~RenderUI() override;

    void Tick_UI() override;
    
    
};
