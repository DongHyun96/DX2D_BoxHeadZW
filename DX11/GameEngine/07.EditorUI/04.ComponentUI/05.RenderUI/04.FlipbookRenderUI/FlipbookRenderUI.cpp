#include "pch.h"
#include "FlipbookRenderUI.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/06.Component/RenderComponent/04.FlipbookRender/CFlipbookRender.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

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
    ComponentUI::Tick_UI();

    Ptr<CFlipbookRender> flipbookRender = GetTargetObject()->FlipbookRender();
    if (!flipbookRender)
        return;

    ImGui::Separator();
    ImGui::Text("Flipbook List");

    ImGui::Button("Drop Flipbook To Append", ImVec2(240.f, 36.f));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content"))
            TryAppendFromPayload(payload, flipbookRender);
        ImGui::EndDragDropTarget();
    }

    DrawFlipbookList(flipbookRender);

    ImGui::Spacing();
    ImGui::Separator();
    DrawPreviewSection(flipbookRender);
}

void FlipbookRenderUI::TryAppendFromPayload(const ImGuiPayload* _Payload, const Ptr<CFlipbookRender>& _FlipbookRender)
{
    if (!_Payload || !_FlipbookRender) return;

    const int prevCount = static_cast<int>(_FlipbookRender->GetFlipbookCount());
    int added = 0;

    auto TryAddOne = [&](DWORD_PTR _RawData)
    {
        Ptr<Asset> asset = reinterpret_cast<Asset*>(_RawData);
        if (asset && asset->GetType() == ASSET_TYPE::FLIPBOOK)
        {
            _FlipbookRender->AddFlipbook(static_cast<AFlipbook*>(asset.Get()));
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
    const int count = static_cast<int>(_FlipbookRender->GetFlipbookCount());

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
            Ptr<AFlipbook> flipbook = _FlipbookRender->GetFlipbook(i);
            string key = flipbook ? string(flipbook->GetKey().begin(), flipbook->GetKey().end()) : "None";

            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", i);

            ImGui::TableSetColumnIndex(1);
            if (ImGui::Selectable(key.c_str(), m_SelectedFlipbookIdx == i, ImGuiSelectableFlags_AllowOverlap))
                m_SelectedFlipbookIdx = i;

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
        if (_FlipbookRender->SwapFlipbook(swapA, swapB))
        {
            if (m_SelectedFlipbookIdx == swapA) m_SelectedFlipbookIdx = swapB;
            else if (m_SelectedFlipbookIdx == swapB) m_SelectedFlipbookIdx = swapA;
        }
    }

    if (removeIndex != -1 && _FlipbookRender->RemoveFlipbook(removeIndex))
    {
        const int newCount = static_cast<int>(_FlipbookRender->GetFlipbookCount());

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

    const int count = static_cast<int>(_FlipbookRender->GetFlipbookCount());
    if (count <= 0)
    {
        ImGui::Text("No Flipbook");
        return;
    }

    if (m_SelectedFlipbookIdx < 0 || m_SelectedFlipbookIdx >= count)
        m_SelectedFlipbookIdx = 0;

    Ptr<AFlipbook> flipbook = _FlipbookRender->GetFlipbook(m_SelectedFlipbookIdx);
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
        m_PreviewPlaying = true;
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