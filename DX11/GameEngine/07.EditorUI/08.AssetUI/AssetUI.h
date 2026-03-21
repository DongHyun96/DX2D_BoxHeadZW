#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

class AssetUI : public EditorUI
{
private:
    const ASSET_TYPE    m_AssetType{};
    Ptr<Asset>          m_TargetAsset{};
    
    

public:
    AssetUI(ASSET_TYPE _Type);
    virtual ~AssetUI();

protected:
    
    void OutputTitle() const;
    
public:
    
    virtual void Tick_UI() override;

public:
    
    GET_SET(Ptr<Asset>, TargetAsset)
    
protected:
    
    void SaveButton();
    
};
