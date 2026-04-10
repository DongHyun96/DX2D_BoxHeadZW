#pragma once

class AMesh;
class AGraphicShader;
class ATexture;

namespace FlipbookRenderInstancing
{
    void BeginInstancing();
    void FlushInstancing();
    
    void Submit
    (
        AMesh*              _Mesh,
        AGraphicShader*     _Shader,
        ATexture*           _Atlas,
        const Matrix&       _World,
        const Vec4&         _RenderTransform,
        const Vec4&         _UV0,
        const Vec4&         _UV1,
        const Vec4&         _TintColor
    );
}
