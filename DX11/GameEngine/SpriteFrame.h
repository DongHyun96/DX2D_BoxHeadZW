#pragma once
#include "04.Asset/Entity.h"
#include "04.Asset/01.Mesh/AMesh.h"

class SpriteFrame : public Entity
{
private:
    
    Ptr<AMesh> m_Mesh{};
    
public:
    
    SpriteFrame();
    virtual ~SpriteFrame() override;

public:
    
    Ptr<AMesh> GetFrameMesh() const { return m_Mesh; }
    
public:
    
    /// <summary>
    /// UV 값 설정된 Sprite 프레임 Mesh 생성 
    /// </summary>
    /// <param name="_SpriteName"> : Sprite 이름 </param>
    /// <param name="FrameX"> : Frame으로 쓸 부분의 X좌표 (0 ~ (SpriteWidthCnt - 1))</param>
    /// <param name="FrameY"> : Frame으로 쓸 부분의 Y좌표 (0 ~ (SpriteHeightCnt - 1)</param>
    /// <returns> : 제대로 생성되었다면 return S_OK, 아니라면 E_FAIL </returns>
    HRESULT Create(const wstring& _SpriteName, UINT FrameX, UINT FrameY, UINT SpriteWidthCnt, UINT SpriteHeightCnt);
    
};
