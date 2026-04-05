#include "pch.h"
#include "RenderUI.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

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
    
    // Material 지정
    ImGui::Text("Material");
    ImGui::SameLine(105);
    wstring MtrlKey = GetTargetObject()->GetRenderCom()->GetMaterial() ? GetTargetObject()->GetRenderCom()->GetMaterial()->GetKey() : L"None";
    ImGui::InputText("##MtrlKey", string(MtrlKey.begin(), MtrlKey.end()).data(), MtrlKey.length() + 1, ImGuiInputTextFlags_ReadOnly);
    
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content"))
        {
            if (!TreeUI::IsPayloadMultiData(PayLoad))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(PayLoad->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                if (ASSET_TYPE::MATERIAL == pAsset->GetType())
                {
                    AMaterial* ReceivedMaterial = static_cast<AMaterial*>(pAsset.Get());
                    GetTargetObject()->GetRenderCom()->SetMaterial(ReceivedMaterial);
                }
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::Spacing();
    
    // Render Offset, Render Scale 조정
    Vec2 vRenderOffset  = GetTargetObject()->GetRenderCom()->GetRenderOffset();
    Vec2 vRenderScale   = GetTargetObject()->GetRenderCom()->GetRenderScale();

    ImGui::Checkbox("Enable RenderOffest Ctrl + IJKL editing", &m_bEnableRenderOffsetIJKLKeyInput);
    
    ImGui::BeginDisabled(!m_bEnableRenderOffsetIJKLKeyInput);
    ImGui::DragFloat("IJKL Speed", &m_IJKLOffsetModifySpeed);
    ImGui::EndDisabled();
    
    ImGui::Text("RenderOffset");
    ImGui::SameLine(100);
    if (ImGui::DragFloat2("##Render Offset", vRenderOffset, 0.001f))
        GetTargetObject()->GetRenderCom()->SetRenderOffset(vRenderOffset);
    
    ImGuiIO& io = ImGui::GetIO();
    const bool canEditByIJKL =
        m_bEnableRenderOffsetIJKLKeyInput &&
        !io.WantTextInput &&
        ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    if (canEditByIJKL && ImGui::IsKeyDown(ImGuiMod_Ctrl))
    {
        Vec2 RenderOffset = GetTargetObject()->GetRenderCom()->GetRenderOffset();

        Vec2 Direction{};
        if (ImGui::IsKeyDown(ImGuiKey_J)) Direction.x -= 1.f;
        if (ImGui::IsKeyDown(ImGuiKey_L)) Direction.x += 1.f;
        if (ImGui::IsKeyDown(ImGuiKey_K)) Direction.y -= 1.f;
        if (ImGui::IsKeyDown(ImGuiKey_I)) Direction.y += 1.f;
        Direction.Normalize();
        
        RenderOffset += Direction * E_DT * m_IJKLOffsetModifySpeed;
        GetTargetObject()->GetRenderCom()->SetRenderOffset(RenderOffset);
    }

    ImGui::Text("RenderScale");
    ImGui::SameLine(100); // 100Pixel 뒤로 이어붙이기
    if (ImGui::DragFloat2("##RENDERSCALE", vRenderScale, 0.001f, 0.1f))
        GetTargetObject()->GetRenderCom()->SetRenderScale(vRenderScale);
    
    ImGui::Spacing();
    ImGui::Separator();
}
