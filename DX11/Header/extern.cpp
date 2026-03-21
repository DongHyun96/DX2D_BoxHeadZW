#include "pch.h"

TransformMatrix g_Trans{};
GlobalData      g_Global{};

const char* ASSET_TYPE_STR[static_cast<UINT>(ASSET_TYPE::END)] = 
{
    "MESH",
    "MATERIAL",
    "TEXTURE",
    "SPRITE",
    "FLIPBOOK",
    "TILEMAP",
    "GRAPHICS_SHADER",
    "COMPUTE_SHADER",
    "LEVEL",
    "SOUND",
    "PREFAB"
};

const char* ToString(ASSET_TYPE _Type)
{
    return ASSET_TYPE_STR[static_cast<UINT>(_Type)];
}
