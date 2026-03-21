#include "pch.h"
#include "SpriteFrame.h"

#include "03.Manager/04.AssetMgr/AssetMgr.h"

SpriteFrame::SpriteFrame()
{
}

SpriteFrame::~SpriteFrame()
{
}

HRESULT SpriteFrame::Create(const wstring& _SpriteName, UINT FrameX, UINT FrameY, UINT SpriteWidthCnt, UINT SpriteHeightCnt)
{
    if (FrameX >= SpriteWidthCnt || FrameY >= SpriteHeightCnt)
        return E_FAIL;
    
    // x = 0 ~ 1
    // y = 0 ~ 1
    
    // 정점 데이터
    Vtx arrVtx[4] = {};
    
    const float UnitX = 1.f / SpriteWidthCnt;
    const float UnitY = 1.f / SpriteHeightCnt;

    const float L = FrameX * UnitX;
    const float R = L + UnitX;
    const float T = FrameY * UnitY;
    const float B = T + UnitY;
    
    // 좌상단
    arrVtx[0].vPos      = Vec3(-0.5f, 0.5f, 0.f);
    arrVtx[0].vUV       = Vec2(L, T);

    // 우상단
    arrVtx[1].vPos      = Vec3(0.5f, 0.5f, 0.f);
    arrVtx[1].vUV       = Vec2(R, T);

    // 우하단
    arrVtx[2].vPos      = Vec3(0.5f, -0.5f, 0.f);
    arrVtx[2].vUV       = Vec2(R, B); 

    // 좌하단
    arrVtx[3].vPos      = Vec3(-0.5f, -0.5f, 0.f);
    arrVtx[3].vUV       = Vec2(L, B);
    
    UINT arrIdx[6] = { 0, 2, 3, 0, 1, 2 };
    
    m_Mesh = new AMesh;
    if (FAILED(m_Mesh->Create(arrVtx, 4, arrIdx, 6)))
        return E_FAIL;

    const wstring AssetName = _SpriteName + to_wstring(FrameX) + to_wstring(FrameY);
    AssetMgr::GetInst()->AddAsset(AssetName, m_Mesh.Get());
    
    return S_OK;
}
