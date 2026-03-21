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
    EditorUI* inst = m_Inst;
    DELEGATE_BOOL mem = m_MemFunc;

    // 먼저 현재 Confirm를 닫는다.
    // 콜백 내부에서 다시 Confirm를 열면 그 상태를 유지할 수 있다.
    SetActive(false);

    if (inst && mem)
        (inst->*mem)(true);
}

void ConfirmUI::OnNo()
{
    EditorUI* inst = m_Inst;
    DELEGATE_BOOL mem = m_MemFunc;

    SetActive(false);

    if (inst && mem)
        (inst->*mem)(false);
}

void ConfirmUI::Deactivate()
{
    m_WarningText.clear();
    m_Inst = nullptr;
    m_MemFunc = nullptr;
}
