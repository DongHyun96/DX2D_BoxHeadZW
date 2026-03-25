#include "pch.h"
#include "CollisionMatrixUI.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/04.Level/ALevel.h"

CollisionMatrixUI::CollisionMatrixUI()
    : EditorUI("CollisionMatrixUI")
{
    SetActive(false);
}

CollisionMatrixUI::~CollisionMatrixUI()
{
}

void CollisionMatrixUI::Tick_UI()
{
    Ptr<ALevel> CurLevel = LevelMgr::GetInst()->GetCurLevel();
    
    if (!CurLevel)
    {
        ImGui::Text("No Level Loaded");
        return;
    }

    if (!m_NameInit) RefreshFromLevel();

    const bool canEdit = (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP);
    ImGui::BeginDisabled(!canEdit);

    ImGui::BeginTable("##CollisionMatrixLayout", 2, ImGuiTableFlags_SizingStretchProp);

    // Left: Layer Names
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Layer Names");

    ImGui::PushItemWidth(140.f);
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        ImGui::PushID(i);
        
        ImGui::Text("%02d", i);
        ImGui::SameLine();

        if (m_arrLayerNameError[i]) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
            
        if (ImGui::InputText("##LayerName", m_LayerNameBuf[i], sizeof(m_LayerNameBuf[i])))
        {
            if (m_arrLayerNameError[i]) ImGui::PopStyleColor();
            m_arrLayerNameError[i] = false;
        }
        
        if (m_arrLayerNameError[i]) ImGui::PopStyleColor();
        
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            string s = m_LayerNameBuf[i];
            wstring ws(s.begin(), s.end());
            
            if (!CurLevel->IsLayerNameDuplicated(ws, i))
            {
                CurLevel->GetLayer(i)->SetName(ws);
                CurLevel->SetChanged();
            }
            else m_arrLayerNameError[i] = true;
            
        }
        ImGui::PopID();
    }
    ImGui::PopItemWidth();

    // Right: Collision Matrix
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("Collision Matrix");

    if (ImGui::BeginTable("##CollisionMat", MAX_LAYER + 1, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("Layer");
        for (int c = 0; c < MAX_LAYER; ++c)
            ImGui::TableSetupColumn(std::to_string(c).c_str());

        ImGui::TableHeadersRow();

        UINT* Mat = CurLevel->GetCollisionMatrix();
        for (int r = 0; r < MAX_LAYER; ++r)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", r);

            for (int c = 0; c < MAX_LAYER; ++c)
            {
                ImGui::TableSetColumnIndex(c + 1);
                int row = (r < c) ? r : c;
                int col = (r < c) ? c : r;

                bool enabled = (Mat[row] & (1u << col)) != 0;

                if (c < r) ImGui::BeginDisabled(true);

                ImGui::PushID(r * MAX_LAYER + c);
                if (ImGui::Checkbox("##chk", &enabled))
                    CurLevel->CheckCollisionLayer(r, c);
                ImGui::PopID();

                if (c < r) ImGui::EndDisabled();
            }
        }
        ImGui::EndTable();
    }

    ImGui::EndTable();
    ImGui::EndDisabled();
}

void CollisionMatrixUI::RefreshFromLevel()
{
    Ptr<ALevel> level = LevelMgr::GetInst()->GetCurLevel();
    if (!level) return;

    for (int i = 0; i < MAX_LAYER; ++i)
    {
        wstring name = level->GetLayer(i)->GetName();
        string s(name.begin(), name.end());

        memset(m_LayerNameBuf[i], 0, sizeof(m_LayerNameBuf[i]));
        strncpy_s(m_LayerNameBuf[i], s.c_str(), sizeof(m_LayerNameBuf[i]) - 1);
    }

    m_NameInit = true;
}

void CollisionMatrixUI::Deactivate()
{
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        if (m_arrLayerNameError[i])
            m_LayerNameBuf[i][0] = '\0';
    }
}
