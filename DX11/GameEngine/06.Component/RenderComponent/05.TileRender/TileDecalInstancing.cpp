#include "pch.h"
#include "TileDecalInstancing.h"

#include <map>
#include <unordered_map>
#include <vector>

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/02.Device/StructuredBuffer/StructuredBuffer.h"
#include "GameEngine/04.Asset/01.Mesh/AMesh.h"
#include "GameEngine/04.Asset/02.Texture/ATexture.h"
#include "GameEngine/04.Asset/03.GraphicShader/AGraphicShader.h"

using namespace std;

namespace
{
    constexpr UINT TILE_DECAL_INSTANCE_DATA_REGISTER = 21;

    struct TileDecalInstanceData
    {
        Matrix  World{};
        Vec4    TintColor{};
        Vec2    LeftTopUV{};
        Vec2    SliceUV{};
    };

    struct TileDecalBatchKey
    {
        AMesh*          Mesh{};
        AGraphicShader* Shader{};
        ATexture*       Atlas{};

        bool operator==(const TileDecalBatchKey& _Other) const
        {
            return Mesh == _Other.Mesh
                && Shader == _Other.Shader
                && Atlas == _Other.Atlas;
        }
    };

    struct TileDecalBatchKeyHasher
    {
        size_t operator()(const TileDecalBatchKey& _Key) const
        {
            const size_t meshHash   = hash<AMesh*>()(_Key.Mesh);
            const size_t shaderHash = hash<AGraphicShader*>()(_Key.Shader);
            const size_t atlasHash  = hash<ATexture*>()(_Key.Atlas);
            return meshHash ^ (shaderHash << 1) ^ (atlasHash << 2);
        }
    };

    struct TileDecalBatch
    {
        AMesh*                          Mesh{};
        AGraphicShader*                 Shader{};
        ATexture*                       Atlas{};
        vector<TileDecalInstanceData>  Instances{};
    };

    using TileDecalBatchMap = unordered_map<TileDecalBatchKey, TileDecalBatch, TileDecalBatchKeyHasher>;

    struct TileDecalDepthBucket
    {
        TileDecalBatchMap Batches{};
    };

    // 데칼은 타일 위에 그려져야 하므로 Depth 순서가 중요할 수 있음 (일단은 맵으로 관리)
    map<float, TileDecalDepthBucket, greater<float>> g_TileDecalDepthBuckets{};

    StructuredBuffer* g_TileDecalInstanceBuffer = nullptr;
    UINT g_TileDecalInstanceCapacity = 0;

    void EnsureTileDecalInstanceBuffer(UINT _RequiredCount)
    {
        if (!g_TileDecalInstanceBuffer)
            g_TileDecalInstanceBuffer = new StructuredBuffer;

        if (_RequiredCount <= g_TileDecalInstanceCapacity) return;

        g_TileDecalInstanceCapacity = max(256u, _RequiredCount);
        g_TileDecalInstanceBuffer->Create(sizeof(TileDecalInstanceData), g_TileDecalInstanceCapacity, SB_TYPE::SRV_ONLY, true);
    }
}

void TileDecalInstancing::BeginInstancing()
{
    for (auto& depthPair : g_TileDecalDepthBuckets)
    {
        for (auto& batchPair : depthPair.second.Batches)
        {
            batchPair.second.Instances.clear();
        }
    }
    g_TileDecalDepthBuckets.clear();
}

void TileDecalInstancing::Submit
(
    AMesh*              _Mesh,
    AGraphicShader*     _Shader,
    ATexture*           _Atlas,
    const Matrix&       _World,
    const Vec2&         _LeftTopUV,
    const Vec2&         _SliceUV,
    const Vec4&         _TintColor
)
{
    if (!_Mesh || !_Shader || !_Atlas) return;

    const float depthZ = _World._43;

    TileDecalInstanceData data{};
    data.World       = _World;
    data.TintColor   = _TintColor;
    data.LeftTopUV   = _LeftTopUV;
    data.SliceUV     = _SliceUV;

    TileDecalBatchKey Key{};
    Key.Mesh   = _Mesh;
    Key.Shader = _Shader;
    Key.Atlas  = _Atlas;

    TileDecalDepthBucket& depthBucket = g_TileDecalDepthBuckets[depthZ];
    auto [iter, inserted] = depthBucket.Batches.try_emplace(Key);
    if (inserted)
    {
        iter->second.Mesh   = _Mesh;
        iter->second.Shader = _Shader;
        iter->second.Atlas  = _Atlas;
    }

    iter->second.Instances.push_back(data);
}

void TileDecalInstancing::FlushInstancing()
{
    if (g_TileDecalDepthBuckets.empty()) return;

    MtrlConst mtrlConst{};
    mtrlConst.IsTex[TEX_0] = true;

    for (auto& depthPair : g_TileDecalDepthBuckets)
    {
        TileDecalDepthBucket& depthBucket = depthPair.second;

        for (auto& batchPair : depthBucket.Batches)
        {
            TileDecalBatch& Batch = batchPair.second;
            const UINT instanceCount = static_cast<UINT>(Batch.Instances.size());
            if (0 == instanceCount) continue;
            if (!Batch.Mesh || !Batch.Shader || !Batch.Atlas) continue;

            EnsureTileDecalInstanceBuffer(instanceCount);
            if (!g_TileDecalInstanceBuffer) continue;

            g_TileDecalInstanceBuffer->SetData(Batch.Instances.data(), sizeof(TileDecalInstanceData) * instanceCount);
            g_TileDecalInstanceBuffer->Binding(TILE_DECAL_INSTANCE_DATA_REGISTER);

            Batch.Shader->Binding();
            Batch.Atlas->Binding(TEX_0);

            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->SetData(&mtrlConst);
            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->Binding();

            Batch.Mesh->RenderInstanced(instanceCount);

            Batch.Atlas->Clear();
            g_TileDecalInstanceBuffer->Clear();
        }
    }

    BeginInstancing();
}
