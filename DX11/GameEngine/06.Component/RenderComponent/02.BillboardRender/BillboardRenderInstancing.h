#pragma once

class AMesh;
class AGraphicShader;
class ATexture;

namespace BillboardRenderInstancing
{
    void BeginFrame();
    void Flush();
    
    void Submit
    (
        AMesh*              _Mesh,
        AGraphicShader*     _Shader,
        ATexture*           _Texture,
        const Matrix&       _World,
        const Vec2&         _BillboardScale
    );
}
