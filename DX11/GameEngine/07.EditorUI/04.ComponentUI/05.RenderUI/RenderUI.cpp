#include "pch.h"
#include "RenderUI.h"

RenderUI::RenderUI(COMPONENT_TYPE _Type, const string& _UIName)
    : ComponentUI(_Type, _UIName)
{
    string Title = _UIName;
    if (Title.ends_with("UI")) Title.erase(Title.size() - 2);
    m_Title = Title;
}

RenderUI::~RenderUI()
{
}

void RenderUI::Tick_UI()
{
    ComponentUI::Tick_UI();

    ImGui::Spacing();
    ImGui::Separator();
    
    // Render Offset, Render Scale 조정
    Vec2 vRenderOffset  = GetTargetObject()->GetRenderCom()->GetRenderOffset();
    Vec2 vRenderScale   = GetTargetObject()->GetRenderCom()->GetRenderScale();

    ImGui::Text("RenderOffset");
    ImGui::SameLine(100);
    if (ImGui::DragFloat2("##Render Offset", vRenderOffset))
        GetTargetObject()->GetRenderCom()->SetRenderOffset(vRenderOffset);

    ImGui::Text("RenderScale");
    ImGui::SameLine(100); // 100Pixel 뒤로 이어붙이기
    if (ImGui::DragFloat2("##RENDERSCALE", vRenderScale))
        GetTargetObject()->GetRenderCom()->SetRenderScale(vRenderScale);
    
    ImGui::Spacing();
    ImGui::Separator();
}
