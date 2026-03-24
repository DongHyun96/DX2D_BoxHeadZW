#include "pch.h"
#include "EditorUI.h"

#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"

EditorUI::EditorUI(const string& _Name)
    : m_UIName(_Name)
    , m_IsModal(false)
    , m_Active(true)
    , m_Parent(nullptr)
{
}

EditorUI::~EditorUI()
{
}

void EditorUI::Tick()
{
    if (m_IsModal)
    {
        const string StrKey = m_UIName + m_UIKey;
        ImGui::OpenPopup(StrKey.c_str());

        bool Active = m_Active;
        bool Temp = m_Active;
        
        if (ImGui::BeginPopupModal(StrKey.c_str(), &Active, ImGuiWindowFlags_AlwaysAutoResize))
        {
            CheckFocus();
            
            Tick_UI();
            
            for (const Ptr<EditorUI>& child : m_vecChildUI)
                if (child->IsActive()) child->Tick();
            
            ImGui::EndPopup();
        }
        else SetActive(Active);
        
        return;
    }
    
    if (!m_Parent) // 최상위 부모일 경우
    {
        bool Active = m_Active;

        const string StrKey = m_UIName + m_UIKey;
        
        ImGui::Begin(StrKey.c_str(), &Active);
        
        if (m_Active != Active) SetActive(Active);

        CheckFocus();
        
        Tick_UI();
        
        for (const Ptr<EditorUI>& child : m_vecChildUI)
            if (child->IsActive()) child->Tick();

        ImGui::End();
        
        return;
    }
    
    // 자식 UI일 경우
    ImGui::BeginChild(m_UIName.c_str(), m_SizeAsChild);

    CheckFocus();
    
    Tick_UI();

    for (const Ptr<EditorUI>& child : m_vecChildUI)
        if (child->IsActive()) child->Tick();
    
    ImGui::EndChild();

    if (m_Separator) ImGui::SeparatorText(""); // 두꺼운 Separator
}

void EditorUI::CheckFocus()
{
    if (ImGui::IsWindowFocused()) EditorMgr::GetInst()->RegisterFocusedUI(this);
}

bool EditorUI::Shortcut(ImGuiKeyChord _KeyChord, ImGuiInputFlags _Flags)
{
    ImGuiIO& io = ImGui::GetIO();
    return !io.WantTextInput && ImGui::Shortcut(_KeyChord, _Flags);
}

void EditorUI::ColoredButtonTitle(const string& _Title, const ImVec4& _Color)
{
    ImGui::PushID(0);
    /*ImGui::PushStyleColor(ImGuiCol_Button,          (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));*/
    ImGui::PushStyleColor(ImGuiCol_Button,        _Color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _Color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  _Color);
    ImGui::Button(_Title.c_str());
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}

// Vec2, Vec4 ImVec형으로 변환 처리 구현
Vec2::operator ImVec2() const { return {x, y}; }
Vec4::operator ImVec4() const { return {x, y, z, w}; }

