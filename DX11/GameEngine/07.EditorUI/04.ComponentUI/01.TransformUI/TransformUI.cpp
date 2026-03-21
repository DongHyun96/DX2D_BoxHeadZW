#include "pch.h"
#include "TransformUI.h"

#include "GameEngine/05.GameObject/GameObject.h"

TransformUI::TransformUI()
    : ComponentUI(COMPONENT_TYPE::TRANSFORM, "TransformUI")
{
    m_ComponentTitle = "Transform";
}

TransformUI::~TransformUI()
{
}

void TransformUI::Tick_UI()
{
    ComponentUI::Tick_UI();
    
    Vec3 vPos   = GetTargetObject()->Transform()->GetRelativePos();
    Vec3 vScale = GetTargetObject()->Transform()->GetRelativeScale();
    Vec3 vRot   = GetTargetObject()->Transform()->GetRelativeRot();

    ImGui::Text("Position");
    ImGui::SameLine(100);
    if (ImGui::DragFloat3("##POSITION", vPos)) // 인자로 넘긴 vPos의 값이 바뀌었다면 true
        GetTargetObject()->Transform()->SetRelativePos(vPos);

    ImGui::Text("Scale");
    ImGui::SameLine(100); // 100Pixel 뒤로 이어붙이기
    if (ImGui::DragFloat3("##SCALE", vScale))
        GetTargetObject()->Transform()->SetRelativeScale(vScale);

    Vec3 vDegree = vRot * 180.f / XM_PI;

    ImGui::Text("Rotation");
    ImGui::SameLine(100);
    if (ImGui::DragFloat3("##ROTATION", vDegree))
    {
        vRot = vDegree * (XM_PI / 180.f);
        GetTargetObject()->Transform()->SetRelativeRot(vRot);
    }


    bool IndependentScale = GetTargetObject()->Transform()->GetIndependentScale(); 

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Independent Scale");
    ImGui::SameLine();
    if (ImGui::Checkbox("##Independent", &IndependentScale))
        GetTargetObject()->Transform()->SetIndependentScale(IndependentScale);

}
