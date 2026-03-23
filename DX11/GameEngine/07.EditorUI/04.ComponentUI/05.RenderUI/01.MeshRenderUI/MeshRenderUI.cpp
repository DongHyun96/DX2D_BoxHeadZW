#include "pch.h"
#include "MeshRenderUI.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/05.ListUI/ListUI.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

MeshRenderUI::MeshRenderUI()
    : RenderUI(COMPONENT_TYPE::MESH_RENDER, "MeshRenderUI")
{
    m_ComponentTitle = "Mesh Renderer";
}

MeshRenderUI::~MeshRenderUI()
{
}


void MeshRenderUI::Tick_UI()
{
    RenderUI::Tick_UI();

    Ptr<CMeshRender> pMeshRender = GetTargetObject()->MeshRender();

    Tick_UI(pMeshRender, ASSET_TYPE::MESH);
    Tick_UI(pMeshRender, ASSET_TYPE::MATERIAL);
}

void MeshRenderUI::Tick_UI(const Ptr<CMeshRender>& _MeshRender, ASSET_TYPE _AssetType)
{
    if (_AssetType != ASSET_TYPE::MESH && _AssetType != ASSET_TYPE::MATERIAL) return;
    
    const bool IsMesh = _AssetType == ASSET_TYPE::MESH;
    
    ImGui::Text(IsMesh ? "Mesh" : "Material");
    ImGui::SameLine(120);

    Ptr<Asset> pTargetAsset = IsMesh ?  static_cast<Asset*>(_MeshRender->GetMesh().Get()) :
                                        static_cast<Asset*>(_MeshRender->GetMaterial().Get());
    
    string AssetKey = pTargetAsset ? string(pTargetAsset->GetKey().begin(), pTargetAsset->GetKey().end()) : "None";

    const string InputTextKey = IsMesh ? "##MeshName" : "##MaterialName";
    
    ImGui::InputText(InputTextKey.c_str(), AssetKey.data(), AssetKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

    /*****************/
    /* 드래그로 처리 */
    /*****************/
    
    // 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중아면 true
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content"))
        {

            if (!TreeUI::IsPayloadMultiData(Payload))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                if (IsMesh)
                {
                    if (pAsset->GetType() == ASSET_TYPE::MESH)
                        _MeshRender->SetMesh(static_cast<AMesh*>(pAsset.Get()));
                }
                else
                {
                    if (pAsset->GetType() == ASSET_TYPE::MATERIAL)
                        _MeshRender->SetMaterial(static_cast<AMaterial*>(pAsset.Get()));
                }
            }
        }
        
        ImGui::EndDragDropTarget();
    }

    /*****************/
    /* 버튼으로 처리 */
    /*****************/
    ImGui::SameLine();
    const string ButtonKey = IsMesh ? "##MeshBtn" : "##MtrlBtn";
    if (ImGui::Button(ButtonKey.c_str(), Vec2(20.f, 20.f)))
    {
        // 버튼이 눌리면, 리스트UI 를 찾아서 활성화 시키고, 출력시키고 싶은 문자열을 ListUI 에 등록시킨다.
        Ptr<ListUI> pUI = dynamic_cast<ListUI*>(EditorMgr::GetInst()->FindUI("ListUI").Get());
        assert(pUI.Get());
        
        pUI->SetUIName(IsMesh ? "Mesh List" : "Material List");
        
        vector<wstring> vecAssetNames;
        AssetMgr::GetInst()->GetAssetKeys(IsMesh ? ASSET_TYPE::MESH : ASSET_TYPE::MATERIAL, vecAssetNames);
        pUI->AddString(vecAssetNames);
        pUI->AddDelegate(this, static_cast<DELEGATE_1>(IsMesh ? &MeshRenderUI::OnSelectMesh : &MeshRenderUI::OnSelectMaterial));
        pUI->SetActive(true);
    }
}

void MeshRenderUI::OnSelectMesh(DWORD_PTR _ListUI)
{
    Ptr<ListUI> pListUI = reinterpret_cast<ListUI*>(_ListUI);

    wstring key = wstring(pListUI->GetSelectedString().begin(), pListUI->GetSelectedString().end());

    Ptr<AMesh> pMesh = FIND_ASSET(AMesh, key);

    GetTargetObject()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::OnSelectMaterial(DWORD_PTR _ListUI)
{
    Ptr<ListUI> pListUI = reinterpret_cast<ListUI*>(_ListUI);

    wstring key = wstring(pListUI->GetSelectedString().begin(), pListUI->GetSelectedString().end());

    Ptr<AMaterial> pMtrl = FIND_ASSET(AMaterial, key);

    GetTargetObject()->MeshRender()->SetMaterial(pMtrl);
}
