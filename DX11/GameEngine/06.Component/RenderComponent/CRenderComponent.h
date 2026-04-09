#pragma once
#include "GameEngine/04.Asset/01.Mesh/AMesh.h"
#include "GameEngine/04.Asset/05.Material/AMaterial.h"
#include "GameEngine/06.Component/Component.h"

class CRenderComponent : public Component
{
private:

    Ptr<AMesh>      m_Mesh{};
    
    Ptr<AMaterial>  m_Material{};           // 현재 사용중인 재질
    Ptr<AMaterial>  m_SharedMaterial{};     // 공유 재질(에셋 매니저의 관리를 받는..)
    Ptr<AMaterial>  m_DynamicMaterial{};    // 나만의 동적 재질(쓰고 버리는..)

private:
    
    Vec2 m_RenderOffset{};
    Vec2 m_RenderScale =  Vec2::One;
    
public:
    
    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Origin);
    
    virtual ~CRenderComponent() override;
    
public:
    
    GET_SET(Ptr<AMesh>, Mesh)
    GET(Ptr<AMaterial>, Material)
    void SetMaterial(const Ptr<AMaterial>& _Material) { m_Material = m_SharedMaterial = _Material; }
    
    GET_SET(Vec2, RenderOffset)
    GET_SET(Vec2, RenderScale)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void FinalTick() override;
    virtual void Render() = 0;

public:
    
    /// <summary> Init 시점에 자신이 사용할 기본 재질 로딩 및 생성, RenderComponent 종류에 따라 각자 처리 </summary>
    virtual void CreateMaterial() = 0;

public:
    
    /// <summary>
    /// <para> SharedMaterial 반환과 동시에, </para>
    /// <para> m_Material(현재 이 RenderComponent가 사용중인 Material)을 SharedMaterial로 세팅한다. </para>
    /// </summary>
    /// <returns></returns>
    Ptr<AMaterial> ReturnToSharedMaterial();
    
    /// <summary>
    /// <para> SharedMaterial을 복사하여 이 RenderComponent만 사용하는 동적 재질 생성 </para>
    /// <para> 만약 이미 DynamicMaterial이 존재한다면 해당 Material 사용 </para>
    /// <para> Level이 Play 중인 상태에서만 동작 가능 </para>
    /// </summary>
    /// <returns> 세팅된 DynamicMaterial 반환 </returns>
    Ptr<AMaterial> CreateDynamicMaterial();
    
    
protected:
    
    void PrintIrrelevantDebugLog() const;

    /// <summary>
    /// Material Const Vec4_3 슬롯을 사용할 예정
    /// </summary>
    void ApplyRenderTransformConst();
    
public:
    
    void DeregisterFromRenderDomain();
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
