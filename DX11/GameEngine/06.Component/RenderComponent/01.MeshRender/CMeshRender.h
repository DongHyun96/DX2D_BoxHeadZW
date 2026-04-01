#pragma once
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

/// <summary>
/// <para> 범용적으로 사용 가능한 RenderComponent </para>
/// <para> 직접 Mesh와 Material 세팅 처리를 해야한다 (생성 시, 기본옵션 Material을 제공하지 않음) </para>
/// </summary>
class CMeshRender : public CRenderComponent
{
public:

	CMeshRender();
	virtual ~CMeshRender() override;
	
	CLONE(CMeshRender)
	
public:

	virtual void FinalTick() override;
	virtual void Render() override;
	void CreateMaterial() override {}

};

