#pragma once

class AMesh;
class AGraphicShader;
class ATexture;

namespace TileDecalInstancing
{
    void BeginInstancing();
    void FlushInstancing();
    
    void Submit
    (
        AMesh*              _Mesh,
        AGraphicShader*     _Shader,
        ATexture*           _Atlas,
        const Matrix&       _World,
        const Vec2&         _LeftTopUV,
        const Vec2&         _SliceUV,
        const Vec4&         _TintColor
    );
}
