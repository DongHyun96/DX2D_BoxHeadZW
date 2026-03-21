#include "pch.h"
#include "ContentUI.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"


ContentUI::ContentUI()
    : EditorUI("ContentUI")
{
    m_Tree = new TreeUI;
    AddChildUI(m_Tree.Get());
    m_Tree->SetSeparator(false);
    m_Tree->AddDynamicSelect(this, static_cast<DELEGATE_1>(&ContentUI::OnSelectAsset));
    ReNew();
}

ContentUI::~ContentUI()
{
}

void ContentUI::Tick_UI()
{
    if (AssetMgr::GetInst()->IsChanged()) ReNew();
    // DeleteAssetTick();
    ChangeAssetNameTick();
    DuplicateAssetTick();
}

void ContentUI::ChangeAssetNameTick()
{
    ImGuiIO& io = ImGui::GetIO();
    const ImGuiInputFlags flags = ImGuiInputFlags_RouteFocused;

    // F2 눌렀을 때 Rename 모드 진입
    if (!io.WantTextInput && ImGui::Shortcut(ImGuiKey_F2, flags))
    {
        Ptr<TreeNode> node = m_Tree->GetSelected();
        
        if (node && node->Data != 0)
        {
            m_RenameMode = true;
            m_RenameTarget = node->Data;

            Ptr<Asset> asset = reinterpret_cast<Asset*>(node->Data);
            
            if (asset->GetIsProvidedByEngine())
            {
                m_RenameMode = false;
                m_RenameTarget = 0;
                return;
            }

            const wstring FileName = asset->GetKey().empty() ? L"UnNamed" : GetFileName(asset->GetKey()); 
            const string FileNameStr = string(FileName.begin(), FileName.end());

            memset(m_RenameBuf, 0, sizeof(m_RenameBuf));
            strncpy_s(m_RenameBuf, FileNameStr.c_str(), sizeof(m_RenameBuf) - 1);

            ImGui::SetKeyboardFocusHere();
        }
    }

    // Rename 모드 처리
    if (m_RenameMode)
    {
        ImGui::SetNextItemWidth(200.f);

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            RenameModeEndBoilerPlate();
            return;
        }
        
        
        // 엔터키일 경우
        if (ImGui::InputText("##RenameAsset", m_RenameBuf, sizeof(m_RenameBuf),
            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Ptr<Asset> asset = reinterpret_cast<Asset*>(m_RenameTarget);
            if (!asset) // Asset 종류의 객체가 아닌 경우
            {
                RenameModeEndBoilerPlate();
                return;
            }
            
            string newName = m_RenameBuf;

            // Texture의 경우, 예외적으로 Extension을 따로 구해야 한다.
            const wstring Extension = asset->GetType() == ASSET_TYPE::TEXTURE ?
                                      GetExtension(asset->GetKey()) : GetAssetTypeExtension(asset->GetType());
            
            wstring NewFileNameFormatted = wstring(newName.begin(), newName.end()) + Extension;
            const wstring NewKey = GetAssetTypeFolderWString(asset->GetType()) + NewFileNameFormatted;
            
            if (AssetMgr::GetInst()->Find(asset->GetType(), NewKey, false))
            {
                // 이미 해당 키가 AssetMgr에 존재하는 상황
                RenameModeEndBoilerPlate();
                return;
            }
            
            // NewKey로 Asset윈도우 파일 이름 바꾸기 가능한지 체크
            if (CanRenameWindowAssetFileName(CONTENT_PATH + asset->GetRelativePath(), NewFileNameFormatted))
            {
                // 실질적인 윈도우 파일명 수정
                RenameWindowAssetFileName(CONTENT_PATH + asset->GetRelativePath(), NewFileNameFormatted);
                
                // Asset Key 수정과 더불어, AssetMgr에서의 Key, 자기자신의 RelativePath까지 일괄 수정되도록 처리
                asset->SetKey(NewKey);
            }
            else // Asset 윈도우 파일이 존재하지 않는 상황 (첫 윈도우 파이 저장 처리 및 키 수정 처리)
            {
                asset->SetKey(NewKey); // RelativePath가 여기서 잡힌다
                asset->SaveBySelfRelativePath();
            }

            RenameModeEndBoilerPlate();
        }
    }
}

void ContentUI::RenameModeEndBoilerPlate()
{
    m_RenameMode = false;
    m_RenameTarget = 0;
    ReNew();
}

void ContentUI::DuplicateAssetTick()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput) return;
    
    const ImGuiInputFlags flags = ImGuiInputFlags_RouteFocused;

    // Ctrl + D키 조합
    if (!ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D, flags)) return;
    
    vector<Ptr<Asset>> vecToDuplicate{};

    const auto& selectedNodes = m_Tree->GetSelectedNodes();
    if (!selectedNodes.empty())
    {
        for (const Ptr<TreeNode>& node : selectedNodes)
        {
            if (node && node->Data != 0)
                vecToDuplicate.push_back(reinterpret_cast<Asset*>(node->Data));
        }
    }
    else
    {
        Ptr<TreeNode> single = m_Tree->GetSelected();
        if (single && single->Data != 0)
            vecToDuplicate.push_back(reinterpret_cast<Asset*>(single->Data));
    }

    bool bCopySucceeded{};
    for (const Ptr<Asset>& asset : vecToDuplicate)
    {
        Ptr<Asset> NewAsset = asset->CreateNewAsset();
        
        // 제대로 복사되었다면(해당 Ctrl+D를 사용하기 위한 CreateNewAsset을 override 한 Asset이라면), AssetMgr에 등록 처리
        if (NewAsset)
        {
            AssetMgr::GetInst()->AddAsset(NewAsset->GetKey(), NewAsset);
            bCopySucceeded = true;
        }
    }

    if (bCopySucceeded) ReNew();
}

void ContentUI::ReNew()
{
    // 이전 선택 Asset Key로 백업
    vector<wstring> prevSelectedKeys{};
    for (const Ptr<TreeNode>& node : m_Tree->GetSelectedNodes())
    {
        if (!node || node->Data == 0) continue;
        if (Ptr<Asset> asset = reinterpret_cast<Asset*>(node->Data)) 
            prevSelectedKeys.push_back(asset->GetKey());
    }

    unordered_set<wstring> selectedKeySet(prevSelectedKeys.begin(), prevSelectedKeys.end());
    vector<Ptr<TreeNode>> nodesToRestore{};
    
    // 트리 재구성
    m_Tree->Clear();

    vector<wstring> vecAssetNames{};
    for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
    {
        const ASSET_TYPE Type = static_cast<ASSET_TYPE>(i);

        Ptr<TreeNode> Node = m_Tree->AddItem(nullptr, ToString(Type));
        Node->SetFramed(true);

        vecAssetNames.clear();
        AssetMgr::GetInst()->GetAssetKeys(Type, vecAssetNames);

        unordered_map<wstring, Ptr<TreeNode>> folderNodes{};
        folderNodes.reserve(vecAssetNames.size());

        for (const wstring& AssetName : vecAssetNames)
        {
            Ptr<Asset> pAsset = AssetMgr::GetInst()->Find(Type, AssetName);
            if (!pAsset) continue;

            wstring DisplayPath = AssetName;
            if (!pAsset->GetRelativePath().empty())
                DisplayPath = pAsset->GetRelativePath();

            vector<wstring> Parts{};
            wstring Cur{};
            for (wchar_t ch : DisplayPath)
            {
                if (ch == L'\\' || ch == L'/')
                {
                    if (!Cur.empty())
                    {
                        Parts.push_back(Cur);
                        Cur.clear();
                    }
                }
                else Cur.push_back(ch);
            }
            if (!Cur.empty()) Parts.push_back(Cur);

            if (Parts.size() > 1)
            {
                string TypeStr = ToString(Type);
                wstring TypeW(TypeStr.begin(), TypeStr.end());
                for (wchar_t& c : TypeW) c = static_cast<wchar_t>(towlower(c));

                wstring Head = Parts.front();
                for (wchar_t& c : Head) c = static_cast<wchar_t>(towlower(c));

                if (Head == TypeW) Parts.erase(Parts.begin());
            }

            if (Parts.empty()) continue;

            Ptr<TreeNode> Parent = Node;
            wstring PathKey{};
            for (size_t p = 0; p + 1 < Parts.size(); ++p)
            {
                if (!PathKey.empty()) PathKey += L"/";
                PathKey += Parts[p];

                auto it = folderNodes.find(PathKey);
                if (it == folderNodes.end())
                {
                    Ptr<TreeNode> folder = m_Tree->AddItem(Parent, string(Parts[p].begin(), Parts[p].end()));
                    folder->SetIsFolderStyleNode(true);
                    folderNodes.insert(make_pair(PathKey, folder));
                    Parent = folder;
                }
                else Parent = it->second;
            }

            const wstring& Leaf = Parts.back();
            Ptr<TreeNode> leafNode = m_Tree->AddItem(
                Parent,
                string(Leaf.begin(), Leaf.end()),
                reinterpret_cast<DWORD_PTR>(pAsset.Get())
            );

            // key 매칭으로 복원 대상 수집
            if (selectedKeySet.find(pAsset->GetKey()) != selectedKeySet.end())
                nodesToRestore.push_back(leafNode);
        }
    }
    
    // ReNew 이후 선택 복원 (없어진 Asset은 자동 스킵)
    bool hasRestored = false;
    for (const Ptr<TreeNode>& node : nodesToRestore)
    {
        if (!node) continue;
        m_Tree->ExpandToNode(node);                           // 부모 체인 열기
        m_Tree->RegisterSelectedEx(node, hasRestored, false); // 다중선택 복원
        hasRestored = true;
    }
}

void ContentUI::OnSelectAsset(DWORD_PTR _Asset)
{
    if (_Asset == 0) return;

    for (const Ptr<Inspector>& InspectorUI : EditorMgr::GetInst()->GetInspectors())
    {
        // Inspector에 골라진 Asset이 있고, Lock이 걸려있을 때
        if (InspectorUI->IsLocked() && InspectorUI->GetTargetAsset())
            continue;
        
        // 클릭한 노드가 들고있는 Asset 주소값을 입력받는다.
        Ptr<Asset> pAsset = reinterpret_cast<Asset*>(_Asset);
        
        InspectorUI->SetTargetAsset(pAsset);
    }
}
