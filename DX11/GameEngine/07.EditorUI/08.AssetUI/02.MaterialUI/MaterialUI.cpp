#include "pch.h"
#include "MaterialUI.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/05.ListUI/ListUI.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

MaterialUI::MaterialUI()
    : AssetUI(ASSET_TYPE::MATERIAL)
{
}

MaterialUI::~MaterialUI()
{
}

void MaterialUI::Tick_UI()
{
    AssetUI::Tick_UI();

    Ptr<AMaterial> pMtrl = static_cast<AMaterial*>(GetTargetAsset().Get());

    // ======
    // Shader
    // ======
    ImGui::Text("Shader");
    ImGui::SameLine(100);

    Ptr<AGraphicShader> pShader = pMtrl->GetShader();

    wstring ShaderKey = pShader ? pShader->GetKey() : L"None";
	
    ImGui::InputText("##ShaderName", string(ShaderKey.begin(), ShaderKey.end()).data(), ShaderKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

    // 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content"))
        {
            if (!TreeUI::IsPayloadMultiData(PayLoad))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(PayLoad->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                if (ASSET_TYPE::GRAPHICS_SHADER == pAsset->GetType())
                {
                    AGraphicShader* ReceivedShader = static_cast<AGraphicShader*>(pAsset.Get()); 
                    pMtrl->SetShader(ReceivedShader);
                    if (ReceivedShader->GetKey() == L"Std2DShader")
                    {
                        SCALAR_PARAM Param = SCALAR_PARAM::VEC4_0; // TintColor
                        pMtrl->SetScalar<Vec4>(Param, Vec4(1.f, 1.f, 1.f, 1.f)); // TintColor DefaultColor로 설정
                    }
                }
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    if (ImGui::Button("##ShaderBtn", Vec2(20.f, 20.f)))
    {
        // 버튼이 눌리면, 리스트UI 를 찾아서 활성화 시키고, 출력시키고 싶은 문자열을 ListUI 에 등록시킨다.
        Ptr<ListUI> pUI = dynamic_cast<ListUI*>(EditorMgr::GetInst()->FindUI("ListUI").Get());
        assert(pUI.Get());

        pUI->SetUIName("Shader List");

        vector<wstring> vecShaderNames;
        AssetMgr::GetInst()->GetAssetKeys(ASSET_TYPE::GRAPHICS_SHADER, vecShaderNames);
        pUI->AddString(vecShaderNames);
        pUI->AddDelegate(this, static_cast<DELEGATE_1>(&MaterialUI::OnSelectShader));
        pUI->SetActive(true);
    }


    // Render Domain 지정 기능
    TickRenderDomain(pMtrl);
    
    // Shader BS Type 지정 기능
    TickSelectBlendState(pMtrl);
    
    // Shader RS Type 지정 기능
    TickSelectRasterizerState(pMtrl);

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    ImGui::Text("Shader Parameter");
    ImGui::Separator();
    
    // Shader Param
    ShaderParameterTick();

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    
    // Save Button
    SaveButton();
    /*if (ImGui::Button("Save##MtrlSaveBtn"))
    {
        wstring FilePath = CONTENT_PATH + pMtrl->GetKey();
        pMtrl->Save(FilePath);
    }*/
}

void MaterialUI::TickRenderDomain(const Ptr<AMaterial>& _InspectingMaterial)
{
    if (!ImGui::CollapsingHeader("Render Domain", ImGuiTreeNodeFlags_None)) return;
    
    if (ImGui::BeginTable("##RenderDomain", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        RENDER_DOMAIN CurrentDomain = _InspectingMaterial->GetDomain();
        
        for (UINT i = 0; i < static_cast<UINT>(RENDER_DOMAIN::DOMAIN_NONE) + 1; ++i)
        {
            RENDER_DOMAIN Domain = static_cast<RENDER_DOMAIN>(i);
            
            ImGui::TableNextColumn();
            
            const bool Selected = (Domain == CurrentDomain);
            if (ImGui::Selectable(RenderDomainTypeToString(Domain).c_str(), Selected))
                _InspectingMaterial->SetDomain(Domain);
        }
        
        ImGui::EndTable();
    }
}

void MaterialUI::TickSelectBlendState(const Ptr<AMaterial>& _InspectingMaterial)
{
    if (!_InspectingMaterial->GetShader()) return;
    if (!ImGui::CollapsingHeader("Shader Blend State", ImGuiTreeNodeFlags_None)) return;
    
    if (ImGui::BeginTable("##BlendState", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        BS_TYPE CurrentBSType = _InspectingMaterial->GetShader()->GetBSType();
        
        for (UINT i = 0; i < static_cast<UINT>(BS_TYPE::END); ++i)
        {
            BS_TYPE BSType = static_cast<BS_TYPE>(i); 
            
            ImGui::TableNextColumn();
            
            const bool Selected = (CurrentBSType == BSType);
            if (ImGui::Selectable(BSTypeToString(BSType).c_str(), Selected))
                _InspectingMaterial->GetShader()->SetBSType(BSType);
        }
        
        ImGui::EndTable();
    }
}

void MaterialUI::TickSelectRasterizerState(const Ptr<AMaterial>& _InspectingMaterial)
{
    if (!_InspectingMaterial->GetShader()) return;
    if (!ImGui::CollapsingHeader("Shader Rasterizer State", ImGuiTreeNodeFlags_None)) return;
    
    if (ImGui::BeginTable("##RasterizerState", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        RS_TYPE CurrentRSType = _InspectingMaterial->GetShader()->GetRSType();
        
        for (UINT i = 0; i < static_cast<UINT>(RS_TYPE::END); ++i)
        {
            RS_TYPE RSType = static_cast<RS_TYPE>(i); 
            
            ImGui::TableNextColumn();
            
            const bool Selected = (CurrentRSType == RSType);
            if (ImGui::Selectable(RSTypeToString(RSType).c_str(), Selected))
                _InspectingMaterial->GetShader()->SetRSType(RSType);
        }
        
        ImGui::EndTable();
    }
}

void MaterialUI::ShaderParameterTick()
{
    Ptr<AMaterial> pMtrl = static_cast<AMaterial*>(GetTargetAsset().Get());
    
    if (!pMtrl->GetShader()) return;
    
    const vector<ShaderParam>& vecParam = pMtrl->GetShader()->GetShaderParams();

    
    
    for (const ShaderParam& shaderParam : vecParam)
    {
        string Desc = string(shaderParam.Desc.begin(), shaderParam.Desc.end());
        if (Desc.empty()) Desc = "Description None";
        ImGui::Text(Desc.c_str());
        
        switch (shaderParam.Type)
        {
        case SHADER_PARAM::INT:
        {
            ImGui::SameLine();
            
            SCALAR_PARAM Param = static_cast<SCALAR_PARAM>(static_cast<UINT>(SCALAR_PARAM::INT_0) + shaderParam.Index); 
            int& Data = pMtrl->GetScalar<int>(Param);
            ImGui::DragInt("##DragInt", &Data);
        }
            break;
        case SHADER_PARAM::FLOAT:
        {
            ImGui::SameLine();
            
            SCALAR_PARAM Param = static_cast<SCALAR_PARAM>(static_cast<float>(SCALAR_PARAM::FLOAT_0) + shaderParam.Index); 
            float& Data = pMtrl->GetScalar<float>(Param);
            ImGui::DragFloat("##DragInt", &Data);
        }
            break;
        case SHADER_PARAM::VEC2:
        {
            ImGui::SameLine();
            
            SCALAR_PARAM Param = static_cast<SCALAR_PARAM>(static_cast<float>(SCALAR_PARAM::VEC2_0) + shaderParam.Index); 
            Vec2& Data = pMtrl->GetScalar<Vec2>(Param);
            ImGui::DragFloat2("##DragFloat2", Data);
        }
            break;
        case SHADER_PARAM::VEC4:
        {
            ImGui::SameLine();
            
            SCALAR_PARAM Param = static_cast<SCALAR_PARAM>(static_cast<UINT>(SCALAR_PARAM::VEC4_0) + shaderParam.Index); 
            Vec4& Data = pMtrl->GetScalar<Vec4>(Param);
            static ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_Float;
            if (ImGui::ColorEdit4("##Float4", Data, base_flags))
                pMtrl->SetScalar<Vec4>(Param, Data);
            
        }
            break;
        case SHADER_PARAM::MAT:
        {
            
        }
            break;
        case SHADER_PARAM::TEX:
        {
            Ptr<ATexture> pTexture = pMtrl->GetTexture(TEX_0);
            ImTextureRef SRV = pTexture ? pTexture->GetSRV().Get() : nullptr;
            
            // 이미지 샘플
            ImGui::ImageWithBg
            (
                SRV,
                ImVec2(100, 100),
                Vec2(0.f, 0.f), Vec2(1.f, 1.f),
                ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
            );
            
            /*****************/
            /* 드래그 처리 */
            /*****************/
            
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content"))
                {
                    if (!TreeUI::IsPayloadMultiData(Payload))
                    {
                        DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                        Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                        if (ASSET_TYPE::TEXTURE == pAsset->GetType())
                            pMtrl->SetTexture(TEX_0, static_cast<ATexture*>(pAsset.Get()));
                    }
                }
        
                ImGui::EndDragDropTarget();
            }
        }
            break;
        }
    }
}

void MaterialUI::OnSelectShader(DWORD_PTR _ListUI)
{
    const Ptr<ListUI>           pListUI     = reinterpret_cast<ListUI*>(_ListUI);
    const wstring               key         = wstring(pListUI->GetSelectedString().begin(), pListUI->GetSelectedString().end());
    const Ptr<AGraphicShader>   pShader     = FIND_ASSET(AGraphicShader, key);
    const Ptr<AMaterial>        pMaterial   = static_cast<AMaterial*>(GetTargetAsset().Get());
    
    pMaterial->SetShader(pShader.Get());

    // Std2DShader에 한해 TintColor DefaultColor로 바로 세팅
    if (pShader->GetKey() == L"Std2DShader")
    {
        SCALAR_PARAM Param = SCALAR_PARAM::VEC4_0; // TintColor
        pMaterial->SetScalar<Vec4>(Param, Vec4(1.f, 1.f, 1.f, 1.f)); // TintColor DefaultColor로 설정
    }
}
