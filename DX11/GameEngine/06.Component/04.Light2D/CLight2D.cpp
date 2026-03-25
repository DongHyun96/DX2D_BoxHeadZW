#include "pch.h"
#include "CLight2D.h"

#include "GameEngine/06.Component/01.Transform/CTransform.h"

CLight2D::CLight2D()
    : Component(COMPONENT_TYPE::LIGHT2D)
{
}

CLight2D::~CLight2D()
{
}

void CLight2D::FinalTick()
{
    if (!GetOwner()->GetIsActive() || !GetOwner()->GetIsVisible()) return;
    
    m_Info.WorldPos = Transform()->GetWorldPos();
    m_Info.LightDir = Transform()->GetDir(DIR::RIGHT);
    RenderMgr::GetInst()->RegisterLight2D(this);
}

void CLight2D::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_Info, sizeof(Light2DInfo), 1, _File);
}

void CLight2D::LoadFromLevelFile(FILE* _File)
{
    fread(&m_Info, sizeof(Light2DInfo), 1, _File);
}
