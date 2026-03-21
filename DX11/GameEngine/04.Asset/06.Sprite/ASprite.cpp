#include "pch.h"
#include "ASprite.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

ASprite::ASprite()
    : Asset(ASSET_TYPE::SPRITE)
{
}

ASprite::~ASprite()
{
}

bool ASprite::SetByFrameCoord(const Vec2& _FrameXY, const Vec2& _TotalFrameXY)
{
    if (!m_Atlas) return false;
    
    assert(_FrameXY.x >= 0.f && _FrameXY.y >= 0.f, L"ASprite::SetByFrameCoord 실패");
    assert(_FrameXY.x < _TotalFrameXY.x && _FrameXY.y < _TotalFrameXY.y, L"ASprite::SetByFrameCoord 실패");
    
    const float Width       = m_Atlas->GetWidth();
    const float Height      = m_Atlas->GetHeight();
    const Vec2 SlicePixel   = Vec2(Width / _TotalFrameXY.x, Height / _TotalFrameXY.y); // 한 칸의 너비, 높이 size
    
    SetLeftTopUV(Vec2((SlicePixel.x / Width) * _FrameXY.x, (SlicePixel.y / Height) * _FrameXY.y));
    SetSliceUV(SlicePixel / Vec2(Width, Height));
    
    return true;
}

HRESULT ASprite::Save(const wstring& _FilePath)
{
    if (FAILED(Asset::Save(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[ASprite::Save] : Open File failed!");
        return E_FAIL;
    }

    // 가리키고 있던 Texture가 누군지 저장
    SaveAssetRef(pFile, m_Atlas.Get());

    // LeftTop, Slice 정보 저장
    fwrite(&m_LeftTopUV,    sizeof(Vec2), 1, pFile);
    fwrite(&m_SliceUV,      sizeof(Vec2), 1, pFile);
    fwrite(&m_BackgroundUV, sizeof(Vec2), 1, pFile);
    fwrite(&m_OffsetUV,     sizeof(Vec2), 1, pFile);

    fclose(pFile);
    
    return S_OK;
}

HRESULT ASprite::Load(const wstring& _FilePath)
{
    if (FAILED(Asset::Load(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"rb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[ASprite::Load] : Open File failed!");
        return E_FAIL;
    }
    
    m_Atlas = LoadAssetRef<ATexture>(pFile);

    fread(&m_LeftTopUV,     sizeof(Vec2), 1, pFile);
    fread(&m_SliceUV,       sizeof(Vec2), 1, pFile);
    fread(&m_BackgroundUV,  sizeof(Vec2), 1, pFile);
    fread(&m_OffsetUV,      sizeof(Vec2), 1, pFile);

    fclose(pFile);
    
    return S_OK;
}
