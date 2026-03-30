#include "pch.h"
#include "ConfirmUI.h"

ConfirmUI::ConfirmUI()
    : EditorUI("ConfirmUI")
{
}

ConfirmUI::~ConfirmUI()
{
}

void ConfirmUI::Tick_UI()
{
    ImGui::Text(m_WarningText.c_str());

    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) OnYes();
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) OnNo();
    
    if (ImGui::Button("YES")) OnYes();
    ImGui::SameLine();
    if (ImGui::Button("NO")) OnNo();
        
}

void ConfirmUI::OnYes()
{
    if (m_EntityInst && m_MemFunc)
        (m_EntityInst->*m_MemFunc)(true);
    
    SetActive(false);
}

void ConfirmUI::OnNo()
{
    if (m_EntityInst && m_MemFunc)
        (m_EntityInst->*m_MemFunc)(false);
    
    SetActive(false);
}

void ConfirmUI::Deactivate()
{
    m_WarningText.clear();
    m_EntityInst = nullptr;
    m_MemFunc = nullptr;
}
