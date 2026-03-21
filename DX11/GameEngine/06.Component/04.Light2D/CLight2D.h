#pragma once
#include "Header/struct.h"

class CLight2D : public Component
{
private:

    Light2DInfo m_Info{};
    
public:
    
    CLight2D();
    virtual ~CLight2D() override;
    
    CLONE(CLight2D)
    
public:
    
    virtual void FinalTick() override;
    
public:

    const Light2DInfo& GetInfo() const { return m_Info; }
    
    LIGHT_TYPE GetLightType() const { return m_Info.Type; }
    void SetLightType(LIGHT_TYPE light) { m_Info.Type = light; }
    
    Vec3 GetLightColor() const { return m_Info.Color; }
    void SetLightColor(const Vec3& _Color) { m_Info.Color = _Color; }

    // void SetLightDir(const Vec3& _Dir) { m_Info.LightDir = _Dir; }
    Vec3 GetLightDir() const { return m_Info.LightDir; }

    Vec3 GetAmbient() const { return m_Info.Ambient; }
    void SetAmbient(const Vec3& _Ambient) { m_Info.Ambient = _Ambient; }
    
    float GetRadius() const { return m_Info.Radius; }
    void SetRadius(float _Radius) { m_Info.Radius = _Radius; }
    
    float GetAngle() const { return m_Info.Angle; }
    void SetAngle(float _Angle) { m_Info.Angle = _Angle; }
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
