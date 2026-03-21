#include "pch.h"
#include "Light2DUI.h"

Light2DUI::Light2DUI()
    : ComponentUI(COMPONENT_TYPE::LIGHT2D, "Light2DUI")
{
    m_ComponentTitle = "Light2D";
}

Light2DUI::~Light2DUI()
{
}

void Light2DUI::Tick_UI()
{
    ComponentUI::Tick_UI();
    
    // Light2D 정보
    /*LIGHT_TYPE  Type{};
    Vec3        Color{};
    Vec3        Ambient{};  // 환경광, 광원이 존재하면서 최소한으로 발생하는 빛의 세기 (Directional Light일 때에만)
    Vec3        LightDir{}; // 광원의 빛이 향하는 방향
    Vec3        WorldPos{}; // 광원의 위치 (Point, Spot)
    float       Radius{};   // 빛의 영향 반경 (Point, Spot)
    float       Angle{};    // SpotLight 범위 각*/ // (Spot)

    static ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_None;

    /* LightType */
    ImGui::Text("LightType");
    ImGui::SameLine();
    LIGHT_TYPE LightType = GetTargetObject()->Light2D()->GetLightType();
    const char* items[] = { "DIRECTIONAL", "POINT", "SPOT" };
    if (ImGui::Combo("##LightType", reinterpret_cast<int*>(&LightType), items, IM_COUNTOF(items)))
        GetTargetObject()->Light2D()->SetLightType(LightType);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("LightColor");
    ImGui::SameLine(100);
    Vec3 LightColor = GetTargetObject()->Light2D()->GetLightColor();
    if (ImGui::ColorEdit3("##LightColor", LightColor, base_flags))
        GetTargetObject()->Light2D()->SetLightColor(LightColor);

    ImGui::BeginDisabled(LightType != LIGHT_TYPE::DIRECTIONAL);
    ImGui::Text("Ambient");
    ImGui::SameLine(100);
    Vec3 AmbientColor = GetTargetObject()->Light2D()->GetAmbient();
    if (ImGui::ColorEdit3("##Ambient", AmbientColor, base_flags))
        GetTargetObject()->Light2D()->SetAmbient(AmbientColor);
    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Spacing();
    
    // Not applicable, use Light Object Transform's rotation instead
    /*ImGui::Text("LightDirection");
    ImGui::SameLine(100);
    Vec3 LightDir = GetTargetObject()->Light2D()->GetLightDir();
    if (ImGui::ColorEdit3("##LightDir", LightDir, base_flags))
        GetTargetObject()->Light2D()->SetLightDir(LightDir);*/
    
    ImGui::BeginDisabled(LightType == LIGHT_TYPE::DIRECTIONAL);
    ImGui::Text("Light Radius");
    ImGui::SameLine(100);
    float Radius = GetTargetObject()->Light2D()->GetRadius();
    if (ImGui::DragFloat("##Radius", &Radius))
        GetTargetObject()->Light2D()->SetRadius(Radius);
    ImGui::EndDisabled();
    
    ImGui::BeginDisabled(LightType != LIGHT_TYPE::SPOT);
    ImGui::Text("SpotLightAngle");
    ImGui::SameLine(100);
    float Angle = GetTargetObject()->Light2D()->GetAngle();
    float Degree = Angle * 180.f / XM_PI; 
    if (ImGui::SliderFloat("##SpotLightAngle", &Degree, 0.f, 360.f))
    {
        Angle = Degree * (XM_PI / 180.f);    
        GetTargetObject()->Light2D()->SetAngle(Angle);
    }
    ImGui::EndDisabled();
}
