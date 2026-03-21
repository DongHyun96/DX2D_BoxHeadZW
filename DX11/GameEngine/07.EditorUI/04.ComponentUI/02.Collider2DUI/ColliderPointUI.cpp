#include "pch.h"
#include "ColliderPointUI.h"

ColliderPointUI::ColliderPointUI()
    : ComponentUI(COMPONENT_TYPE::COLLIDER2D_POINT, "ColliderPointUI")
{
    m_ComponentTitle = "Collider2D Point";
}

ColliderPointUI::~ColliderPointUI()
{
}

void ColliderPointUI::Tick_UI()
{
    ComponentUI::Tick_UI();
    
    CColliderPoint* Collider   = GetTargetObject()->ColliderPoint().Get();

    ImGui::Text("Offset");
    ImGui::SameLine(100);
    
    Vec2 Offset = Collider->GetOffset();
    if (ImGui::DragFloat2("##Offset", Offset, 0.01f))
        Collider->SetOffset(Offset);
    
}
