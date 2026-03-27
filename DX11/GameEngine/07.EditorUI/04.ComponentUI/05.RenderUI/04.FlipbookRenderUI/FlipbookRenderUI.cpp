#include "pch.h"
#include "FlipbookRenderUI.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/06.Component/RenderComponent/04.FlipbookRender/CFlipbookRender.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"

FlipbookRenderUI::FlipbookRenderUI()
    : RenderUI(COMPONENT_TYPE::FLIPBOOK_RENDER, "FlipbookRenderUI")
{
    m_ComponentTitle = "Flipbook Renderer";
}

FlipbookRenderUI::~FlipbookRenderUI()
{
}

void FlipbookRenderUI::Tick_UI()
{
    RenderUI::Tick_UI();
    

    Ptr<CFlipbookRender> flipbookRender = GetTargetObject()->FlipbookRender();
    
    // TickSelectingLevelBeginningInfo(flipbookRender);
    // RenderUI 에서 설정한 Render Transform 현재 AFlipbook에 저장하기
    if (ImGui::Button("Save Current RenderTransform to selected AFlipbook") || ShortCut(ImGuiKey_S))
    {
        if (Ptr<AFlipbook> TargetFlipbook = flipbookRender->GetFlipbook(m_CurSelectedCategory, m_SelectedFlipbookIdx))
        {
            TargetFlipbook->SetRenderOffset(flipbookRender->GetRenderOffset());
            TargetFlipbook->SetRenderScale(flipbookRender->GetRenderScale());
            DebugUtil::AddDebugLog(L"RenderTransform Saved to AFlipbook");
        }
    }
    ImGui::SameLine();
    ImGui::Text("ShortCut : s"); // TODO : 이 ShortCut 사용 후 제거하기
    

    ImGui::Separator(); ImGui::Separator(); ImGui::Separator();
    
    TickCheckNumberKey();
    
    TickSelectPreviewCategory(flipbookRender);
    TickAddNewCategory(flipbookRender);
    TickRemoveCategory(flipbookRender);

    ImGui::Separator(); ImGui::Separator(); ImGui::Separator();
    
    TickPreview(flipbookRender);
}

void FlipbookRenderUI::TickCheckNumberKey()
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)
    {
        m_NumShortCutTapped = -1;
        return;
    }
    
    if (ShortCut(ImGuiKey_1)) m_NumShortCutTapped = 1;
    else if (ShortCut(ImGuiKey_2)) m_NumShortCutTapped = 2;
    else if (ShortCut(ImGuiKey_3)) m_NumShortCutTapped = 3;
    else if (ShortCut(ImGuiKey_4)) m_NumShortCutTapped = 4;
    else if (ShortCut(ImGuiKey_5)) m_NumShortCutTapped = 5;
    else if (ShortCut(ImGuiKey_6)) m_NumShortCutTapped = 6;
    else if (ShortCut(ImGuiKey_7)) m_NumShortCutTapped = 7;
    else if (ShortCut(ImGuiKey_8)) m_NumShortCutTapped = 8;
    else if (ShortCut(ImGuiKey_9)) m_NumShortCutTapped = 9;
    else m_NumShortCutTapped = -1;
    
    if (ShortCut(ImGuiKey_Q)) m_CategoryShortCutTapped = 0;
    else if (ShortCut(ImGuiKey_W)) m_CategoryShortCutTapped = 1;
    else if (ShortCut(ImGuiKey_E)) m_CategoryShortCutTapped = 2;
    else if (ShortCut(ImGuiKey_R)) m_CategoryShortCutTapped = 3;
    else if (ShortCut(ImGuiKey_T)) m_CategoryShortCutTapped = 4;
    else if (ShortCut(ImGuiKey_Y)) m_CategoryShortCutTapped = 5;
    else if (ShortCut(ImGuiKey_U)) m_CategoryShortCutTapped = 6;
    else if (ShortCut(ImGuiKey_I)) m_CategoryShortCutTapped = 7;
    else if (ShortCut(ImGuiKey_O)) m_CategoryShortCutTapped = 8;
    else if (ShortCut(ImGuiKey_P)) m_CategoryShortCutTapped = 9;
    else m_CategoryShortCutTapped = -1;
}

void FlipbookRenderUI::TickSelectingLevelBeginningInfo(const Ptr<CFlipbookRender>& _FlipbookRender)
{
    ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader("ComponentSelectedCategory", ImGuiTreeNodeFlags_None))
    {
        string CategoryTitleName    = "<REAL_SELECTED_CATEGORY>";
        float windowWidth           = ImGui::GetWindowSize().x;
        float textWidth             = ImGui::CalcTextSize(CategoryTitleName.c_str()).x;
        
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(CategoryTitleName.c_str());

        //Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
        //const int curLayer = m_TargetObject->GetLayerIdx();
        

        if (ImGui::BeginTable("##Category", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
        {
            for (auto it = _FlipbookRender->m_mapCategoryFlipbooks.begin(); it != _FlipbookRender->m_mapCategoryFlipbooks.end(); ++it)
            {
                wstring Name = it->first;
                string strCategoryName = string(Name.begin(), Name.end());

                ImGui::TableNextColumn();

                const string CurSelectedStr = string(_FlipbookRender->m_CurSelectedCategory.begin(), _FlipbookRender->m_CurSelectedCategory.end()); 
                
                const bool selected = CurSelectedStr == strCategoryName;
                
                if (ImGui::Selectable(strCategoryName.c_str(), selected))
                    _FlipbookRender->SetCurrentCategory(wstring(strCategoryName.begin(), strCategoryName.end()));
            }

            ImGui::EndTable();
        }
    }
    
    ImGui::SeparatorText("");
}

void FlipbookRenderUI::TickPreview(const Ptr<CFlipbookRender>& _FlipbookRender)
{
    // 아무 카테고리도 없는 Flipbook Renderer
    if (_FlipbookRender->m_mapCategoryFlipbooks.empty()) return;
    
    // 현재 선택된 카테고리가 없는 경우
    if (m_CurSelectedCategory.empty()) return;
    
    ImGui::Separator();
    string Temp = string(m_CurSelectedCategory.begin(), m_CurSelectedCategory.end()) + " : Flipbook List ";
    ImGui::Text(Temp.c_str());

    ImGui::Button("Drop Flipbook To Append", ImVec2(240.f, 36.f));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content"))
            TryAppendFromPayload(payload, _FlipbookRender);
        ImGui::EndDragDropTarget();
    }

    DrawFlipbookList(_FlipbookRender);

    ImGui::Spacing();
    ImGui::Separator();
    DrawPreviewSection(_FlipbookRender);
}

void FlipbookRenderUI::TickSelectPreviewCategory(const Ptr<CFlipbookRender>& _FlipbookRender)
{
    ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader("Previewing Category", ImGuiTreeNodeFlags_None))
    {
        string CategoryTitleName    = "<CATEGORY>";
        float windowWidth           = ImGui::GetWindowSize().x;
        float textWidth             = ImGui::CalcTextSize(CategoryTitleName.c_str()).x;
        
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(CategoryTitleName.c_str());

        //Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
        //const int curLayer = m_TargetObject->GetLayerIdx();
        
        /*auto lambda_SelectCategory = [&](map<wstring, vector<Ptr<AFlipbook>>>::iterator it)
        {
            wstring Name = it->first;
            string strCategoryName = string(Name.begin(), Name.end());

            ImGui::TableNextColumn();

            const string CurSelectedStr = string(m_CurSelectedCategory.begin(), m_CurSelectedCategory.end()); 
                
            const bool selected = CurSelectedStr == strCategoryName;
                
            if (ImGui::Selectable(strCategoryName.c_str(), selected))
                m_CurSelectedCategory = wstring(strCategoryName.begin(), strCategoryName.end());
        };*/
        
        
        if (ImGui::BeginTable("##Category2", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
        {
            for (map<wstring, vector<Ptr<AFlipbook>>>::iterator it = _FlipbookRender->m_mapCategoryFlipbooks.begin(); it != _FlipbookRender->m_mapCategoryFlipbooks.end(); ++it)
            {
                wstring Name = it->first;
                string strCategoryName = string(Name.begin(), Name.end());

                ImGui::TableNextColumn();

                const string CurSelectedStr = string(m_CurSelectedCategory.begin(), m_CurSelectedCategory.end()); 
                
                const bool selected = CurSelectedStr == strCategoryName;
                
                if (ImGui::Selectable(strCategoryName.c_str(), selected))
                {
                    m_CurSelectedCategory = wstring(strCategoryName.begin(), strCategoryName.end());
                    if (m_SelectedFlipbookIdx == -1) m_SelectedFlipbookIdx = 0;
                    _FlipbookRender->SetCurrentCategory(m_CurSelectedCategory, m_SelectedFlipbookIdx, _FlipbookRender->m_CurAnimatingSpriteIdx);
                }
            }

            ImGui::EndTable();
        }

        if (m_CategoryShortCutTapped != -1 && m_CategoryShortCutTapped < _FlipbookRender->m_mapCategoryFlipbooks.size())
        {
            int index{};
            for (auto it = _FlipbookRender->m_mapCategoryFlipbooks.begin(); it != _FlipbookRender->m_mapCategoryFlipbooks.end(); ++it)
            {
                wstring Name = it->first;
                string strCategoryName = string(Name.begin(), Name.end());

                ImGui::TableNextColumn();

                const string CurSelectedStr = string(m_CurSelectedCategory.begin(), m_CurSelectedCategory.end()); 
                
                if (index == m_CategoryShortCutTapped)
                {
                    m_CategoryShortCutTapped = -1;
                    m_CurSelectedCategory = wstring(strCategoryName.begin(), strCategoryName.end());
                    if (m_SelectedFlipbookIdx == -1) m_SelectedFlipbookIdx = 0;
                    _FlipbookRender->SetCurrentCategory(m_CurSelectedCategory, m_SelectedFlipbookIdx, _FlipbookRender->m_CurAnimatingSpriteIdx);
                    break;
                }
                ++index;
            }
        }
    }
}

void FlipbookRenderUI::TickAddNewCategory(const Ptr<CFlipbookRender>& _FlipbookRender)
{
    ImGui::SetNextItemWidth(400.0f);
    ImGui::InputText("New Category", m_NewCategoryNameBuf, IM_ARRAYSIZE(m_NewCategoryNameBuf));
    ImGui::SameLine();

    if (!ImGui::Button("Add Category"))
        return;

    string newName = m_NewCategoryNameBuf;
    if (newName.empty())
        return;

    wstring newWName(newName.begin(), newName.end());

    // 이미 있는 이름이라면 처리 x
    if (_FlipbookRender->m_mapCategoryFlipbooks.contains(newWName))
    {
        m_NewCategoryNameBuf[0] = '\0';
        return;
    }
    
    // 공백 문자열이라면 처리 x
    if (m_NewCategoryNameBuf[0] == '\0') return;

    // 신규 카테고리 생성
    _FlipbookRender->m_mapCategoryFlipbooks.insert(make_pair(newWName, vector<Ptr<AFlipbook>>{}));
    m_NewCategoryNameBuf[0] = '\0';
}

void FlipbookRenderUI::TickRemoveCategory(const Ptr<CFlipbookRender>& _FlipbookRender)
{
    const bool canRemove = !m_CurSelectedCategory.empty() && _FlipbookRender->m_mapCategoryFlipbooks.contains(m_CurSelectedCategory);

    ImGui::BeginDisabled(!canRemove);

    if (ImGui::Button("Remove Selected Category"))
    {
        Ptr<ConfirmUI> pUI = dynamic_cast<ConfirmUI*>(EditorMgr::GetInst()->FindUI("ConfirmUI").Get());
        assert(pUI.Get());

        pUI->SetWarningText("Are you sure you want to remove this selected Category?");
        pUI->AddDelegate(this, static_cast<DELEGATE_BOOL>(&FlipbookRenderUI::OnRemoveCategoryConfirmed));
        pUI->SetActive(true);
    }

    ImGui::EndDisabled();
}

void FlipbookRenderUI::TryAppendFromPayload(const ImGuiPayload* _Payload, const Ptr<CFlipbookRender>& _FlipbookRender)
{
    if (!_Payload || !_FlipbookRender) return;

    const int prevCount = _FlipbookRender->GetCategoryFlipbookCount(m_CurSelectedCategory);
    int added = 0;

    auto TryAddOne = [&](DWORD_PTR _RawData)
    {
        Ptr<Asset> asset = reinterpret_cast<Asset*>(_RawData);
        if (asset && asset->GetType() == ASSET_TYPE::FLIPBOOK)
        {
            _FlipbookRender->AddFlipbook(m_CurSelectedCategory, static_cast<AFlipbook*>(asset.Get()));
            ++added;
        }
    };

    if (TreeUI::IsPayloadMultiData(_Payload))
    {
        const DWORD_PTR* data = static_cast<const DWORD_PTR*>(_Payload->Data);
        const int payloadCount = _Payload->DataSize / sizeof(DWORD_PTR);

        for (int i = 0; i < payloadCount; ++i)
            TryAddOne(data[i]);
    }
    else
    {
        DWORD_PTR data = *static_cast<const DWORD_PTR*>(_Payload->Data);
        TryAddOne(data);
    }

    if (added > 0 && m_SelectedFlipbookIdx < 0)
        m_SelectedFlipbookIdx = prevCount;
}

void FlipbookRenderUI::DrawFlipbookList(const Ptr<CFlipbookRender>& _FlipbookRender)
{
    const int count = _FlipbookRender->GetCategoryFlipbookCount(m_CurSelectedCategory);

    if (count <= 0)
    {
        m_SelectedFlipbookIdx = -1;
        ImGui::Text("No Flipbook");
        return;
    }

    if (m_SelectedFlipbookIdx < 0 || m_SelectedFlipbookIdx >= count)
        m_SelectedFlipbookIdx = 0;

    ImGui::Text("Count : %d", count);

    int removeIndex = -1;
    int swapA = -1;
    int swapB = -1;

    if (ImGui::BeginTable("##FlipbookRenderList", 3,
        ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.f);
        ImGui::TableSetupColumn("Flipbook");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 190.f);
        ImGui::TableHeadersRow();

        for (int i = 0; i < count; ++i)
        {
            Ptr<AFlipbook> flipbook = _FlipbookRender->GetFlipbook(m_CurSelectedCategory, i);
            string key = flipbook ? string(flipbook->GetKey().begin(), flipbook->GetKey().end()) : "None";

            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", i);

            ImGui::TableSetColumnIndex(1);
            if (ImGui::Selectable(key.c_str(), m_SelectedFlipbookIdx == i, ImGuiSelectableFlags_AllowOverlap) || m_NumShortCutTapped - 1 == i)
            {
                m_SelectedFlipbookIdx = i;
                _FlipbookRender->Stop(m_CurSelectedCategory, m_SelectedFlipbookIdx, 2); // TODO : 2 지우기
                m_NumShortCutTapped = -1;
            }
            
            ImGui::TableSetColumnIndex(2);

            ImGui::BeginDisabled(i == 0);
            if (ImGui::SmallButton("Up"))
            {
                swapA = i;
                swapB = i - 1;
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(i == count - 1);
            if (ImGui::SmallButton("Down"))
            {
                swapA = i;
                swapB = i + 1;
            }
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::SmallButton("Remove"))
                removeIndex = i;

            ImGui::PopID();

            if (removeIndex != -1 || swapA != -1)
                break;
        }

        ImGui::EndTable();
    }

    if (swapA != -1 && swapB != -1)
    {
        if (_FlipbookRender->SwapFlipbook(m_CurSelectedCategory, swapA, swapB))
        {
            if (m_SelectedFlipbookIdx == swapA) m_SelectedFlipbookIdx = swapB;
            else if (m_SelectedFlipbookIdx == swapB) m_SelectedFlipbookIdx = swapA;
        }
    }

    if (removeIndex != -1 && _FlipbookRender->RemoveFlipbook(m_CurSelectedCategory, removeIndex))
    {
        const int newCount = _FlipbookRender->GetCategoryFlipbookCount(m_CurSelectedCategory);

        if (newCount <= 0)
        {
            m_SelectedFlipbookIdx = -1;
            m_PreviewTarget = nullptr;
        }
        else
        {
            if (m_SelectedFlipbookIdx > removeIndex) --m_SelectedFlipbookIdx;
            else if (m_SelectedFlipbookIdx == removeIndex)
                m_SelectedFlipbookIdx = min(removeIndex, newCount - 1);
        }
    }
}

void FlipbookRenderUI::DrawPreviewSection(const Ptr<CFlipbookRender>& _FlipbookRender)
{
    ImGui::Text("Preview");

    const int count = _FlipbookRender->GetCategoryFlipbookCount(m_CurSelectedCategory);
    if (count <= 0)
    {
        ImGui::Text("No Flipbook");
        return;
    }

    if (m_SelectedFlipbookIdx < 0 || m_SelectedFlipbookIdx >= count)
        m_SelectedFlipbookIdx = 0;

    Ptr<AFlipbook> flipbook = _FlipbookRender->GetFlipbook(m_CurSelectedCategory, m_SelectedFlipbookIdx);
    if (!flipbook)
    {
        ImGui::Text("Selected Flipbook is None");
        return;
    }

    string key = string(flipbook->GetKey().begin(), flipbook->GetKey().end());
    ImGui::Text("Selected");
    ImGui::SameLine(120.f);
    ImGui::InputText("##SelectedFlipbook", key.data(), key.length() + 1, ImGuiInputTextFlags_ReadOnly);

    if (m_PreviewTarget != flipbook.Get())
    {
        m_PreviewTarget = flipbook.Get();
        m_PreviewCurFrame = 0;
        m_PreviewAccTime = 0.f;
        // m_PreviewPlaying = true;
    }

    const int spriteCount = static_cast<int>(flipbook->GetSpriteCount());
    if (spriteCount <= 0)
    {
        ImGui::Text("Empty Flipbook");
        return;
    }

    if (ImGui::Button(m_PreviewPlaying ? "Pause##FlipbookPreview" : "Play##FlipbookPreview"))
        m_PreviewPlaying = !m_PreviewPlaying;

    ImGui::SameLine();
    if (ImGui::Button("Reset##FlipbookPreview"))
    {
        m_PreviewCurFrame = 0;
        m_PreviewAccTime = 0.f;
    }

    ImGui::SameLine();
    ImGui::Checkbox("Loop##FlipbookPreview", &m_PreviewLoop);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(90.f);
    ImGui::InputFloat("FPS##FlipbookPreview", &m_PreviewFPS, 1.f, 5.f, "%.1f");
    m_PreviewFPS = max(m_PreviewFPS, 0.1f);

    int frame = m_PreviewCurFrame;
    ImGui::BeginDisabled(m_PreviewPlaying);
    if (ImGui::SliderInt("Frame##FlipbookPreview", &frame, 0, spriteCount - 1))
    {
        m_PreviewCurFrame = frame;
        m_PreviewAccTime = 0.f;
    }
    ImGui::EndDisabled();

    if (m_PreviewCurFrame >= spriteCount)
        m_PreviewCurFrame = 0;

    if (m_PreviewPlaying)
    {
        const float frameLimit = 1.f / m_PreviewFPS;
        m_PreviewAccTime += E_DT;

        while (m_PreviewAccTime >= frameLimit)
        {
            m_PreviewAccTime -= frameLimit;
            ++m_PreviewCurFrame;

            if (m_PreviewCurFrame >= spriteCount)
            {
                if (m_PreviewLoop) m_PreviewCurFrame = 0;
                else
                {
                    m_PreviewCurFrame = spriteCount - 1;
                    m_PreviewPlaying = false;
                    break;
                }
            }
        }
    }

    ImGui::Text("Frame %d / %d", m_PreviewCurFrame, spriteCount - 1);
    DrawSpritePreview(flipbook->GetSprite(m_PreviewCurFrame), 220.f);
}

void FlipbookRenderUI::DrawSpritePreview(const Ptr<ASprite>& _Sprite, float _MaxPreviewSize)
{
    if (!_Sprite)
    {
        ImGui::Text("Empty Frame");
        return;
    }

    Ptr<ATexture> atlas = _Sprite->GetAtlas();
    if (!atlas)
    {
        ImGui::Text("No Atlas");
        return;
    }

    Vec2 leftTopUV = _Sprite->GetLeftTopUV();
    Vec2 sliceUV = _Sprite->GetSliceUV();
    Vec2 bgUV = _Sprite->GetBackgroundUV();
    Vec2 offsetUV = _Sprite->GetOffsetUV();

    if (bgUV.x <= 0.f || bgUV.y <= 0.f)
        bgUV = sliceUV;

    Vec2 leftTop = leftTopUV + sliceUV * 0.5f - bgUV * 0.5f;
    Vec2 uv0 = leftTop - offsetUV;
    Vec2 uv1 = leftTop + bgUV - offsetUV;

    const float previewW = bgUV.x * atlas->GetWidth();
    const float previewH = bgUV.y * atlas->GetHeight();
    if (previewW <= 0.f || previewH <= 0.f)
    {
        ImGui::Text("Invalid UV");
        return;
    }

    const float maxDim = max(previewW, previewH);
    const float scale = (maxDim > _MaxPreviewSize) ? (_MaxPreviewSize / maxDim) : 1.f;

    ImGui::ImageWithBg(
        atlas->GetSRV().Get(),
        ImVec2(previewW * scale, previewH * scale),
        Vec2(uv0.x, uv0.y),
        Vec2(uv1.x, uv1.y),
        ImVec4(0.f, 0.f, 0.f, 1.f)
    );
}

void FlipbookRenderUI::OnRemoveCategoryConfirmed(bool _Yes)
{
    if (!_Yes) return;
    
    Ptr<CFlipbookRender> FlipbookRender = GetTargetObject()->FlipbookRender();
    
    FlipbookRender->m_mapCategoryFlipbooks.erase(m_CurSelectedCategory);

    if (FlipbookRender->m_mapCategoryFlipbooks.empty()) m_CurSelectedCategory.clear();
    else
    {
        auto it = FlipbookRender->m_mapCategoryFlipbooks.begin();
        m_CurSelectedCategory = it->first;
    }    
}
