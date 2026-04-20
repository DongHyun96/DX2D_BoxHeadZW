#pragma once
#include "GameEngine/02.Device/StructuredBuffer/StructuredBuffer.h"
#include "GameEngine/04.Asset/08.TileMap/ATileMap.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"
#include "GameEngine/DataStructure/RandomizedSet.h"

struct TileInfo
{
    Vec2 LeftTop{};
    Vec2 Slice{};
};

struct DecalInfo
{
    Vec2   Pos{};         // 타일맵 UV 기준 위치 (0~1)
    Vec2   Scale{};       // 타일맵 UV 기준 크기 (1.f 가 CellSize 1개의 크기)
    Vec2   LeftTop{};     // 데칼 아틀라스 내 UV 좌상단
    Vec2   Slice{};       // 데칼 아틀라스 내 UV 슬라이스
    Vec4   TintColor{};   // 데칼 색상
    Matrix matWorld{};    // 데칼의 월드 행렬 (미리 계산)
    int    Active{};      // 활성화 여부
    int    ID{};          // 데칼 ID
    int    Padding[2]{};

    bool operator==(const DecalInfo& _Other) const { return ID == _Other.ID; }
};

namespace std
{
    template<>
    struct hash<DecalInfo>
    {
        size_t operator()(const DecalInfo& _Info) const
        {
            return hash<int>{}(_Info.ID);
        }
    };
}

class CTileRender : public CRenderComponent
{
private:
    
    Ptr<ATileMap>               m_TileMap{};
    Ptr<ATexture>               m_DecalAtlas{};
    
    Ptr<ATexture>               m_DecalAtlases[TEX_END]{};
    
    vector<TileInfo>            m_vecTileInfo{};
    Ptr<StructuredBuffer>       m_TileBuffer{};

    RandomizedSet<DecalInfo>    m_rsDecalInfo{};
    bool                        m_bDecalChanged{};
    int                         m_iNextDecalID{};
    
private:
    
    static AMesh*           s_RectMesh;
    static AGraphicShader*  s_TileDecalShader;
    
    
public:
    
    CTileRender();
    CTileRender(const CTileRender& _Origin);
    
    virtual ~CTileRender() override;
    
public:
    
    CLONE(CTileRender);
    
public:
    void Init() override;
    void FinalTick() override;
    void Render() override;
    void CreateMaterial() override;
    
public:

    Ptr<ATileMap> GetTileMap() const { return m_TileMap; }
    void SetTileMap(const Ptr<ATileMap>& _TileMap);

    void SetDecalAtlas(const Ptr<ATexture>& _Atlas) { m_DecalAtlas = _Atlas; }
    ATexture* GetDecalAtlas() const { return m_DecalAtlas.Get(); }
    
    int AddDecal(const Vec2& _vPos, const Vec2& _vScale, const Ptr<ASprite>& _pDecalSprite, const Vec4& _TintColor);
    int AddDecal(const Vec2& _vPos, const Vec2& _vScale, const Ptr<ASprite>& _pDecalSprite, float _ColorAlpha);
    void RemoveDecal(int _ID);
    void ClearAllDecals();

    DecalInfo* GetDecalInfo(int _ID);
    void SetDecalAlpha(int _ID, float _Alpha);
    void SetDecalChanged() { m_bDecalChanged = true; }
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
