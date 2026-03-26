#pragma once
#include "GameEngine/04.Asset/Asset.h"
#include "GameEngine/04.Asset/02.Texture/ATexture.h"
#include "GameEngine/04.Asset/06.Sprite/ASprite.h"

class ATileMap : public Asset
{
private:
    UINT                    m_Row{};              // 타일맵의 행 개수
    UINT                    m_Col{};              // 타일맵의 열 개수
    Vec2                    m_TileSize{};         // 타일맵을 구성하는 타일 1개의 크기

    Ptr<ATexture>           m_Atlas{};            // 타일맵을 구성하는 Sprite 들의 공통 아틀라스 텍스쳐
    vector<Ptr<ASprite>>    m_vecSpriteInfo{};
    
private:

    // CTileRender에서 이 ATileMap을 SetTileMap 처리해서 참조중인 TileRender Component를 들고 있는다.
    // ATileMap::Save 시, 수정된 TileMap을 기반으로 CTileRender::SetTileMap을 호출시켜 재구성이 필요해서 들고 있는다.
    // 해당 작업 이후 비로소, 기존의 CTileRender에 제대로 적용됨
    set<class CTileRender*> m_setTileRenderComponentReferencer{};
    
private: 
    
    // TileMap 만드는 과정에서, AtlasTexture가 서로 다른 Sprite로 생성한 TileMap의 경우, 아래의 정보값도 같이 초기화시킴
    // 추후, 다른 AtlasTexture 타일셋이 타일맵으로 들어왔을 때, TileMap Editor에서 생성한 기존의 AtlasTexture 및 Sprite들을 정리하기 위함
    
    bool            m_UsesGeneratedAtlas{};
    wstring         m_GeneratedAtlasBaseName{}; // 예: "MyAtlas"
    vector<wstring> m_GeneratedSpriteKeys{};    // 예: "Sprite\\MyAtlas_0_0.sprite"
    

public:
    ATileMap();
    virtual ~ATileMap() override;

public:
    /// <summary>
    /// TileMap Row, Col 총 개수 setting
    /// </summary>
    void SetRowCol(UINT _Row, UINT _Col);

    /// <summary>
    /// 해당 Row, col 자리에 Sprite 지정
    /// </summary>
    void SetSprite(UINT _Row, UINT _Col, const Ptr<ASprite>& _Sprite);
    
    GET(UINT, Row)
    GET(UINT, Col)
    
    const vector<Ptr<ASprite>>& GetSprites() const { return m_vecSpriteInfo; }
    
    GET_SET(Vec2, TileSize)
    GET_SET(Ptr<ATexture>, Atlas)

    /// <summary>
    /// 세팅되어진 Atlas 크기와 Param으로 들어온 Atlas의 총 프레임 X, Y count에 따른 TileSize 지정
    /// </summary>
    /// <returns> : Valid하지 않은 Total X, Y Count를 받거나, Atlas가 지정되지 않은 상태라면 return false </returns>
    bool SetTileSizeByAtlasTotalRowColCount(UINT _X, UINT _Y);
    
public:
    
    bool UsesGeneratedAtlas() const { return m_UsesGeneratedAtlas; }
    const wstring& GetGeneratedAtlasBaseName() const { return m_GeneratedAtlasBaseName; }
    const vector<wstring>& GetGeneratedSpriteKeys() const { return m_GeneratedSpriteKeys; }

    void SetGeneratedAtlasMeta(bool uses, const wstring& baseName, const vector<wstring>& spriteKeys)
    {
        m_UsesGeneratedAtlas        = uses;
        m_GeneratedAtlasBaseName    = baseName;
        m_GeneratedSpriteKeys       = spriteKeys;
    }

    void ClearGeneratedAtlasMeta()
    {
        m_UsesGeneratedAtlas = false;
        m_GeneratedAtlasBaseName.clear();
        m_GeneratedSpriteKeys.clear();
    }
    
public:
    
    virtual HRESULT Save(const wstring& _FilePath) override;
    virtual HRESULT Load(const wstring& _FilePath) override;
    
public:
    
    /// <summary>
    /// CTileRender Referencer 객체 제거 
    /// </summary>
    /// <returns> : 제대로 제거되었다면 return true </returns>
    bool RemoveTileRenderReferencer(CTileRender* _Referencer);

    /// <summary>
    /// CTileRender Referencer 객체 추가
    /// </summary>
    /// <returns> : 제대로 중복된 요소 없이 추가되었다면 return true </returns>
    bool AddTileRenderReferencer(CTileRender* _Referencer) { return m_setTileRenderComponentReferencer.insert(_Referencer).second; }
    
};
