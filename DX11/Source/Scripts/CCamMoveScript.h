#pragma once
#include "GameEngine/06.Component/Script/CScript.h"

/*enum class CAM_EFFECT_TYPE
{
    AIRSTRIKE,
    END
};*/

class CCamMoveScript : public CScript
{
private:

    const float m_CamLerpAlphaSpeed = 7.5f;
    Vec3 m_CamLerpDestPos{};

private:
    
    /// <summary>
    /// 해당 Effect가 켜져있는지 조사 
    /// </summary>
    /*map<CAM_EFFECT_TYPE, bool> m_mapCamEffectEnabled = 
    {
        { CAM_EFFECT_TYPE::AIRSTRIKE, false },    
    };*/
    
    // 해당 AirStrike가 잡혀있다면 AirStrike Effect 진행
    class CAirStrike*   m_AirStriker{};
    float               m_OrthoScaleDest = 1.f;
    float               m_OrthoScaleLerpAlphaSpeed = 20.f;
public:
    
    CCamMoveScript();
    virtual ~CCamMoveScript() override;

    CLONE(CCamMoveScript)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    void HandleCameraEffect();
    void HandleBoundary();
    
public:
    
    // void SetCameraEffect(CAM_EFFECT_TYPE _Effect, bool _Enabled) { m_mapCamEffectEnabled[_Effect] = _Enabled; }
    void SetAirStriker(CAirStrike* _AirStrike) { m_AirStriker = _AirStrike; }
    void SetOrthoScaleLerpData(float _OrthoScaleDest, float _OrthoScaleLerpAlphaSpeed) { m_OrthoScaleDest = _OrthoScaleDest; m_OrthoScaleLerpAlphaSpeed = _OrthoScaleLerpAlphaSpeed; }
    
private:
    
    void MovePerspective();
    // void CheckTogglingTargetMode();
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
