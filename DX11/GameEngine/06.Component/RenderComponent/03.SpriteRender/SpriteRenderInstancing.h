#pragma once

class AMesh;
class AGraphicShader;
class ATexture;
class ASprite;

namespace SpriteRenderInstancing
{
    void BeginFrame();
    void Flush();
    
    void Submit
    (
        AMesh*              _Mesh,
        AGraphicShader*     _Shader,
        ATexture*           _Atlas,
        const Matrix&       _World,
        const Vec2&         _LeftTopUV,
        const Vec2&         _SliceUV
    );
}
