#include "pch.h"
#include "TileRenderUI.h"

#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

TileRenderUI::TileRenderUI()
    : RenderUI(COMPONENT_TYPE::TILE_RENDER, "TileRenderUI")
{
    m_ComponentTitle = "Tile Renderer";
}

TileRenderUI::~TileRenderUI()
{
}

void TileRenderUI::Tick_UI()
{
    RenderUI::Tick_UI();

    ImGui::Separator();
    ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader("TileMap Preview", ImGuiTreeNodeFlags_None))
        RenderPreview();
    
    ATexture* DecalAtlas = GetTargetObject()->TileRender()->GetDecalAtlas();
    ImTextureRef SRV = DecalAtlas ? DecalAtlas->GetSRV().Get() : nullptr;

    ImGui::Text("Current Decal Atlas : ");
    
    ImGui::ImageWithBg
    (
        SRV,
        ImVec2(200, 200),
        Vec2(0.f, 0.f), Vec2(1.f, 1.f),
        ImVec4(0.f, 0.f, 0.f, 1.f)
    );
    
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content"))
        {
            if (!TreeUI::IsPayloadMultiData(Payload))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                if (ASSET_TYPE::TEXTURE == pAsset->GetType())
                    GetTargetObject()->TileRender()->SetDecalAtlas(static_cast<ATexture*>(pAsset.Get()));
            }
        }
        
        ImGui::EndDragDropTarget();
    }

    ImGui::Spacing();
    
    if (ImGui::Button("Clear DecalAtlas"))
        GetTargetObject()->TileRender()->SetDecalAtlas(nullptr);
}

void TileRenderUI::RenderPreview()
{
    ImGui::Text("Selected TileMap (Drop tilemap to change)");

    Ptr<CTileRender> tileRender = GetTargetObject()->TileRender();
    Ptr<ATileMap> tile = tileRender ? tileRender->GetTileMap() : nullptr;

    const ImVec2 previewSize(240.f, 240.f);
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##TileMapPreview", previewSize);
    ImVec2 p1(p0.x + previewSize.x, p0.y + previewSize.y);

    // 기본 배경(검정)
    dl->AddRectFilled(p0, p1, IM_COL32(0, 0, 0, 255));

    // 드래그 드롭으로 TileMap 세팅
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content"))
        {
            if (!TreeUI::IsPayloadMultiData(payload))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(payload->Data);
                Ptr<Asset> asset = reinterpret_cast<Asset*>(data);

                if (asset && asset->GetType() == ASSET_TYPE::TILEMAP)
                    tileRender->SetTileMap(static_cast<ATileMap*>(asset.Get()));
            }
        }
        ImGui::EndDragDropTarget();
    }

    // 실제 프리뷰 렌더
    if (tile)
    {
        UINT row = tile->GetRow();
        UINT col = tile->GetCol();

        if (row > 0 && col > 0)
        {
            Vec2 ts = tile->GetTileSize();
            if (ts.x <= 0) ts.x = 32.f;
            if (ts.y <= 0) ts.y = 32.f;

            float mapW = ts.x * col;
            float mapH = ts.y * row;

            float scale = min(previewSize.x / mapW, previewSize.y / mapH);
            float cellW = ts.x * scale;
            float cellH = ts.y * scale;

            ImVec2 start(
                p0.x + (previewSize.x - cellW * col) * 0.5f,
                p0.y + (previewSize.y - cellH * row) * 0.5f
            );

            const auto& sprites = tile->GetSprites();

            for (UINT r = 0; r < row; ++r)
            {
                for (UINT c = 0; c < col; ++c)
                {
                    int idx = static_cast<int>(r * col + c);
                    if (idx < 0 || idx >= static_cast<int>(sprites.size())) continue;

                    Ptr<ASprite> sp = sprites[idx];
                    ImVec2 q0(start.x + c * cellW, start.y + r * cellH);
                    ImVec2 q1(q0.x + cellW, q0.y + cellH);

                    // grid
                    dl->AddRect(q0, q1, IM_COL32(60, 60, 60, 255));

                    if (sp && sp->GetAtlas())
                    {
                        Vec2 lt = sp->GetLeftTopUV();
                        Vec2 sl = sp->GetSliceUV();
                        ImVec2 uv0(lt.x, lt.y);
                        ImVec2 uv1(lt.x + sl.x, lt.y + sl.y);

                        dl->AddImage(
                            sp->GetAtlas()->GetSRV().Get(),
                            q0, q1, uv0, uv1
                        );
                    }
                }
            }
        }
    }

    ImGui::Spacing();       
}
