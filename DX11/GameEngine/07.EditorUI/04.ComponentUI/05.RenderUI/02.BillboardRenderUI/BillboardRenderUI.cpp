#include "pch.h"
#include "BillboardRenderUI.h"

BillboardRenderUI::BillboardRenderUI()
    : RenderUI(COMPONENT_TYPE::BILLBOARD_RENDER, "BillboardRenderUI")
{
    m_ComponentTitle = "Billboard Renderer";
}

BillboardRenderUI::~BillboardRenderUI()
{
}

void BillboardRenderUI::Tick_UI()
{
    RenderUI::Tick_UI();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    Vec2 BillboardScale = GetTargetObject()->BillboardRender()->GetBillboardScale();

    ImGui::Text("Billboard Scale : ");
    ImGui::SameLine();
    if (ImGui::DragFloat2("##BillboardScale", BillboardScale))
        GetTargetObject()->BillboardRender()->SetBillboardScale(BillboardScale);
}
