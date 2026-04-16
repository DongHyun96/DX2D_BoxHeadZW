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
    
    bool        m_bIsFirstMainCamera{}; // Level 첫 시작 시, 첫 MainCamera로 잡힐 카메라 오브젝트인지 체크
    bool        m_bIsUICamera{};        // Level 시작 시, 첫 UICamera로 잡힐 카메라 오브젝트인지 체크
    
private:

    UINT        m_LayerCheck{};     // 어떤 레이어만 화면에 렌더링 할 것인지 비트체크
    PROJ_TYPE   m_Type{};           // 투영방식
    
    float       m_Far{};            // 카메라 시야 최대거리 (현재 Clipping 공간 1~10000으로 설정되어 있음)
    float       m_Width{};          // 투영 가로길이 (직교투영)
    float       m_AspectRatio{};    // 종횡비 (가로 / 세로), 세로대비 가로의 길이 비율
    
    float       m_FOV{};            // 시야각
    
    float       m_OrthoScale = 1.f;     // 직교투영 배율
    
    Vec2        m_ViewRectMin{};        // 2D Frustum culling용 ViewRectMin
    Vec2        m_ViewRectMax{};        // 2D Frustum culling용 ViewRectMax

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
    
    const Matrix& GetViewMat() const { return m_matView; }
    const Matrix& GetProjMat() const { return m_matProj; }
    
public:
    
    void LayerCheckAll() { m_LayerCheck = 0xffffffff; } // 0xff 255 -> 8개의 비트를 모두 채워줌 
    void LayerCheckClear() { m_LayerCheck = m_LayerCheck = 0; }

    /// <summary>
    /// 해당 Idx Layer on off toggle
    /// </summary>
    void LayerCheck(int _Idx);
    

public: // Level Stop 상태에서 현재 Editor로 바라보는 중인 Level에서의 MainCamera 및 UICamera 지정 (Level Stop 상태에서만 동작을 한다)
    
    bool SetAsFirstMainCamera(bool _bAsMainCamera);
    bool SetAsUICamera(bool _bAsUICamera);
    
    bool GetIsFirstMainCamera() const { return m_bIsFirstMainCamera; }
    bool GetIsUICamera() const { return m_bIsUICamera; }
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void FinalTick() override;
    
    void Render(bool _bUseRenderDomainSort = true);
    void RenderGameUI();
    
private:
    
    // void SortObject();
    
};
