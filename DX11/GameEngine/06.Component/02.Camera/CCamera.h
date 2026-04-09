#pragma once
#include "GameEngine/06.Component/Component.h"

enum class PROJ_TYPE
{
    ORTHOGRAPHIC,   // 직교투영
    PERSPECTIVE,    // 원근투영
};

class CCamera : public Component
{
private:

    UINT        m_LayerCheck{};     // 어떤 레이어만 화면에 렌더링 할 것인지 비트체크
    PROJ_TYPE   m_Type{};           // 투영방식
    
    float       m_Far{};            // 카메라 시야 최대거리 (현재 Clipping 공간 1~10000으로 설정되어 있음)
    float       m_Width{};          // 투영 가로길이 (직교투영)
    float       m_AspectRatio{};    // 종횡비 (가로 / 세로), 세로대비 가로의 길이 비율
    
    float       m_FOV{};            // 시야각
    
    float       m_OrthoScale = 1.f;     // 직교투영 배율
    
    Matrix      m_matView{};        // View 행렬
    Matrix      m_matProj{};        // Proj 행렬

    /*vector<Ptr<GameObject>> m_vecOpaque{};
    vector<Ptr<GameObject>> m_vecMasked{};
    vector<Ptr<GameObject>> m_vecTransparent{};
    vector<Ptr<GameObject>> m_vecPostProcess{};*/
    
public:
    
    CCamera();
    virtual ~CCamera() override;
    
public:
    
    CLONE(CCamera)
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:

    GET_SET(PROJ_TYPE, Type)
    
    float GetFar() const { return m_Far; }
    void SetFar(float _Far) { m_Far = _Far; }
    
    GET_SET(float, Width)
    GET_SET(float, AspectRatio)
    
    float GetFOV() const { return m_FOV * (180.f / XM_PI); }
    void SetFOV(float _Degree) { m_FOV = _Degree * (XM_PI / 180.f); }
    
    GET_SET(float, OrthoScale)
    
    UINT GetLayerCheck() const { return m_LayerCheck; }
    
public:
    
    void LayerCheckAll() { m_LayerCheck = 0xffffffff; } // 0xff 255 -> 8개의 비트를 모두 채워줌 
    void LayerCheckClear() { m_LayerCheck = m_LayerCheck = 0; }

    /// <summary>
    /// 해당 Idx Layer on off toggle
    /// </summary>
    void LayerCheck(int _Idx);
    
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void FinalTick() override;
    
    void Render(bool _bUseRenderDomainSort = true);
    
private:
    
    // void SortObject();
    
};
