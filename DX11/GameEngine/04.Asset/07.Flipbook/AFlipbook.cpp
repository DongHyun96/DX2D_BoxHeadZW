#include "pch.h"
#include "AFlipbook.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

AFlipbook::AFlipbook()
    : Asset(ASSET_TYPE::FLIPBOOK)
{
}

AFlipbook::~AFlipbook()
{
}

Ptr<Asset> AFlipbook::CreateNewAsset()
{
    const wstring FileNameWithoutExtension = GetFileNameWithoutExtension(GetKey());
    Ptr<AFlipbook> NewAsset = AssetMgr::GetInst()->CreateNewAsset<AFlipbook>(FileNameWithoutExtension);

    /* 나머지 멤버변수 복사 처리 */
    NewAsset->m_vecSprite = this->m_vecSprite;
    return NewAsset.Get();
}

void AFlipbook::InsertSprite(int _Idx, const Ptr<ASprite>& _Sprite)
{
    if (_Idx < 0) _Idx = 0;
    if (_Idx > static_cast<int>(m_vecSprite.size()))
        _Idx = static_cast<int>(m_vecSprite.size());

    m_vecSprite.insert(m_vecSprite.begin() + _Idx, _Sprite);
}

bool AFlipbook::RemoveSprite(int _Idx)
{
    if (_Idx < 0 || _Idx > static_cast<int>(m_vecSprite.size())) return false;
        
    m_vecSprite.erase(m_vecSprite.begin() + _Idx);
    return true;
}

bool AFlipbook::RemoveSprite(const Ptr<ASprite>& _Sprite)
{
    if (_Sprite == nullptr) return false;
        
    for (vector<Ptr<ASprite>>::iterator it = m_vecSprite.begin(); it != m_vecSprite.end(); ++it)
    {
        if (*it == _Sprite)
        {
            m_vecSprite.erase(it);
            return true;
        }
    }
    
    return false;
}

bool AFlipbook::ClearSprites()
{
    m_vecSprite.clear();
    return true;
}

HRESULT AFlipbook::Save(const wstring& _FilePath)
{
    if (FAILED(Asset::Save(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[AFlipbook::Save] : Open File failed!");
        return E_FAIL;
    }
    
    // 이 Flipbook 전용 RenderOffset 및 RenderScale값 저장
    fwrite(&m_RenderOffset, sizeof(Vec2), 1, pFile);
    fwrite(&m_RenderScale, sizeof(Vec2), 1, pFile);

    // 몇장의 스프라이트로 구성되었는지 저장
    UINT SpriteCount = m_vecSprite.size();
    fwrite(&SpriteCount, sizeof(UINT), 1, pFile);

    // 가리키고 있었던 Sprite가 누군지 저장
    for (const Ptr<ASprite>& Sprite : m_vecSprite)
        SaveAssetRef(pFile, Sprite.Get());
    
    fclose(pFile);
    
    return S_OK;
}

HRESULT AFlipbook::Load(const wstring& _FilePath)
{
    if (FAILED(Asset::Load(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"rb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[AFlipbook::Load] : Open File failed!");
        return E_FAIL;
    }

    // 이 Flipbook 전용 RenderOffset 및 RenderScale값 불러오기 (TODO : 저장하고 이 주석 풀기)
    /*fread(&m_RenderOffset, sizeof(Vec2), 1, pFile);
    fread(&m_RenderScale, sizeof(Vec2), 1, pFile);*/
    
    UINT SpriteCount{};
    fread(&SpriteCount, sizeof(UINT), 1, pFile);

    for (int i = 0; i < SpriteCount; ++i)
    {
        Ptr<ASprite> pSprite = LoadAssetRef<ASprite>(pFile);
        AddSprite(pSprite);
    }
    
    fclose(pFile);
    
    return S_OK;
}
