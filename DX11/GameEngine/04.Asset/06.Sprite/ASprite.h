#pragma once

#include "GameEngine/04.Asset/Asset.h"
#include "GameEngine/04.Asset/02.Texture/ATexture.h"


/// <summary>
/// 이미지 조각
/// </summary>
class ASprite : public Asset
{
private:
    
    Ptr<ATexture>   m_Atlas{};
    
    Vec2            m_LeftTopUV{};
    Vec2            m_SliceUV{};
    Vec2            m_BackgroundUV{};   // 공통된 UV Background Size
    Vec2            m_OffsetUV{};       // Offset size 
    
public:
    
    ASprite();
    virtual ~ASprite() override;

    
    
public:

    GET_SET(Ptr<ATexture>, Atlas)
    GET_SET(Vec2, LeftTopUV)
    GET_SET(Vec2, SliceUV)
    GET_SET(Vec2, BackgroundUV)
    GET_SET(Vec2, OffsetUV)

    /// <summary>
    /// Atlas가 Setting되었을 때, 프레임 좌표 기준으로 Sprite 세팅 (LeftTopUV, SliceUV 세팅) 
    /// </summary>
    /// <param name="_FrameXY">      : 사용할 SpriteFrame X, Y 좌표 (좌상단 부터 (0, 0) ~ 우하단(TotFrameX - 1, TotFrameY - 1)) </param>
    /// <param name="_TotalFrameXY"> : 총 프레임 x, y 개수 </param>
    /// <returns>                    : Atlas가 미리 setting 되어있지 않았다면 return false </returns>
    bool SetByFrameCoord(const Vec2& _FrameXY, const Vec2& _TotalFrameXY);
    
    virtual HRESULT Save(const wstring& _FilePath) override;
    virtual HRESULT Load(const wstring& _FilePath) override;
    
};
