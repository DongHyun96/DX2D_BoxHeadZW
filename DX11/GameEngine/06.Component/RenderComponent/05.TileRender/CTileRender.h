#pragma once
#include "GameEngine/02.Device/StructuredBuffer/StructuredBuffer.h"
#include "GameEngine/04.Asset/08.TileMap/ATileMap.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

struct SpriteInfo
{
    Vec2 LeftTop{};
    Vec2 Slice{};
};

class CTileRender : public CRenderComponent
{
private:
    
    Ptr<ATileMap>           m_TileMap{};
    vector<SpriteInfo>      m_vecSpriteInfo{};
    Ptr<StructuredBuffer>   m_Buffer{};
    
    
public:
    
    CTileRender();
    CTileRender(const CTileRender& _Origin);
    
    virtual ~CTileRender() override;
    
public:
    
    CLONE(CTileRender)
    
public:
    void Init() override;
    void FinalTick() override;
    void Render() override;
    void CreateMaterial() override;
    
public:

    Ptr<ATileMap> GetTileMap() const { return m_TileMap; }
    void SetTileMap(const Ptr<ATileMap>& _TileMap);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
