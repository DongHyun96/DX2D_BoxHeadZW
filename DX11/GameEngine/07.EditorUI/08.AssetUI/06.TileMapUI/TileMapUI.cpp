#include "pch.h"
#include "TileMapUI.h"

#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

TileMapUI::TileMapUI()
    : AssetUI(ASSET_TYPE::TILEMAP)
{
}

TileMapUI::~TileMapUI()
{
}

void TileMapUI::Tick_UI()
{
    AssetUI::Tick_UI();

    Ptr<ATileMap> tile = static_cast<ATileMap*>(GetTargetAsset().Get());
    if (!tile) return;

    // 타겟 바뀌면 UI 상태 동기화
    if (m_CachedTarget != tile.Get())
    {
        m_CachedTarget = tile.Get();
        SyncFromAsset(tile);
    }

    ImGui::Separator();

    // Row/Col/CellSize
    ImGui::Text("Row / Col");
    int r = m_Row, c = m_Col;
    ImGui::SetNextItemWidth(100.f);
    ImGui::InputInt("##Row", &r);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.f);
    ImGui::InputInt("##Col", &c);

    if (r != m_Row || c != m_Col)
        ResizeGrid(max(1, r), max(1, c));

    ImGui::Text("Cell Size");
    float cs[2] = { m_CellSize.x, m_CellSize.y };
    ImGui::SetNextItemWidth(200.f);
    if (ImGui::InputFloat2("##CellSize", cs))
        m_CellSize = Vec2(max(1.f, cs[0]), max(1.f, cs[1]));

    ImGui::Spacing();
    DrawPalette();

    ImGui::Spacing();
    if (!m_OpenDetachedCanvasWindow)
    {
        if (ImGui::Button("Open Detached TileMap Window"))
            m_OpenDetachedCanvasWindow = true;

        DrawTileCanvas();
    }
    else
    {
        if (ImGui::Button("Close Detached TileMap Window"))
            m_OpenDetachedCanvasWindow = false;

        ImGui::Text("TileMap canvas is opened in a detached window.");
    }

    ImGui::Spacing();
    if (ImGui::Button("Save TileMap"))
        BakeAndSaveTileMap();
    
    DrawDetachedTileCanvasWindow();
    DrawAtlasNameModal();
}

void TileMapUI::SyncFromAsset(const Ptr<ATileMap>& tile)
{
    if (!tile) return;

    m_Row = static_cast<int>(tile->GetRow());
    m_Col = static_cast<int>(tile->GetCol());

    if (m_Row <= 0) m_Row = 5;
    if (m_Col <= 0) m_Col = 5;

    m_CellSize = tile->GetTileSize();
    if (m_CellSize.x <= 0.f) m_CellSize.x = 32.f;
    if (m_CellSize.y <= 0.f) m_CellSize.y = 32.f;

    const auto& src = tile->GetSprites();
    m_WorkingCells.clear();
    m_WorkingCells.resize(m_Row * m_Col);

    const int copyCount = min(src.size(), m_WorkingCells.size());
    for (int i = 0; i < copyCount; ++i)
        m_WorkingCells[i] = src[i];

    m_LastPaintIdx = -1;
}

void TileMapUI::ResizeGrid(int newRow, int newCol)
{
    if (newRow <= 0) newRow = 1;
    if (newCol <= 0) newCol = 1;

    vector<Ptr<ASprite>> newCells;
    newCells.resize(newRow * newCol);

    // 겹치는 영역만 복사
    const int minRow = min(m_Row, newRow);
    const int minCol = min(m_Col, newCol);

    for (int r = 0; r < minRow; ++r)
    {
        for (int c = 0; c < minCol; ++c)
        {
            newCells[r * newCol + c] = m_WorkingCells[r * m_Col + c];
        }
    }

    m_Row = newRow;
    m_Col = newCol;
    m_WorkingCells.swap(newCells);
    m_LastPaintIdx = -1;
}

void TileMapUI::DrawPalette()
{
    ImGui::Text("Palette");

    if (ImGui::Button("Clear Palette"))
    {
        m_Palette.clear();
        m_SelectedPalette = -1;
    }

    ImGui::SameLine();

    ImGui::Button("Drop Sprite Here", ImVec2(200.f, 40.f));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content"))
        {
            if (TreeUI::IsPayloadMultiData(payload))
            {
                const DWORD_PTR* data = static_cast<const DWORD_PTR*>(payload->Data);
                int count = payload->DataSize / sizeof(DWORD_PTR);
                for (int i = 0; i < count; ++i)
                {
                    Ptr<Asset> a = reinterpret_cast<Asset*>(data[i]);
                    if (a && a->GetType() == ASSET_TYPE::SPRITE)
                        m_Palette.push_back(static_cast<ASprite*>(a.Get()));
                }
            }
            else
            {
                DWORD_PTR data = *static_cast<const DWORD_PTR*>(payload->Data);
                Ptr<Asset> a = reinterpret_cast<Asset*>(data);
                if (a && a->GetType() == ASSET_TYPE::SPRITE)
                    m_Palette.push_back(static_cast<ASprite*>(a.Get()));
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Spacing();

    if (m_SelectedPalette >= 0 && m_SelectedPalette < m_Palette.size())
    {
        Ptr<ASprite> sel = m_Palette[m_SelectedPalette];
        string key = sel ? string(sel->GetKey().begin(), sel->GetKey().end()) : "None";
        ImGui::Text("Selected: %s", key.c_str());
    }
    else
    {
        ImGui::Text("Selected: None");
    }

    ImGui::Separator();
    ImGui::BeginChild("PaletteGrid", ImVec2(0, 220), true);

    const float previewSize = 48.f;
    const float pad = 6.f;
    float avail = ImGui::GetContentRegionAvail().x;
    int columns = max(1, (int)(avail / (previewSize + pad)));

    ImDrawList* dl = ImGui::GetWindowDrawList();
    int removeIdx = -1;

    for (int i = 0; i < m_Palette.size(); ++i)
    {
        Ptr<ASprite> sp = m_Palette[i];
        ImGui::PushID(i);

        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##pal_btn", ImVec2(previewSize, previewSize));

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            m_SelectedPalette = i;

        // 우클릭 시 해당 Sprite 위에 팝업 버튼
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup("##pal_ctx");

        ImGui::SetNextWindowPos(
            ImVec2(cursor.x + previewSize * 0.5f, cursor.y + previewSize * 0.5f),
            ImGuiCond_Appearing,
            ImVec2(0.5f, 0.5f)
        );

        if (ImGui::BeginPopup("##pal_ctx"))
        {
            if (ImGui::Button("Delete", ImVec2(80.f, 0.f)))
            {
                removeIdx = i;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (sp && sp->GetAtlas())
        {
            Vec2 lt = sp->GetLeftTopUV();
            Vec2 sl = sp->GetSliceUV();
            ImVec2 uv0(lt.x, lt.y);
            ImVec2 uv1(lt.x + sl.x, lt.y + sl.y);

            dl->AddImage(
                sp->GetAtlas()->GetSRV().Get(),
                cursor,
                ImVec2(cursor.x + previewSize, cursor.y + previewSize),
                uv0,
                uv1
            );
        }
        else
        {
            dl->AddRectFilled(
                cursor,
                ImVec2(cursor.x + previewSize, cursor.y + previewSize),
                IM_COL32(40, 40, 40, 255)
            );
        }

        if (m_SelectedPalette == i)
        {
            dl->AddRect(
                cursor,
                ImVec2(cursor.x + previewSize, cursor.y + previewSize),
                IM_COL32(255, 200, 50, 255),
                0.f, 0, 2.0f
            );
        }
        else
        {
            dl->AddRect(
                cursor,
                ImVec2(cursor.x + previewSize, cursor.y + previewSize),
                IM_COL32(90, 90, 90, 255)
            );
        }

        if ((i + 1) % columns != 0)
            ImGui::SameLine(0.f, pad);

        ImGui::PopID();
    }

    ImGui::EndChild();

    if (removeIdx >= 0 && removeIdx < m_Palette.size())
    {
        m_Palette.erase(m_Palette.begin() + removeIdx);

        if (m_SelectedPalette == removeIdx) m_SelectedPalette = -1;
        else if (m_SelectedPalette > removeIdx) --m_SelectedPalette;
    }
}

void TileMapUI::DrawTileCanvas()
{
    ImGui::Text("TileMap");

    // 줌 컨트롤(버튼)
    ImGui::SetNextItemWidth(80.f);
    ImGui::InputFloat("Zoom", &m_Zoom, 0.1f, 0.5f, "%.2f");
    m_Zoom = max(m_MinZoom, min(m_Zoom, m_MaxZoom));
    ImGui::SameLine();
    if (ImGui::Button("Reset")) m_Zoom = 1.0f;

    ImGui::BeginChild("TileMapCanvas", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

    // 마우스 휠 줌 (캔버스 위에서만)
    if (ImGui::IsWindowHovered())
    {
        ImGuiIO& io = ImGui::GetIO();
        
        float wheel = ImGui::GetIO().MouseWheel;
        if (io.KeyCtrl && wheel != 0.f)
        {
            m_Zoom += wheel * 0.1f;
            m_Zoom = max(m_MinZoom, min(m_Zoom, m_MaxZoom));
        }
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();

    const float cellW = m_CellSize.x * m_Zoom;
    const float cellH = m_CellSize.y * m_Zoom;

    ImVec2 canvasSize(cellW * m_Col, cellH * m_Row);

    ImGui::InvisibleButton("##TileMapBtn", canvasSize,
        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

    bool hovered = ImGui::IsItemHovered();
    ImVec2 mouse = ImGui::GetIO().MousePos;

    if (hovered)
    {
        int col = static_cast<int>((mouse.x - origin.x) / cellW);
        int row = static_cast<int>((mouse.y - origin.y) / cellH);

        if (row >= 0 && row < m_Row && col >= 0 && col < m_Col)
        {
            int idx = row * m_Col + col;

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                if (idx != m_LastPaintIdx)
                {
                    Ptr<ASprite> sel = (m_SelectedPalette >= 0 && m_SelectedPalette < (int)m_Palette.size())
                        ? m_Palette[m_SelectedPalette]
                        : nullptr;
                    PaintCell(row, col, sel);
                    m_LastPaintIdx = idx;
                }
            }
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                if (idx != m_LastPaintIdx)
                {
                    EraseCell(row, col);
                    m_LastPaintIdx = idx;
                }
            }
            else
            {
                m_LastPaintIdx = -1;
            }
        }
    }
    else
    {
        m_LastPaintIdx = -1;
    }

    // 셀 렌더 + 그리드
    for (int r = 0; r < m_Row; ++r)
    {
        for (int c = 0; c < m_Col; ++c)
        {
            int idx = r * m_Col + c;
            ImVec2 p0(origin.x + c * cellW, origin.y + r * cellH);
            ImVec2 p1(p0.x + cellW, p0.y + cellH);

            dl->AddRect(p0, p1, IM_COL32(80, 80, 80, 255));

            Ptr<ASprite> sp = m_WorkingCells[idx];
            if (sp && sp->GetAtlas())
            {
                Vec2 lt = sp->GetLeftTopUV();
                Vec2 sl = sp->GetSliceUV();
                ImVec2 uv0(lt.x, lt.y);
                ImVec2 uv1(lt.x + sl.x, lt.y + sl.y);
                dl->AddImage(sp->GetAtlas()->GetSRV().Get(), p0, p1, uv0, uv1);
            }
        }
    }

    ImGui::EndChild();
}

void TileMapUI::DrawDetachedTileCanvasWindow()
{
    if (!m_OpenDetachedCanvasWindow)
        return;

    if (ImGui::Begin("TileMap Canvas##Detached", &m_OpenDetachedCanvasWindow))
        DrawTileCanvas();

    ImGui::End();
}

void TileMapUI::PaintCell(int row, int col, const Ptr<ASprite>& sp)
{
    int idx = row * m_Col + col;
    if (idx < 0 || idx >= (int)m_WorkingCells.size()) return;
    m_WorkingCells[idx] = sp;
}

void TileMapUI::EraseCell(int row, int col)
{
    PaintCell(row, col, nullptr);
}

void TileMapUI::BakeAndSaveTileMap()
{
    Ptr<ATileMap> tile = static_cast<ATileMap*>(GetTargetAsset().Get());
    if (!tile) return;

    tile->SetRowCol(m_Row, m_Col);
    tile->SetTileSize(m_CellSize);

    // 사용된 스프라이트 수집 (중복 제거)
    vector<Ptr<ASprite>> used;
    unordered_set<ASprite*> seen;
    for (auto& sp : m_WorkingCells)
    {
        if (!sp) continue;
        if (seen.insert(sp.Get()).second)
            used.push_back(sp);
    }

    // 공용 atlas 검사
    Ptr<ATexture> commonAtlas{};
    bool allCommon = true;
    for (auto& sp : used)
    {
        if (!sp || !sp->GetAtlas()) { allCommon = false; break; }
        if (!commonAtlas) commonAtlas = sp->GetAtlas();
        else if (commonAtlas != sp->GetAtlas()) { allCommon = false; break; }
    }

    if (allCommon)
    {
        tile->SetAtlas(commonAtlas);
        for (int r = 0; r < m_Row; ++r)
            for (int c = 0; c < m_Col; ++c)
                tile->SetSprite(r, c, m_WorkingCells[r * m_Col + c]);

        tile->SaveBySelfRelativePath();
        return;
    }
    else
    {
        // 공용 Atlas로 구성한 TileMap이 아닌 경우
        // 모달용 스냅샷 저장
        m_PendingCells      = m_WorkingCells;
        m_PendingRow        = m_Row;
        m_PendingCol        = m_Col;
        m_PendingCellSize   = m_CellSize;

        m_AtlasNameConflict = false;
        m_AtlasNameConflictMsg.clear();

        m_PendingAtlasSave = true;
        m_OpenAtlasNameModal = true;
        return; // 여기서 저장 중단 -> 모달에서 처리
    }
        
    

    /*
    // 공용 atlas 생성 + sprite 재생성
    Ptr<ATexture> newAtlas{};
    unordered_map<ASprite*, Ptr<ASprite>> remap;
    if (!BuildSharedAtlasIfNeeded(used, newAtlas, remap))
        return;

    tile->SetAtlas(newAtlas);
    for (int r = 0; r < m_Row; ++r)
    {
        for (int c = 0; c < m_Col; ++c)
        {
            Ptr<ASprite> sp = m_WorkingCells[r * m_Col + c];
            if (!sp) { tile->SetSprite(r, c, nullptr); continue; }

            auto it = remap.find(sp.Get());
            tile->SetSprite(r, c, (it != remap.end()) ? it->second : nullptr);
        }
    }

    tile->SaveBySelfRelativePath();
    */
}

bool TileMapUI::BuildSharedAtlasIfNeeded
(
    const vector<Ptr<ASprite>>&             usedSprites,
    Ptr<ATexture>&                          outAtlas,
    unordered_map<ASprite*, Ptr<ASprite>>&  outRemap,
    const wstring&                          baseName,
    AtlasCreateMode                         mode,
    vector<wstring>&                        outSpriteKeys,
    string&                                 outErrMsg
)
{
    outAtlas = nullptr;
    outRemap.clear();
    outSpriteKeys.clear();
    outErrMsg.clear();

    if (usedSprites.empty())
    {
        outErrMsg = "No sprites to build atlas.";
        return false;
    }
    if (baseName.empty())
    {
        outErrMsg = "Atlas name is empty.";
        return false;
    }

    const int pad = 1; // bleeding 방지용 패딩

    struct SrcInfo
    {
        Ptr<ASprite>  sp;
        Ptr<ATexture> atlas;
        int x, y, w, h;
        int texW, texH;
    };
    vector<SrcInfo> src;

    int cellW = 0;
    int cellH = 0;

    // 1) 소스 정보 수집 + 클램프
    for (auto& sp : usedSprites)
    {
        if (!sp)
        {
            outErrMsg = "Null sprite in used list.";
            return false;
        }

        Ptr<ATexture> at = sp->GetAtlas();
        if (!at)
        {
            outErrMsg = "Sprite has no atlas.";
            return false;
        }

        int texW = (int)at->GetWidth();
        int texH = (int)at->GetHeight();

        Vec2 lt = sp->GetLeftTopUV();
        Vec2 sl = sp->GetSliceUV();

        int x = (int)roundf(lt.x * texW);
        int y = (int)roundf(lt.y * texH);
        int w = (int)roundf(sl.x * texW);
        int h = (int)roundf(sl.y * texH);

        if (w <= 0 || h <= 0)
        {
            outErrMsg = "Invalid sprite slice size.";
            return false;
        }

        // 경계 클램프
        x = max(0, min(x, texW - 1));
        y = max(0, min(y, texH - 1));
        w = max(1, min(w, texW - x));
        h = max(1, min(h, texH - y));

        cellW = max(cellW, w + pad * 2);
        cellH = max(cellH, h + pad * 2);

        src.push_back({ sp, at, x, y, w, h, texW, texH });
    }

    const int n = (int)src.size();
    const int cols = (int)ceilf(sqrtf((float)n));
    const int rows = (int)ceilf((float)n / cols);
    const int atlasW = cellW * cols;
    const int atlasH = cellH * rows;

    if (atlasW <= 0 || atlasH <= 0)
    {
        outErrMsg = "Atlas size is zero.";
        return false;
    }

    // 2) 아틀라스 이미지 생성
    ScratchImage atlasImg;
    HRESULT hr = atlasImg.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, atlasW, atlasH, 1, 1);
    if (FAILED(hr))
    {
        outErrMsg = "Atlas image init failed.";
        return false;
    }

    const Image* dst = atlasImg.GetImage(0, 0, 0);
    memset(dst->pixels, 0, dst->rowPitch * dst->height);

    // 3) 복사 + 패딩
    for (int i = 0; i < n; ++i)
    {
        const auto& s = src[i];
        int dstX = (i % cols) * cellW + pad;
        int dstY = (i / cols) * cellH + pad;

        const Image* srcImg = s.atlas->GetImage().GetImage(0, 0, 0);
        if (!srcImg)
        {
            outErrMsg = "Source image is null.";
            return false;
        }

        // 포맷 변환 필요 시
        ScratchImage converted;
        if (srcImg->format != DXGI_FORMAT_R8G8B8A8_UNORM)
        {
            hr = Convert(*srcImg, DXGI_FORMAT_R8G8B8A8_UNORM, TEX_FILTER_DEFAULT, 0.0f, converted);
            if (FAILED(hr))
            {
                outErrMsg = "Convert failed.";
                return false;
            }
            srcImg = converted.GetImage(0, 0, 0);
        }

        Rect r = { (size_t)s.x, (size_t)s.y, (size_t)s.w, (size_t)s.h };
        hr = CopyRectangle(*srcImg, r, *dst, TEX_FILTER_DEFAULT, dstX, dstY);
        if (FAILED(hr))
        {
            outErrMsg = "CopyRectangle failed.";
            return false;
        }

        // edge padding (1px)
        if (pad > 0)
        {
            Rect top    = { (size_t)s.x, (size_t)s.y,           (size_t)s.w, 1 };
            Rect bottom = { (size_t)s.x, (size_t)(s.y + s.h - 1), (size_t)s.w, 1 };
            Rect left   = { (size_t)s.x, (size_t)s.y,           1, (size_t)s.h };
            Rect right  = { (size_t)(s.x + s.w - 1), (size_t)s.y, 1, (size_t)s.h };

            CopyRectangle(*srcImg, top,    *dst, TEX_FILTER_DEFAULT, dstX, dstY - 1);
            CopyRectangle(*srcImg, bottom, *dst, TEX_FILTER_DEFAULT, dstX, dstY + s.h);
            CopyRectangle(*srcImg, left,   *dst, TEX_FILTER_DEFAULT, dstX - 1, dstY);
            CopyRectangle(*srcImg, right,  *dst, TEX_FILTER_DEFAULT, dstX + s.w, dstY);
        }
    }

    // 4) 파일 저장
    wstring atlasFile = L"Texture\\" + baseName + L".png";
    wstring fullPath = CONTENT_PATH + atlasFile;

    hr = SaveToWICFile(*atlasImg.GetImage(0, 0, 0), TEXTURE_LOADING_FLAG,
        GetWICCodec(WIC_CODEC_PNG), fullPath.c_str());

    if (FAILED(hr))
    {
        outErrMsg = "SaveToWICFile failed.";
        return false;
    }

    // 5) Texture Asset 로드
    Ptr<Asset> found = AssetMgr::GetInst()->Find(ASSET_TYPE::TEXTURE, atlasFile, false);
    if (found)
    {
        outAtlas = static_cast<ATexture*>(found.Get());
        outAtlas->Load(fullPath); // 기존 텍스처 재로딩 (덮어쓴 PNG 반영)
    }
    else outAtlas = AssetMgr::GetInst()->Load<ATexture>(atlasFile, atlasFile);
        
    if (!outAtlas)
    {
        outErrMsg = "Atlas load failed.";
        return false;
    }

    // 6) 스프라이트 생성/재사용
    for (int i = 0; i < n; ++i)
    {
        int dstX = (i % cols) * cellW + pad;
        int dstY = (i / cols) * cellH + pad;

        int y = i / cols;
        int x = i % cols;

        wstring spriteKey = L"Sprite\\" + baseName + L"_" + to_wstring(y) + L"_" + to_wstring(x) + L".sprite";
        Ptr<Asset> asset = AssetMgr::GetInst()->Find(ASSET_TYPE::SPRITE, spriteKey, false);
        Ptr<ASprite> sp = dynamic_cast<ASprite*>(asset.Get());

        if (mode == AtlasCreateMode::NEWNAME && sp)
        {
            outErrMsg = "Sprite name already exists.";
            return false;
        }

        if (!sp)
        {
            Ptr<ASprite> newSp{};
            wstring spName = baseName + L"_" + to_wstring(y) + L"_" + to_wstring(x);
            AssetMgr::GetInst()->CreateNewAsset<ASprite>(spName, newSp);
            sp = newSp;
        }

        sp->SetAtlas(outAtlas);
        sp->SetLeftTopUV(Vec2((float)dstX / atlasW, (float)dstY / atlasH));
        sp->SetSliceUV(Vec2((float)src[i].w / atlasW, (float)src[i].h / atlasH));
        sp->SetBackgroundUV(sp->GetSliceUV());
        sp->SetOffsetUV(Vec2(0, 0));
        sp->SaveBySelfRelativePath();

        outRemap[src[i].sp.Get()] = sp;
        outSpriteKeys.push_back(sp->GetKey());
    }

    return true;
}

void TileMapUI::DrawAtlasNameModal()
{
    // 공통: m_PendingCells로 used 계산
    auto buildUsed = [&]()
    {
        vector<Ptr<ASprite>> used{};
        unordered_set<ASprite*> seen{};
        for (auto& sp : m_PendingCells)
        {
            if (!sp) continue;
            if (seen.insert(sp.Get()).second)
                used.push_back(sp);
        }
        return used;
    };
    
    if (m_OpenAtlasNameModal)
        ImGui::OpenPopup("New Atlas Name");

    if (ImGui::BeginPopupModal("New Atlas Name", &m_OpenAtlasNameModal, ImGuiWindowFlags_AlwaysAutoResize))
    {
        Ptr<ATileMap> Tile = static_cast<ATileMap*>(GetTargetAsset().Get());

        ImGui::Text("Enter atlas texture name (without extension).");
        ImGui::Spacing();

        m_AtlasNameInput.resize(40);
        ImGui::InputText("##AtlasNameInput", m_AtlasNameInput.data(), m_AtlasNameInput.length());
        m_AtlasNameInput.resize(strlen(m_AtlasNameInput.c_str()));

        if (m_AtlasNameConflict)
            ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), m_AtlasNameConflictMsg.c_str());

        ImGui::Spacing();

        // Reuse Existing
        if (Tile && Tile->UsesGeneratedAtlas() && !Tile->GetGeneratedAtlasBaseName().empty())
        {
            if (ImGui::Button("Reuse Existing Atlas"))
            {
                vector<Ptr<ASprite>> used = buildUsed();

                Ptr<ATexture> newAtlas{};
                unordered_map<ASprite*, Ptr<ASprite>> remap;
                vector<wstring> spriteKeys;
                string err;

                wstring baseName = Tile->GetGeneratedAtlasBaseName();

                if (!BuildSharedAtlasIfNeeded(used, newAtlas, remap, baseName,
                    AtlasCreateMode::REUSE, spriteKeys, err))
                {
                    m_AtlasNameConflict = true;
                    m_AtlasNameConflictMsg = err.empty() ? "Reuse failed." : err;
                }
                else
                {
                    Tile->SetRowCol(m_PendingRow, m_PendingCol);
                    Tile->SetTileSize(m_PendingCellSize);
                    Tile->SetAtlas(newAtlas);

                    for (int r = 0; r < m_PendingRow; ++r)
                    {
                        for (int c = 0; c < m_PendingCol; ++c)
                        {
                            Ptr<ASprite> sp = m_PendingCells[r * m_PendingCol + c];
                            if (!sp) { Tile->SetSprite(r, c, nullptr); continue; }

                            auto it = remap.find(sp.Get());
                            Tile->SetSprite(r, c, (it != remap.end()) ? it->second : nullptr);
                        }
                    }

                    Tile->SetGeneratedAtlasMeta(true, baseName, spriteKeys);
                    Tile->SaveBySelfRelativePath();

                    m_PendingAtlasSave = false;
                    m_OpenAtlasNameModal = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
        }

        // Create New
        if (ImGui::Button("Create New"))
        {
            wstring baseName(m_AtlasNameInput.begin(), m_AtlasNameInput.end());
            if (baseName.empty())
            {
                m_AtlasNameConflict = true;
                m_AtlasNameConflictMsg = "Name is empty.";
            }
            else
            {
                wstring atlasRel = L"Texture\\" + baseName + L".png";
                wstring atlasFull = CONTENT_PATH + atlasRel;

                if (filesystem::exists(atlasFull))
                {
                    m_AtlasNameConflict = true;
                    m_AtlasNameConflictMsg = "Name already exists. Choose another.";
                }
                else
                {
                    vector<Ptr<ASprite>> used = buildUsed();

                    Ptr<ATexture> newAtlas{};
                    unordered_map<ASprite*, Ptr<ASprite>> remap;
                    vector<wstring> spriteKeys;
                    string err;

                    if (!BuildSharedAtlasIfNeeded(used, newAtlas, remap, baseName,
                        AtlasCreateMode::NEWNAME, spriteKeys, err))
                    {
                        m_AtlasNameConflict = true;
                        m_AtlasNameConflictMsg = err.empty() ? "Create failed." : err;
                    }
                    else
                    {
                        // 새 생성 성공 후 이전 생성물 정리
                        CleanupGeneratedAssets(Tile.Get());

                        Tile->SetRowCol(m_PendingRow, m_PendingCol);
                        Tile->SetTileSize(m_PendingCellSize);
                        Tile->SetAtlas(newAtlas);

                        for (int r = 0; r < m_PendingRow; ++r)
                        {
                            for (int c = 0; c < m_PendingCol; ++c)
                            {
                                Ptr<ASprite> sp = m_PendingCells[r * m_PendingCol + c];
                                if (!sp) { Tile->SetSprite(r, c, nullptr); continue; }

                                auto it = remap.find(sp.Get());
                                Tile->SetSprite(r, c, (it != remap.end()) ? it->second : nullptr);
                            }
                        }

                        Tile->SetGeneratedAtlasMeta(true, baseName, spriteKeys);
                        Tile->SaveBySelfRelativePath();

                        m_PendingAtlasSave = false;
                        m_OpenAtlasNameModal = false;
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            m_PendingAtlasSave = false;
            m_OpenAtlasNameModal = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void TileMapUI::CleanupGeneratedAssets(ATileMap* tile)
{
    if (!tile || !tile->UsesGeneratedAtlas()) return;

    // Sprite 정리
    for (const auto& key : tile->GetGeneratedSpriteKeys())
    {
        AssetMgr::GetInst()->RemoveAsset(ASSET_TYPE::SPRITE, key);
        RemoveWindowFile(CONTENT_PATH + key);
    }

    // Atlas 정리
    if (!tile->GetGeneratedAtlasBaseName().empty())
    {
        wstring oldTex = L"Texture\\" + tile->GetGeneratedAtlasBaseName() + L".png";
        AssetMgr::GetInst()->RemoveAsset(ASSET_TYPE::TEXTURE, oldTex);
        RemoveWindowFile(CONTENT_PATH + oldTex);
    }

    tile->ClearGeneratedAtlasMeta();
}
