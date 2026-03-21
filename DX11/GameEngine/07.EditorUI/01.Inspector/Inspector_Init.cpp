#include "pch.h"
#include "Inspector.h"
#include "GameEngine/07.EditorUI/08.AssetUI/01.MeshUI/MeshUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/02.MaterialUI/MaterialUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/03.TextureUI/TextureUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/04.SpriteUI/SpriteUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/05.FlipbookUI/FlipbookUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/06.TileMapUI/TileMapUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/07.GraphicShaderUI/GraphicShaderUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/08.ComputeShaderUI/ComputeShaderUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/09.LevelUI/LevelUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/10.SoundUI/SoundUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/11.PrefabUI/PrefabUI.h"

#define ADD_ASSET_UI(AssetType, type) m_arrAssetUI[static_cast<UINT>(AssetType)] = new type;\
                                      AddChildUI(m_arrAssetUI[static_cast<UINT>(AssetType)].Get());

void Inspector::CreateChildUI()
{
    m_ObjectViewer.Init(this);

    ADD_ASSET_UI(ASSET_TYPE::MESH,              MeshUI);
    ADD_ASSET_UI(ASSET_TYPE::MATERIAL,          MaterialUI);
    ADD_ASSET_UI(ASSET_TYPE::TEXTURE,           TextureUI);
    ADD_ASSET_UI(ASSET_TYPE::SOUND,             SoundUI);
    ADD_ASSET_UI(ASSET_TYPE::GRAPHICS_SHADER,   GraphicShaderUI);
    ADD_ASSET_UI(ASSET_TYPE::COMPUTE_SHADER,    ComputeShaderUI);
    ADD_ASSET_UI(ASSET_TYPE::LEVEL,             LevelUI);
    ADD_ASSET_UI(ASSET_TYPE::SPRITE,            SpriteUI);
    ADD_ASSET_UI(ASSET_TYPE::FLIPBOOK,          FlipbookUI);
    ADD_ASSET_UI(ASSET_TYPE::TILEMAP,           TileMapUI);
    ADD_ASSET_UI(ASSET_TYPE::PREFAB,            PrefabUI);
}
