#include "pch.h"
#include "RenderUI.h"

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
    /* Renderer Default 정보 */
    
    ColoredButtonTitle(m_Title);
    ImGui::Spacing(); ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    
    SubHeading("<MESH INFO>");
    
    ImGui::Spacing();
    ImGui::Spacing();

    wstring MeshName = GetTargetObject()->GetRenderCom()->GetMesh()->GetKey();
    string Text = "Current Mesh : " + string(MeshName.begin(), MeshName.end()); 
    ImGui::BulletText(Text.c_str());
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    
    SubHeading("<MATERIAL>");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    wstring ShaderName = GetTargetObject()->GetRenderCom()->GetMaterial()->GetKey();
    Text = "Current Shader : " + string(ShaderName.begin(), ShaderName.end());
    ImGui::BulletText(Text.c_str());
    
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::TreeNode("Textures"))
    {
        for (UINT i = 0; i < TEX_END; ++i)
        {
            TEX_PARAM Param = static_cast<TEX_PARAM>(i);
            Ptr<ATexture> TextureAsset = GetTargetObject()->GetRenderCom()->GetMaterial()->GetTexture(Param);
            
            wstring TextureName = (TextureAsset) ? TextureAsset->GetKey() : L"None";
            
            string Text = "TEX" + to_string(i) + " : " + string(TextureName.begin(), TextureName.end()); 
            ImGui::BulletText(Text.c_str());
        }
        
        ImGui::TreePop();
    }
    
    ComponentUI::Tick_UI();
    
}
