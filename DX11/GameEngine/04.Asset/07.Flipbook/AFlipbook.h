#pragma once
#include "GameEngine/04.Asset/Asset.h"
#include "GameEngine/04.Asset/06.Sprite/ASprite.h"

class AFlipbook : public Asset
{
private:
    
    vector<Ptr<ASprite>> m_vecSprite{};
    
public:
    
    AFlipbook();
    virtual ~AFlipbook() override;

private:
    
    virtual Ptr<Asset> CreateNewAsset() override;
    
public:
    
    void AddSprite(const Ptr<ASprite>& _Sprite)  { m_vecSprite.push_back(_Sprite); }

    void InsertSprite(int _Idx, const Ptr<ASprite>& _Sprite);
    
    bool RemoveSprite(int _Idx);
    bool RemoveSprite(const Ptr<ASprite>& _Sprite);
    
public:
    
    void SetSprite(int _Idx, const Ptr<ASprite>& _Sprite)
    {
        if (m_vecSprite.size() <= _Idx)
            m_vecSprite.resize(_Idx + 1);
        
        m_vecSprite[_Idx] = _Sprite;
    }
    
    Ptr<ASprite> GetSprite(int _Idx) { return m_vecSprite[_Idx]; }
    
    UINT GetSpriteCount() const { return m_vecSprite.size(); }
    
    virtual HRESULT Save(const wstring& _FilePath) override;
    virtual HRESULT Load(const wstring& _FilePath) override;
    
};
