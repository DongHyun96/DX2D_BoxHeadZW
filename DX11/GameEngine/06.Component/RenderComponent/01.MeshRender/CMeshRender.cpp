#include "pch.h"
#include "CMeshRender.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESH_RENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::FinalTick()
{
	CRenderComponent::FinalTick();
}

void CMeshRender::Render()
{
	// Mesh or shader 미설정 상태
	if (!GetMesh() || !GetMaterial()) return;

	ApplyRenderTransformConst();
	GetMaterial()->Binding();
	GetMesh()->Render();
	
	GetMaterial()->Clear();
}
