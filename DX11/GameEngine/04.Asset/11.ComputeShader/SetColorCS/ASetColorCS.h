#pragma once

#include "GameEngine/04.Asset/11.ComputeShader/AComputeShader.h"

class ASetColorCS : public AComputeShader
{
private:
    Ptr<ATexture>  m_Target;
    Vec4           m_Color;

public:
    ASetColorCS();
    virtual ~ASetColorCS() override;
    
public:
    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;
    
public:
    void SetTargetTex(Ptr<ATexture> _Tex) { m_Target = _Tex; }
    void SetColor(Vec4 _Color) { m_Color = _Color; }


};