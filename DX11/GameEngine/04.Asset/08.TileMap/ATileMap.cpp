#include "pch.h"
#include "ATileMap.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

ATileMap::ATileMap()
    : Asset(ASSET_TYPE::TILEMAP)
{
}

ATileMap::~ATileMap()
{
}

void ATileMap::SetRowCol(UINT _Row, UINT _Col)
{
    m_Row = _Row;
    m_Col = _Col;
    m_vecSpriteInfo.resize(m_Row * m_Col);
}

void ATileMap::SetSprite(UINT _Row, UINT _Col, const Ptr<ASprite>& _Sprite)
{
    int idx = _Row * m_Col + _Col;
    if (idx < 0 || idx >= static_cast<int>(m_vecSpriteInfo.size())) return;

    if (!_Sprite)
    {
        m_vecSpriteInfo[idx] = nullptr;
        return;
    }

    if (!m_Atlas) m_Atlas = _Sprite->GetAtlas();
    if (_Sprite->GetAtlas() != m_Atlas) return;

    m_vecSpriteInfo[idx] = _Sprite;
}

bool ATileMap::SetTileSizeByAtlasTotalRowColCount(UINT _X, UINT _Y)
{
    if (!m_Atlas)
    {
        assert(nullptr, L"ATileMap::SetTileSizeByAtlasTotalRowColCount: Atlas nullptr!");
        return false;
    }
    if (_X <= 0 || _Y <= 0)
    {
        assert(nullptr, L"ATileMap::SetTileSizeByAtlasTotalRowColCount: invalid arguments");
        return false;
    }
    
    SetTileSize(Vec2(m_Atlas->GetWidth() / _X, m_Atlas->GetHeight() / _Y));
    
    return true;
}

HRESULT ATileMap::Save(const wstring& _FilePath)
{
    if (FAILED(Asset::Save(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[ALevel::Save] : Open File failed!");
        return E_FAIL;
    }
    
    fwrite(&m_Row, sizeof(UINT), 1, pFile);
    fwrite(&m_Col, sizeof(UINT), 1, pFile);
    fwrite(&m_TileSize, sizeof(Vec2), 1, pFile);
    
    SaveAssetRef(pFile, m_Atlas.Get());
    
    UINT SpriteInfoCount = m_vecSpriteInfo.size();
    fwrite(&SpriteInfoCount, sizeof(UINT), 1, pFile);

    for (const Ptr<ASprite>& SpriteInfo : m_vecSpriteInfo)
        SaveAssetRef(pFile, SpriteInfo.Get());
    
    // ===== Generated Atlas Meta =====
    const uint32_t metaTag = 0x4D47544D; // 'MTGM'
    const uint32_t metaVer = 1;
    fwrite(&metaTag, sizeof(uint32_t), 1, pFile);
    fwrite(&metaVer, sizeof(uint32_t), 1, pFile);

    fwrite(&m_UsesGeneratedAtlas, sizeof(bool), 1, pFile);
    SaveWString(pFile, m_GeneratedAtlasBaseName);

    UINT keyCount = (UINT)m_GeneratedSpriteKeys.size();
    fwrite(&keyCount, sizeof(UINT), 1, pFile);
    for (const auto& key : m_GeneratedSpriteKeys)
        SaveWString(pFile, key);
    
    fclose(pFile);
    
    // Save 처리와 동시에, 이 TileMap 에셋의 CTileRender 참조자가 존재한다면,
    // CTileRender의 SetTileMap을 한 번 호출해주어야 CTileRenderer가 제대로 갱신 처리가 됨
    // SetTileMap 내부에 Referencer set 삭제, 추가하는 코드 있어서 iterator로 처리
    for (auto it = m_setTileRenderComponentReferencer.begin(); it != m_setTileRenderComponentReferencer.end();)
    {
        CTileRender* Referencer = *it;
        ++it;
        if (Referencer) Referencer->SetTileMap(this);
    }
    
    return S_OK;
}

HRESULT ATileMap::Load(const wstring& _FilePath)
{
    if (FAILED(Asset::Load(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"rb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[ATileMap::Load] : Open File failed!");
        return E_FAIL;
    }
    
    // atlas 메타데이터 초기화
    ClearGeneratedAtlasMeta();
    
    fread(&m_Row, sizeof(UINT), 1, pFile);
    fread(&m_Col, sizeof(UINT), 1, pFile);
    fread(&m_TileSize, sizeof(Vec2), 1, pFile);
    
    m_Atlas = LoadAssetRef<ATexture>(pFile);
    
    UINT SpriteInfoCount{};
    fread(&SpriteInfoCount, sizeof(UINT), 1, pFile);

    m_vecSpriteInfo.clear();
    
    for (UINT i = 0; i < SpriteInfoCount; ++i)
    {
        Ptr<ASprite> pSprite = LoadAssetRef<ASprite>(pFile);
        m_vecSpriteInfo.push_back(pSprite);
    }

    // ===== Generated Atlas Meta ===== (파일 끝에 메타 데이터가 있으면 읽는 방식)
    long cur = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    long end = ftell(pFile);
    fseek(pFile, cur, SEEK_SET);

    if (end - cur >= static_cast<long>(sizeof(uint32_t)) * 2)
    {
        uint32_t metaTag = 0, metaVer = 0;
        fread(&metaTag, sizeof(uint32_t), 1, pFile);
        fread(&metaVer, sizeof(uint32_t), 1, pFile);

        if (metaTag == 0x4D47544D && metaVer == 1)
        {
            fread(&m_UsesGeneratedAtlas, sizeof(bool), 1, pFile);
            m_GeneratedAtlasBaseName = LoadWString(pFile);

            UINT keyCount = 0;
            fread(&keyCount, sizeof(UINT), 1, pFile);

            m_GeneratedSpriteKeys.clear();
            for (UINT i = 0; i < keyCount; ++i)
                m_GeneratedSpriteKeys.push_back(LoadWString(pFile));
        }
    }
    
    fclose(pFile);
    
    return S_OK;
}

bool ATileMap::RemoveTileRenderReferencer(CTileRender* _Referencer)
{
    if (!m_setTileRenderComponentReferencer.contains(_Referencer)) return false;
    m_setTileRenderComponentReferencer.erase(_Referencer);
    return true;
}
