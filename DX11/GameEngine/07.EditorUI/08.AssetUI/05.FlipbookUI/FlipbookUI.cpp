#include "pch.h"
#include "FlipbookUI.h"

#include <algorithm>

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"

FlipbookUI::FlipbookUI()
    : AssetUI(ASSET_TYPE::FLIPBOOK)
{
}

FlipbookUI::~FlipbookUI()
{
}

void FlipbookUI::Tick_UI()
{
    AssetUI::Tick_UI();

    ImGui::Separator();

    Ptr<AFlipbook> pFlipbook = static_cast<AFlipbook*>(GetTargetAsset().Get());
    if (!pFlipbook)
    {
        SaveButton();
        return;
    }

    ImGui::Text("Sprite Count");
    ImGui::SameLine(130);
    ImGui::Text("%u", pFlipbook->GetSpriteCount());

    DrawPreviewSection(pFlipbook);

    int removeIndex = -1;
    int insertIndex = -1;
    vector<Ptr<ASprite>> insertSprites{};

    DrawSpriteTable(pFlipbook, removeIndex, insertIndex, insertSprites);
    ApplySpriteTableEdits(pFlipbook, removeIndex, insertIndex, insertSprites);

    DrawUVEditor(pFlipbook);
    DrawAppendSection(pFlipbook);

    SaveButton();
}

float FlipbookUI::DrawSpritePreview
(
    const Ptr<ASprite>& _Sprite,
    float               _MaxPreviewSize,
    bool                _EnableZoom,
    float               _CurrentZoomFactor    
)
{
    if (!_Sprite)
    {
        ImGui::Text("Empty");
        return _CurrentZoomFactor;
    }

    Ptr<ATexture> pAtlas = _Sprite->GetAtlas();
    if (!pAtlas)
    {
        ImGui::Text("No Atlas");
        return _CurrentZoomFactor;
    }

    Vec2 LeftTopUV   = _Sprite->GetLeftTopUV();
    Vec2 SliceUV     = _Sprite->GetSliceUV();
    Vec2 BgUV        = _Sprite->GetBackgroundUV();
    Vec2 OffsetUV    = _Sprite->GetOffsetUV();

    if (BgUV.x <= 0.f || BgUV.y <= 0.f)
        BgUV = SliceUV;

    Vec2 LeftTop = LeftTopUV + SliceUV * 0.5f - BgUV * 0.5f;
    Vec2 UV0     = LeftTop - OffsetUV;
    Vec2 UV1     = LeftTop + BgUV - OffsetUV;

    const float PreviewW = BgUV.x * pAtlas->GetWidth();
    const float PreviewH = BgUV.y * pAtlas->GetHeight();

    if (PreviewW <= 0.f || PreviewH <= 0.f)
    {
        ImGui::Text("Invalid UV");
        return _CurrentZoomFactor;
    }

    const float MaxDim = max(PreviewW, PreviewH);
    const float Scale = (MaxDim > _MaxPreviewSize) ? (_MaxPreviewSize / MaxDim) : 1.f;
    
    ImVec2 ImageSize{};
    if (_EnableZoom)
    {
        ImGui::SliderFloat("ZoomFactor##Slider", &_CurrentZoomFactor, 0.1f, 10.0f, "%.1f");
        ImageSize = ImVec2(PreviewW * Scale * _CurrentZoomFactor, PreviewH * Scale * _CurrentZoomFactor);
    }
    else ImageSize = ImVec2(PreviewW * Scale, PreviewH * Scale);
    
    ImGui::ImageWithBg
    (
        pAtlas->GetSRV().Get(),
        ImageSize,
        Vec2(UV0.x, UV0.y),
        Vec2(UV1.x, UV1.y),
        ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    
    return _CurrentZoomFactor;
}

void FlipbookUI::DrawPreviewSection(const Ptr<AFlipbook>& _Flipbook)
{
    if (m_PreviewTarget != _Flipbook.Get())
    {
        m_PreviewTarget = _Flipbook.Get();
        m_PreviewCurFrame = 0;
        m_PreviewAccTime = 0.f;
        m_PreviewPlaying = true;
    }

    const int spriteCount = static_cast<int>(_Flipbook->GetSpriteCount());

    ImGui::Spacing();
    ImGui::Text("Preview");

    if (spriteCount <= 0)
    {
        ImGui::Text("Empty Flipbook");
        ImGui::Separator();
        return;
    }

    if (ImGui::Button(m_PreviewPlaying ? "Pause" : "Play"))
        m_PreviewPlaying = !m_PreviewPlaying;

    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        m_PreviewCurFrame = 0;
        m_PreviewAccTime = 0.f;
    }

    ImGui::SameLine();
    ImGui::Checkbox("Loop", &m_PreviewLoop);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(80.f);
    ImGui::InputFloat("FPS", &m_PreviewFPS, 1.f, 5.f, "%.1f");
    m_PreviewFPS = max(m_PreviewFPS, 0.1f);
    
    int frame = m_PreviewCurFrame;
    ImGui::BeginDisabled(m_PreviewPlaying);
    if (ImGui::SliderInt("Frame", &frame, 0, spriteCount - 1))
    {
        m_PreviewCurFrame = frame;
        m_PreviewAccTime = 0.f;
    }
    ImGui::EndDisabled();

    if (m_PreviewCurFrame >= spriteCount) m_PreviewCurFrame = 0;

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
                if (m_PreviewLoop)
                    m_PreviewCurFrame = 0;
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
    static float ZoomFactor = 1.f;
    ImGui::PushID("PreviewSection");
    ZoomFactor = DrawSpritePreview(_Flipbook->GetSprite(m_PreviewCurFrame), 200.f, true, ZoomFactor);
    ImGui::PopID();

    ImGui::Separator();
}

void FlipbookUI::DrawSpriteTable
(
    const Ptr<AFlipbook>&   _Flipbook,
    int&                    _RemoveIndex,
    int&                    _InsertIndex,
    vector<Ptr<ASprite>>&   _InsertSprites
)
{
    if (ImGui::Button("ClearSprites"))
    {
        Ptr<ConfirmUI> pUI = dynamic_cast<ConfirmUI*>(EditorMgr::GetInst()->FindUI("ConfirmUI").Get());
        assert(pUI.Get());

        pUI->SetWarningText("Are you sure you want to clear all sprites from this Flipbook?");
        pUI->AddDelegate(this, static_cast<DELEGATE_BOOL>(&FlipbookUI::OnConfirmClearSprites));
        pUI->SetActive(true);
    }
    
    if (ImGui::BeginTable("##FlipbookSpriteTable", 4,
        ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.f);
        ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, 110.f);
        ImGui::TableSetupColumn("Sprite Key");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 260.f);
        ImGui::TableHeadersRow();

        const int count = static_cast<int>(_Flipbook->GetSpriteCount());
        for (int i = 0; i < count; ++i)
        {
            Ptr<ASprite> pSprite = _Flipbook->GetSprite(i);

            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", i);

            ImGui::TableSetColumnIndex(1);
            DrawSpritePreview(pSprite, 80.f);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content"))
                {
                    if (TreeUI::IsPayloadMultiData(Payload))
                    {
                        const DWORD_PTR* data = static_cast<const DWORD_PTR*>(Payload->Data);
                        const int payloadCount = Payload->DataSize / sizeof(DWORD_PTR);

                        for (int j = 0; j < payloadCount; ++j)
                        {
                            Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data[j]);
                            if (pAsset && pAsset->GetType() == ASSET_TYPE::SPRITE)
                                _Flipbook->SetSprite(i + j, static_cast<ASprite*>(pAsset.Get()));
                        }
                    }
                    else
                    {
                        DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                        Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                        if (pAsset && pAsset->GetType() == ASSET_TYPE::SPRITE)
                            _Flipbook->SetSprite(i, static_cast<ASprite*>(pAsset.Get()));
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::TableSetColumnIndex(2);
            string keyStr = "None";
            if (pSprite)
                keyStr = string(pSprite->GetKey().begin(), pSprite->GetKey().end());
            ImGui::InputText("##SpriteKey", keyStr.data(), keyStr.length() + 1, ImGuiInputTextFlags_ReadOnly);

            ImGui::TableSetColumnIndex(3);
            if (ImGui::SmallButton("Edit UV"))
                m_SelectedSpriteIdx = i;

            ImGui::SameLine();
            if (ImGui::Button("Remove"))
                _RemoveIndex = i;

            ImGui::SameLine();
            ImGui::Button("Insert Here");
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content"))
                {
                    _InsertSprites.clear();

                    if (TreeUI::IsPayloadMultiData(Payload))
                    {
                        const DWORD_PTR* data = static_cast<const DWORD_PTR*>(Payload->Data);
                        const int payloadCount = Payload->DataSize / sizeof(DWORD_PTR);

                        for (int j = 0; j < payloadCount; ++j)
                        {
                            Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data[j]);
                            if (pAsset && pAsset->GetType() == ASSET_TYPE::SPRITE)
                                _InsertSprites.push_back(static_cast<ASprite*>(pAsset.Get()));
                        }
                    }
                    else
                    {
                        DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                        Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                        if (pAsset && pAsset->GetType() == ASSET_TYPE::SPRITE)
                            _InsertSprites.push_back(static_cast<ASprite*>(pAsset.Get()));
                    }

                    if (!_InsertSprites.empty())
                        _InsertIndex = i;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PopID();

            if (_RemoveIndex != -1 || _InsertIndex != -1)
                break;
        }

        ImGui::EndTable();
    }
}

void FlipbookUI::ApplySpriteTableEdits
(
    const Ptr<AFlipbook>&       _Flipbook,
    int                         _RemoveIndex,
    int                         _InsertIndex,
    const vector<Ptr<ASprite>>& _InsertSprites
)
{
    if (_RemoveIndex != -1)
        _Flipbook->RemoveSprite(_RemoveIndex);

    if (_InsertIndex != -1)
    {
        int idx = _InsertIndex;
        for (const Ptr<ASprite>& sp : _InsertSprites)
        {
            _Flipbook->InsertSprite(idx, sp);
            ++idx;
        }
    }
}

void FlipbookUI::DrawUVEditor(const Ptr<AFlipbook>& _Flipbook)
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Sprite UV Editor");

    if (m_SelectedSpriteIdx < 0 || m_SelectedSpriteIdx >= static_cast<int>(_Flipbook->GetSpriteCount()))
    {
        ImGui::Text("Select a sprite and click 'Edit UV'.");
        return;
    }

    Ptr<ASprite> pEditSprite = _Flipbook->GetSprite(m_SelectedSpriteIdx);
    if (!pEditSprite)
    {
        ImGui::Text("Empty Sprite");
        return;
    }

    Ptr<ATexture> pAtlas = pEditSprite->GetAtlas();
    const bool isAtlas = pAtlas.Get();

    string editKeyStr = string(pEditSprite->GetKey().begin(), pEditSprite->GetKey().end());
    ImGui::Text("Sprite Key");
    ImGui::SameLine(120);
    ImGui::InputText("##EditSpriteKey", editKeyStr.data(), editKeyStr.length() + 1, ImGuiInputTextFlags_ReadOnly);

    static float ZoomFactor = 1.f;
    ImGui::PushID("UVEditorSection");
    ZoomFactor = DrawSpritePreview(pEditSprite, 200.f, true, ZoomFactor);
    ImGui::PopID();

    ImGui::BeginDisabled(!isAtlas);
    {
        if (isAtlas)
        {
            Vec2 bgUV = pEditSprite->GetBackgroundUV();
            Vec2 offUV = pEditSprite->GetOffsetUV();

            int bgPixel[2] =
            {
                static_cast<int>(bgUV.x * pAtlas->GetWidth()),
                static_cast<int>(bgUV.y * pAtlas->GetHeight())
            };

            if (ImGui::DragInt2("Background (px)", bgPixel, 1.0f))
            {
                bgPixel[0] = max(0, bgPixel[0]);
                bgPixel[1] = max(0, bgPixel[1]);

                pEditSprite->SetBackgroundUV
                (
                    Vec2(bgPixel[0] / pAtlas->GetWidth(), bgPixel[1] / pAtlas->GetHeight())
                );
            }
            
            int offPixel[2] =
            {
                static_cast<int>(offUV.x * pAtlas->GetWidth()),
                static_cast<int>(offUV.y * pAtlas->GetHeight())
            };

            if (ImGui::DragInt2("Offset (px)", offPixel, 1.0f))
            {
                //offPixel[0] = max(0, offPixel[0]);
                //offPixel[1] = max(0, offPixel[1]);
                
                offPixel[0] = offPixel[0];
                offPixel[1] = offPixel[1];

                pEditSprite->SetOffsetUV
                (
                    Vec2(offPixel[0] / pAtlas->GetWidth(), offPixel[1] / pAtlas->GetHeight())
                );
            }
        }
        else
        {
            int dummy[2] = { 0, 0 };
            ImGui::InputInt2("Background (px)", dummy);
            ImGui::InputInt2("Offset (px)", dummy);
        }
    }
    ImGui::EndDisabled();

    if (ImGui::Button("Save Sprite"))
        pEditSprite->SaveBySelfRelativePath();
    
    ImGui::SameLine(200);
    if (ImGui::Button("Close Edit UV"))
        m_SelectedSpriteIdx = -1;
}

void FlipbookUI::DrawAppendSection(const Ptr<AFlipbook>& _Flipbook)
{
    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Append Sprite");
    ImGui::Button("Drop Sprite To Append", ImVec2(200.f, 40.f));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content"))
        {
            if (TreeUI::IsPayloadMultiData(Payload))
            {
                const DWORD_PTR* data = static_cast<const DWORD_PTR*>(Payload->Data);
                const int payloadCount = Payload->DataSize / sizeof(DWORD_PTR);

                for (int i = 0; i < payloadCount; ++i)
                {
                    Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data[i]);
                    if (pAsset && pAsset->GetType() == ASSET_TYPE::SPRITE)
                        _Flipbook->AddSprite(static_cast<ASprite*>(pAsset.Get()));
                }
            }
            else
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                if (pAsset && pAsset->GetType() == ASSET_TYPE::SPRITE)
                    _Flipbook->AddSprite(static_cast<ASprite*>(pAsset.Get()));
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void FlipbookUI::OnConfirmClearSprites(bool _Yes)
{
    if (_Yes)
    {
        Ptr<AFlipbook> pFlipbook = static_cast<AFlipbook*>(GetTargetAsset().Get());
        pFlipbook->ClearSprites();
    }
}
