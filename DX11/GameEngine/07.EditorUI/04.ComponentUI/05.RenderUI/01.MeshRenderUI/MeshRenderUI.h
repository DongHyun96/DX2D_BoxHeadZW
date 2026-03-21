#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/RenderUI.h"

class MeshRenderUI : public RenderUI
{
public:

    MeshRenderUI();
    virtual ~MeshRenderUI() override;

public:
    
    void Tick_UI() override;
    
private:
    
    /// <summary>
    /// Mesh와 Material 별 Tick_UI 처리 Boilerplate code  
    /// </summary>
    /// <param name="_MeshRender"></param>
    /// <param name="_AssetType"> : Mesh 또는 Material 만 처리됨</param>
    void Tick_UI(const Ptr<CMeshRender>& _MeshRender, ASSET_TYPE _AssetType);
    
private:
    
    void OnSelectMesh(DWORD_PTR _ListUI);
    void OnSelectMaterial(DWORD_PTR _ListUI);  
};
