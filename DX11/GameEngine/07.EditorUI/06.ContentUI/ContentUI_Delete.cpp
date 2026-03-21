#include "pch.h"
#include "ContentUI.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"
#include "Source/ScriptMgr.h"

namespace
{
    string ToNarrow(const wstring& _W)
    {
        return string(_W.begin(), _W.end());
    }

    wstring ObjNameOrDefault(const Ptr<GameObject>& _Obj)
    {
        if (!_Obj) return L"InvalidObject";
        return _Obj->GetName().empty() ? L"UnNamed" : _Obj->GetName();
    }
}

void ContentUI::DeleteAssetTick()
{
    if (m_DeleteFlowActive) return;

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput) return;

    const ImGuiInputFlags flags = ImGuiInputFlags_RouteFocused;
    if (!ImGui::Shortcut(ImGuiKey_Delete, flags)) return;

    
    
    
    vector<Ptr<Asset>> ToDelete{};

    const auto& selectedNodes = m_Tree->GetSelectedNodes();
    if (!selectedNodes.empty())
    {
        for (const Ptr<TreeNode>& node : selectedNodes)
        {
            if (!node || node->Data == 0) continue;
            ToDelete.push_back(reinterpret_cast<Asset*>(node->Data));
        }
    }
    else
    {
        Ptr<TreeNode> single = m_Tree->GetSelected();
        if (single && single->Data != 0)
            ToDelete.push_back(reinterpret_cast<Asset*>(single->Data));
    }

    if (!ToDelete.empty()) OnDeleteKeyPressed(ToDelete);
}

void ContentUI::OnDeleteKeyPressed(const vector<Ptr<Asset>>& _Assets)
{
    unordered_set<Asset*> unique{};
    m_DeleteQueue.clear();

    for (const Ptr<Asset>& asset : _Assets)
    {
        if (!asset) continue;

        if (asset->GetIsProvidedByEngine())
        {
            DebugUtil::AddDebugLog(L"[ContentUI::Delete] Engine provided asset cannot be deleted: " + asset->GetKey());
            continue;
        }

        if (unique.insert(asset.Get()).second)
            m_DeleteQueue.push_back(asset);
    }

    if (m_DeleteQueue.empty()) return;

    m_DeleteFlowActive = true;
    m_DeleteCursor = 0;
    m_DeleteSucceeded = 0;
    m_DeleteSkipped = 0;
    m_CurrentReferencers.clear();

    Ptr<ConfirmUI> pUI = dynamic_cast<ConfirmUI*>(EditorMgr::GetInst()->FindUI("ConfirmUI").Get());
    assert(pUI.Get());

    pUI->SetWarningText(BuildDeleteConfirmText());
    pUI->AddDelegate(this, static_cast<DELEGATE_BOOL>(&ContentUI::OnDeleteSelectionConfirm));
    pUI->SetActive(true);
}

void ContentUI::OnDeleteSelectionConfirm(bool _Yes)
{
    if (!_Yes)
    {
        ResetDeleteFlow();
        return;
    }

    ProcessDeleteQueue();
}

void ContentUI::OnDeleteSingleConfirm(bool _Yes)
{
    if (m_DeleteCursor >= m_DeleteQueue.size())
    {
        ResetDeleteFlow();
        return;
    }

    Ptr<Asset> target = m_DeleteQueue[m_DeleteCursor];

    if (_Yes)
    {
        ClearAllReferencesToAsset(target);

        if (DeleteAssetNow(target)) ++m_DeleteSucceeded;
        else                         ++m_DeleteSkipped;
    }
    else ++m_DeleteSkipped;

    ++m_DeleteCursor;
    ProcessDeleteQueue();
}

void ContentUI::ProcessDeleteQueue()
{
    while (m_DeleteCursor < m_DeleteQueue.size())
    {
        Ptr<Asset> target = m_DeleteQueue[m_DeleteCursor];

        if (!target || target->GetIsProvidedByEngine())
        {
            ++m_DeleteSkipped;
            ++m_DeleteCursor;
            continue;
        }

        vector<wstring> refs{};
        CollectReferencers(target, refs);

        if (refs.empty())
        {
            if (DeleteAssetNow(target)) ++m_DeleteSucceeded;
            else                         ++m_DeleteSkipped;

            ++m_DeleteCursor;
            continue;
        }

        m_CurrentReferencers = refs;

        Ptr<ConfirmUI> pUI = dynamic_cast<ConfirmUI*>(EditorMgr::GetInst()->FindUI("ConfirmUI").Get());
        assert(pUI.Get());

        pUI->SetWarningText(BuildReferencerConfirmText(target, m_CurrentReferencers));
        pUI->AddDelegate(this, static_cast<DELEGATE_BOOL>(&ContentUI::OnDeleteSingleConfirm));
        pUI->SetActive(true);
        return;
    }

    wstring log = L"[ContentUI::Delete] Done. deleted=" + to_wstring(m_DeleteSucceeded)
                + L", skipped=" + to_wstring(m_DeleteSkipped);
    DebugUtil::AddDebugLog(log);

    ReNew();
    ResetDeleteFlow();
}

void ContentUI::ResetDeleteFlow()
{
    m_DeleteFlowActive      = false;
    m_DeleteCursor          = 0;
    m_DeleteSucceeded       = 0;
    m_DeleteSkipped         = 0;
    
    m_DeleteQueue.clear();
    m_CurrentReferencers.clear();
}

void ContentUI::CollectReferencers(const Ptr<Asset>& _Target, vector<wstring>& _OutRefs) const
{
    _OutRefs.clear();
    if (!_Target) return;

    // Asset -> Asset 참조
    vector<wstring> keys{};
    for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
    {
        const ASSET_TYPE type = static_cast<ASSET_TYPE>(i);

        keys.clear();
        AssetMgr::GetInst()->GetAssetKeys(type, keys);

        for (const wstring& key : keys)
        {
            Ptr<Asset> owner = AssetMgr::GetInst()->Find(type, key, false);
            if (!owner || owner == _Target) continue;

            CollectReferencersFromAsset(owner, _Target, _OutRefs);
        }
    }

    // Component / Script 참조
    vector<Ptr<GameObject>> objects{};
    GatherAllObjects(objects);

    for (const Ptr<GameObject>& obj : objects)
        CollectReferencersFromObject(obj, _Target, _OutRefs);
}

void ContentUI::CollectReferencersFromAsset(const Ptr<Asset>& _Owner, const Ptr<Asset>& _Target, vector<wstring>& _OutRefs) const
{
    if (!_Owner || !_Target) return;

    ATexture*       targetTex       = dynamic_cast<ATexture*>(_Target.Get());
    AGraphicShader* targetShader    = dynamic_cast<AGraphicShader*>(_Target.Get());
    ASprite*        targetSprite    = dynamic_cast<ASprite*>(_Target.Get());

    if (AMaterial* mtrl = dynamic_cast<AMaterial*>(_Owner.Get()))
    {
        if (targetShader && mtrl->GetShader() == targetShader)
            _OutRefs.push_back(L"[Asset] " + _Owner->GetKey() + L" (Shader)");

        if (targetTex)
        {
            for (int i = 0; i < TEX_END; ++i)
            {
                if (mtrl->GetTexture(static_cast<TEX_PARAM>(i)) == targetTex)
                    _OutRefs.push_back(L"[Asset] " + _Owner->GetKey() + L" (Texture Slot " + to_wstring(i) + L")");
            }
        }
        return;
    }

    if (ASprite* sprite = dynamic_cast<ASprite*>(_Owner.Get()))
    {
        if (targetTex && sprite->GetAtlas() == targetTex)
            _OutRefs.push_back(L"[Asset] " + _Owner->GetKey() + L" (Atlas)");
        
        return;
    }

    if (AFlipbook* flip = dynamic_cast<AFlipbook*>(_Owner.Get()))
    {
        if (targetSprite)
        {
            const UINT count = flip->GetSpriteCount();
            for (UINT i = 0; i < count; ++i)
            {
                if (flip->GetSprite(i) == targetSprite)
                    _OutRefs.push_back(L"[Asset] " + _Owner->GetKey() + L" (Sprite[" + to_wstring(i) + L"])");
            }
        }
        return;
    }

    if (ATileMap* tile = dynamic_cast<ATileMap*>(_Owner.Get()))
    {
        if (targetTex && tile->GetAtlas() == targetTex)
            _OutRefs.push_back(L"[Asset] " + _Owner->GetKey() + L" (Atlas)");

        if (targetSprite)
        {
            const vector<Ptr<ASprite>>& cells = tile->GetSprites();
            for (size_t i = 0; i < cells.size(); ++i)
            {
                if (cells[i] == targetSprite)
                    _OutRefs.push_back(L"[Asset] " + _Owner->GetKey() + L" (Cell[" + to_wstring(i) + L"])");
            }
        }
    }
}

void ContentUI::CollectReferencersFromObject(const Ptr<GameObject>& _Obj, const Ptr<Asset>& _Target, vector<wstring>& _OutRefs) const
{
    if (!_Obj || !_Target) return;

    const wstring objName = ObjNameOrDefault(_Obj);

    AMesh*      targetMesh      = dynamic_cast<AMesh*>(_Target.Get());
    AMaterial*  targetMtrl      = dynamic_cast<AMaterial*>(_Target.Get());
    ASprite*    targetSprite    = dynamic_cast<ASprite*>(_Target.Get());
    AFlipbook*  targetFlipbook  = dynamic_cast<AFlipbook*>(_Target.Get());
    ATileMap*   targetTileMap   = dynamic_cast<ATileMap*>(_Target.Get());
    ATexture*   targetTex       = dynamic_cast<ATexture*>(_Target.Get());

    if (Ptr<CRenderComponent> render = _Obj->GetRenderCom())
    {
        if (targetMesh && render->GetMesh() == targetMesh)
            _OutRefs.push_back(L"[Component] " + objName + L" (RenderComponent.Mesh)");

        if (targetMtrl && render->GetMaterial() == targetMtrl)
            _OutRefs.push_back(L"[Component] " + objName + L" (RenderComponent.Material)");
    }

    if (Ptr<CSpriteRender> sp = _Obj->SpriteRender())
    {
        if (targetSprite && sp->GetSprite() == targetSprite)
            _OutRefs.push_back(L"[Component] " + objName + L" (CSpriteRender.Sprite)");
    }

    if (Ptr<CFlipbookRender> fr = _Obj->FlipbookRender())
    {
        if (targetFlipbook)
        {
            for (int i = 0; i < static_cast<int>(fr->GetFlipbookCount()); ++i)
            {
                if (fr->GetFlipbook(i) == targetFlipbook)
                    _OutRefs.push_back(L"[Component] " + objName + L" (CFlipbookRender.Flipbook[" + to_wstring(i) + L"])");
            }
        }
    }

    if (Ptr<CTileRender> tr = _Obj->TileRender())
    {
        if (targetTileMap && tr->GetTileMap() == targetTileMap)
            _OutRefs.push_back(L"[Component] " + objName + L" (CTileRender.TileMap)");
    }

    for (const Ptr<CScript>& script : _Obj->GetScripts())
    {
        if (!script) continue;

        // EditorCamMoveScript 같이 ScriptType < 0 인 엔진/내부 스크립트는 ScriptMgr name 조회 대상에서 제외
        const int scriptType = script->GetScriptType();
        if (scriptType < 0) continue;

        const wstring scriptName = ScriptMgr::GetScriptName(script.Get());
        const vector<tScriptParam>& params = script->GetScriptParam();

        for (const tScriptParam& p : params)
        {
            if (!p.Data) continue;

            if (p.Param == SCRIPT_PARAM::TEXTURE && targetTex)
            {
                Ptr<ATexture>* ptr = static_cast<Ptr<ATexture>*>(p.Data);
                if (ptr && (*ptr == targetTex))
                    _OutRefs.push_back(L"[Script] " + objName + L" (" + scriptName + L") Param=" + p.Desc);
            }
            else if (p.Param == SCRIPT_PARAM::MATERIAL && targetMtrl)
            {
                Ptr<AMaterial>* ptr = static_cast<Ptr<AMaterial>*>(p.Data);
                if (ptr && (*ptr == targetMtrl))
                    _OutRefs.push_back(L"[Script] " + objName + L" (" + scriptName + L") Param=" + p.Desc);
            }
        }
    }
}

void ContentUI::GatherAllObjects(vector<Ptr<GameObject>>& _OutObjects) const
{
    _OutObjects.clear();
    unordered_set<GameObject*> visited{};

    auto collectFromLevel = [this, &_OutObjects, &visited](const Ptr<ALevel>& level)
    {
        if (!level) return;

        for (UINT i = 0; i < MAX_LAYER; ++i)
        {
            const vector<Ptr<GameObject>>& parents = level->GetLayer(i)->GetParentObjects();
            for (const Ptr<GameObject>& parent : parents)
                CollectObjectRecursive(parent, visited, _OutObjects);
        }
    };

    collectFromLevel(LevelMgr::GetInst()->GetCurLevel());

    vector<wstring> levelKeys{};
    AssetMgr::GetInst()->GetAssetKeys(ASSET_TYPE::LEVEL, levelKeys);
    for (const wstring& key : levelKeys)
    {
        Ptr<Asset> a = AssetMgr::GetInst()->Find(ASSET_TYPE::LEVEL, key, false);
        Ptr<ALevel> lv = dynamic_cast<ALevel*>(a.Get());
        if (lv) collectFromLevel(lv);
    }

    for (const Ptr<GameObject>& editorObj : EditorMgr::GetInst()->GetGameObjects())
        CollectObjectRecursive(editorObj, visited, _OutObjects);

    vector<wstring> prefabKeys{};
    AssetMgr::GetInst()->GetAssetKeys(ASSET_TYPE::PREFAB, prefabKeys);
    for (const wstring& key : prefabKeys)
    {
        Ptr<Asset> a = AssetMgr::GetInst()->Find(ASSET_TYPE::PREFAB, key, false);
        Ptr<APrefab> prefab = dynamic_cast<APrefab*>(a.Get());
        if (prefab && prefab->m_ProtoObj)
            CollectObjectRecursive(prefab->m_ProtoObj, visited, _OutObjects);
    }
}

void ContentUI::CollectObjectRecursive(const Ptr<GameObject>& _Obj, unordered_set<GameObject*>& _Visited, vector<Ptr<GameObject>>& _Out) const
{
    if (!_Obj) return;
    if (!_Visited.insert(_Obj.Get()).second) return;

    _Out.push_back(_Obj);

    for (const Ptr<GameObject>& child : _Obj->GetChildren())
        CollectObjectRecursive(child, _Visited, _Out);
}

int ContentUI::ClearAllReferencesToAsset(const Ptr<Asset>& _Target)
{
    if (!_Target) return 0;

    int cleared = 0;

    vector<wstring> keys{};
    for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
    {
        const ASSET_TYPE type = static_cast<ASSET_TYPE>(i);

        keys.clear();
        AssetMgr::GetInst()->GetAssetKeys(type, keys);

        for (const wstring& key : keys)
        {
            Ptr<Asset> owner = AssetMgr::GetInst()->Find(type, key, false);
            if (!owner || owner == _Target) continue;

            cleared += ClearReferencesFromAsset(owner, _Target);
        }
    }

    vector<Ptr<GameObject>> objects{};
    GatherAllObjects(objects);

    for (const Ptr<GameObject>& obj : objects)
        cleared += ClearReferencesFromObject(obj, _Target);

    if (cleared > 0)
    {
        DebugUtil::AddDebugLog(L"[ContentUI::Delete] Cleared " + to_wstring(cleared) + L" dangling references.");
    }

    return cleared;
}

int ContentUI::ClearReferencesFromAsset(const Ptr<Asset>& _Owner, const Ptr<Asset>& _Target)
{
    if (!_Owner || !_Target) return 0;

    int cleared = 0;

    ATexture* targetTex       = dynamic_cast<ATexture*>(_Target.Get());
    ASprite*  targetSprite    = dynamic_cast<ASprite*>(_Target.Get());
    
    // Shader의 경우, 엔진 제공 컨텐츠 밖에 없다
    // AGraphicShader* targetShader    = dynamic_cast<AGraphicShader*>(_Target.Get());

    if (AMaterial* mtrl = dynamic_cast<AMaterial*>(_Owner.Get()))
    {
        /*if (targetShader && mtrl->GetShader() == targetShader)
        {
            mtrl->SetShader(nullptr);
            ++cleared;
        }*/

        if (targetTex)
        {
            for (int i = 0; i < TEX_END; ++i)
            {
                TEX_PARAM slot = static_cast<TEX_PARAM>(i);
                if (mtrl->GetTexture(slot) == targetTex)
                {
                    mtrl->SetTexture(slot, nullptr);
                    ++cleared;
                }
            }
        }
        
        return cleared;
    }

    if (ASprite* sp = dynamic_cast<ASprite*>(_Owner.Get()))
    {
        if (targetTex && sp->GetAtlas() == targetTex)
        {
            sp->SetAtlas(nullptr);
            ++cleared;
        }
        
        return cleared;
    }

    if (AFlipbook* flip = dynamic_cast<AFlipbook*>(_Owner.Get()))
    {
        bool RemoveSucceeded{};
        if (targetSprite)
            RemoveSucceeded = flip->RemoveSprite(targetSprite); // 만약 해당 Sprite가 없다면 아무 일 x

        if (RemoveSucceeded) ++cleared;
        return cleared;
    }

    if (ATileMap* tile = dynamic_cast<ATileMap*>(_Owner.Get()))
    {
        if (targetTex && tile->GetAtlas() == targetTex)
        {
            tile->SetAtlas(nullptr);
            ++cleared;
        }

        if (targetSprite)
        {
            const UINT col = tile->GetCol();
            if (col > 0)
            {
                const vector<Ptr<ASprite>>& cells = tile->GetSprites();
                for (size_t i = 0; i < cells.size(); ++i)
                {
                    if (cells[i] == targetSprite)
                    {
                        const UINT rowIdx = static_cast<UINT>(i) / col;
                        const UINT colIdx = static_cast<UINT>(i) % col;
                        tile->SetSprite(rowIdx, colIdx, nullptr);
                        ++cleared;
                    }
                }
            }
        }
        
        return cleared;
    }

    return cleared;
}

int ContentUI::ClearReferencesFromObject(const Ptr<GameObject>& _Obj, const Ptr<Asset>& _Target)
{
    if (!_Obj || !_Target) return 0;

    int cleared = 0;

    AMesh*      targetMesh      = dynamic_cast<AMesh*>(_Target.Get());
    AMaterial*  targetMtrl      = dynamic_cast<AMaterial*>(_Target.Get());
    ASprite*    targetSprite    = dynamic_cast<ASprite*>(_Target.Get());
    AFlipbook*  targetFlipbook  = dynamic_cast<AFlipbook*>(_Target.Get());
    ATileMap*   targetTileMap   = dynamic_cast<ATileMap*>(_Target.Get());
    ATexture*   targetTex       = dynamic_cast<ATexture*>(_Target.Get());

    if (Ptr<CRenderComponent> render = _Obj->GetRenderCom())
    {
        if (targetMesh && render->GetMesh() == targetMesh)
        {
            render->SetMesh(nullptr);
            ++cleared;
        }

        if (targetMtrl && render->GetMaterial() == targetMtrl)
        {
            render->SetMaterial(nullptr);
            ++cleared;
        }
    }

    if (Ptr<CSpriteRender> sp = _Obj->SpriteRender())
    {
        if (targetSprite && sp->GetSprite() == targetSprite)
        {
            sp->SetSprite(nullptr);
            ++cleared;
        }
    }

    if (Ptr<CFlipbookRender> fr = _Obj->FlipbookRender())
    {
        if (targetFlipbook)
        {
            if (fr->RemoveFlipbook(targetFlipbook))
                ++cleared;
        }
    }

    if (Ptr<CTileRender> tr = _Obj->TileRender())
    {
        if (targetTileMap && tr->GetTileMap() == targetTileMap)
        {
            tr->SetTileMap(nullptr);
            ++cleared;
        }
    }

    // 스크립트의 경우, 만약 에디터 이외에 코드 상에서 Asset reference를 들고 있게끔 처리를 한 경우에는,
    // 직접 터쳐서 찾을 것 (어쩔 수 없음)
    for (const Ptr<CScript>& script : _Obj->GetScripts())
    {
        if (!script) continue;
        if (script->GetScriptType() < 0) continue;

        const vector<tScriptParam>& params = script->GetScriptParam();
        for (const tScriptParam& p : params)
        {
            if (!p.Data) continue;

            if (p.Param == SCRIPT_PARAM::TEXTURE && targetTex)
            {
                Ptr<ATexture>* ptr = static_cast<Ptr<ATexture>*>(p.Data);
                if (ptr && (*ptr == targetTex))
                {
                    *ptr = nullptr;
                    ++cleared;
                }
            }
            else if (p.Param == SCRIPT_PARAM::MATERIAL && targetMtrl)
            {
                Ptr<AMaterial>* ptr = static_cast<Ptr<AMaterial>*>(p.Data);
                if (ptr && (*ptr == targetMtrl))
                {
                    *ptr = nullptr;
                    ++cleared;
                }
            }
        }
    }

    return cleared;
}

bool ContentUI::DeleteAssetNow(const Ptr<Asset>& _Asset)
{
    if (!_Asset) return false;
    if (_Asset->GetIsProvidedByEngine()) return false;

    const ASSET_TYPE type = _Asset->GetType();
    const wstring key = _Asset->GetKey();
    const wstring rel = _Asset->GetRelativePath();
    const wstring fullPath = rel.empty() ? L"" : (CONTENT_PATH + rel);

    bool fileRemoved = true;
    if (!fullPath.empty() && filesystem::exists(fullPath))
        fileRemoved = RemoveWindowFile(fullPath);

    bool mapRemoved = AssetMgr::GetInst()->RemoveAsset(type, key);

    for (const Ptr<Inspector>& inspector : EditorMgr::GetInst()->GetInspectors())
    {
        if (inspector && inspector->GetTargetAsset() == _Asset)
            inspector->SetTargetAsset(nullptr);
    }

    if (!mapRemoved)
    {
        DebugUtil::AddDebugLog(L"[ContentUI::Delete] RemoveAsset failed: " + key);
        return false;
    }

    if (!fileRemoved)
    {
        DebugUtil::AddDebugLog(L"[ContentUI::Delete] File delete failed: " + fullPath);
        return false;
    }

    return true;
}

string ContentUI::BuildDeleteConfirmText() const
{
    string msg = "Delete selected assets?\n";
    msg += "Count: " + to_string(m_DeleteQueue.size()) + "\n\n";

    const size_t previewCount = min<size_t>(m_DeleteQueue.size(), 8);
    for (size_t i = 0; i < previewCount; ++i)
    {
        if (!m_DeleteQueue[i]) continue;
        msg += " - " + ToNarrow(m_DeleteQueue[i]->GetKey()) + "\n";
    }

    if (m_DeleteQueue.size() > previewCount)
        msg += " ... +" + to_string(m_DeleteQueue.size() - previewCount) + " more";

    return msg;
}

string ContentUI::BuildReferencerConfirmText(const Ptr<Asset>& _Target, const vector<wstring>& _Refs) const
{
    string msg = "Asset is referenced.\nDelete anyway and clear those references?\n\n";
    msg += "Target: " + ToNarrow(_Target->GetKey()) + "\n";
    msg += "Referencers: " + to_string(_Refs.size()) + "\n\n";

    const size_t previewCount = min<size_t>(_Refs.size(), 10);
    for (size_t i = 0; i < previewCount; ++i)
        msg += " - " + ToNarrow(_Refs[i]) + "\n";

    if (_Refs.size() > previewCount)
        msg += " ... +" + to_string(_Refs.size() - previewCount) + " more";

    return msg;
}

