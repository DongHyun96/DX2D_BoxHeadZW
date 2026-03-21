#include "pch.h"
#include "ColliderRectUI.h"

ColliderRectUI::ColliderRectUI()
    : ComponentUI(COMPONENT_TYPE::COLLIDER2D_RECT, "ColliderRectUI")
{
    m_ComponentTitle = "Collider2D Rect";
}

ColliderRectUI::~ColliderRectUI()
{
}

void ColliderRectUI::Tick_UI()
{
    ComponentUI::Tick_UI();
    
    CColliderRect* Collider   = GetTargetObject()->ColliderRect().Get();

    ImGui::Text("Offset");
    ImGui::SameLine(100);
    
    Vec2 Offset = Collider->GetOffset();
    if (ImGui::DragFloat2("##Offset", Offset, 0.01f))
        Collider->SetOffset(Offset);

    ImGui::Text("Scale");
    ImGui::SameLine(100);
    Vec2 Scale = Collider->GetScale();
    if (ImGui::DragFloat2("##Scale", Scale, 0.01f))
        Collider->SetScale(Scale);
    
}
