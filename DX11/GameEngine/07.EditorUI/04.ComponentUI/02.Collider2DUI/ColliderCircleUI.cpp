#include "pch.h"
#include "ColliderCircleUI.h"

ColliderCircleUI::ColliderCircleUI()
    : ComponentUI(COMPONENT_TYPE::COLLIDER2D_CIRCLE, "ColliderCircleUI")
{
    m_ComponentTitle = "Collider2D Circle";
}

ColliderCircleUI::~ColliderCircleUI()
{
}

void ColliderCircleUI::Tick_UI()
{
    ComponentUI::Tick_UI();
    
    CColliderCircle* Collider = GetTargetObject()->ColliderCircle().Get();

    bool Active = Collider->GetActive();
    if (ImGui::Checkbox("Active", &Active))
        Collider->SetActive(Active);

    ImGui::Spacing();
    
    ImGui::Text("Offset");
    ImGui::SameLine(100);
    
    Vec2 Offset = Collider->GetOffset();
    if (ImGui::DragFloat2("##Offset", Offset, 0.01f))
        Collider->SetOffset(Offset);
    
    
    ImGui::Text("Radius");
    ImGui::SameLine(100);
    float Radius = Collider->GetRadius();
    if (ImGui::DragFloat("##Radius", &Radius))
    {
        Radius = max(Radius, 0.0f);
        Collider->SetRadius(Radius);
    }
}
