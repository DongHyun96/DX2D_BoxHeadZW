#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class MaterialUI : public AssetUI
{
public:
    MaterialUI();
    virtual ~MaterialUI();
    
public:
    virtual void Tick_UI() override;

private:
    
    void TickRenderDomain(const Ptr<AMaterial>& _InspectingMaterial);
    
private:
    
    /// <summary>
    /// ShaderParameter Tick (Shader에서 요구하는 파라미터값 수정) 
    /// </summary>
    void ShaderParameterTick();
    
    void OnSelectShader(DWORD_PTR _ListUI);

};
