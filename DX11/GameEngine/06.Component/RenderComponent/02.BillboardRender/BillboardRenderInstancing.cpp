#include "pch.h"
#include "BillboardRenderInstancing.h"

#include <map>
#include <unordered_map>

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/02.Device/StructuredBuffer/StructuredBuffer.h"
#include "GameEngine/04.Asset/01.Mesh/AMesh.h"
#include "GameEngine/04.Asset/02.Texture/ATexture.h"
#include "GameEngine/04.Asset/03.GraphicShader/AGraphicShader.h"

namespace
{
    constexpr UINT BILLBOARD_INSTANCE_DATA_REGISTER = 21;

    struct BillboardInstanceData
    {
        Matrix  World{};
        Vec2    BillboardScale{};
        Vec2    Padding{};
    };

    struct BillboardBatchKey
    {
        AMesh*          Mesh{};
        AGraphicShader* Shader{};
        ATexture*       Texture{};

        bool operator==(const BillboardBatchKey& _Other) const
        {
            return Mesh == _Other.Mesh
                && Shader == _Other.Shader
                && Texture == _Other.Texture;
        }
    };

    struct BillboardBatchKeyHasher
    {
        size_t operator()(const BillboardBatchKey& _Key) const
        {
            const size_t meshHash    = hash<AMesh*>()(_Key.Mesh);
            const size_t shaderHash  = hash<AGraphicShader*>()(_Key.Shader);
            const size_t textureHash = hash<ATexture*>()(_Key.Texture);
            return meshHash ^ (shaderHash << 1) ^ (textureHash << 2);
        }
    };

    struct BillboardBatch
    {
        AMesh*                         Mesh{};
        AGraphicShader*                Shader{};
        ATexture*                      Texture{};
        vector<BillboardInstanceData>  Instances{};
    };

    using BillboardBatchMap = unordered_map<BillboardBatchKey, BillboardBatch, BillboardBatchKeyHasher>;

    struct BillboardDepthBucket
    {
        BillboardBatchMap Batches{};
    };

    map<float, BillboardDepthBucket, greater<float>> g_BillboardDepthBuckets{};

    Ptr<StructuredBuffer> g_BillboardInstanceBuffer{};
    UINT g_BillboardInstanceCapacity{};

    void EnsureBillboardInstanceBuffer(UINT _RequiredCount)
    {
        if (!g_BillboardInstanceBuffer)
            g_BillboardInstanceBuffer = new StructuredBuffer;

        if (_RequiredCount <= g_BillboardInstanceCapacity) return;

        g_BillboardInstanceCapacity = max(64u, _RequiredCount);
        g_BillboardInstanceBuffer->Create(sizeof(BillboardInstanceData), g_BillboardInstanceCapacity, SB_TYPE::SRV_ONLY, true);
    }
}

void BillboardRenderInstancing::BeginFrame()
{
    g_BillboardDepthBuckets.clear();
}

void BillboardRenderInstancing::Submit
(
    AMesh*              _Mesh,
    AGraphicShader*     _Shader,
    ATexture*           _Texture,
    const Matrix&       _World,
    const Vec2&         _BillboardScale
)
{
    if (!_Mesh || !_Shader || !_Texture) return;

    const float depthZ = _World._43;

    BillboardInstanceData data{};
    data.World            = _World;
    data.BillboardScale   = _BillboardScale;

    BillboardBatchKey Key{};
    Key.Mesh    = _Mesh;
    Key.Shader  = _Shader;
    Key.Texture = _Texture;

    BillboardDepthBucket& depthBucket = g_BillboardDepthBuckets[depthZ];
    auto [iter, inserted] = depthBucket.Batches.try_emplace(Key);
    if (inserted)
    {
        iter->second.Mesh    = _Mesh;
        iter->second.Shader  = _Shader;
        iter->second.Texture = _Texture;
    }

    iter->second.Instances.push_back(data);
}

void BillboardRenderInstancing::Flush()
{
    if (g_BillboardDepthBuckets.empty()) return;

    MtrlConst mtrlConst{};
    mtrlConst.IsTex[TEX_0] = true;

    for (auto& depthPair : g_BillboardDepthBuckets)
    {
        BillboardDepthBucket& depthBucket = depthPair.second;

        for (auto& batchPair : depthBucket.Batches)
        {
            BillboardBatch& Batch = batchPair.second;
            const UINT instanceCount = static_cast<UINT>(Batch.Instances.size());
            if (0 == instanceCount) continue;
            if (!Batch.Mesh || !Batch.Shader || !Batch.Texture) continue;

            EnsureBillboardInstanceBuffer(instanceCount);
            if (!g_BillboardInstanceBuffer) continue;

            g_BillboardInstanceBuffer->SetData(Batch.Instances.data(), sizeof(BillboardInstanceData) * instanceCount);
            g_BillboardInstanceBuffer->Binding(BILLBOARD_INSTANCE_DATA_REGISTER);

            Batch.Shader->Binding();
            Batch.Texture->Binding(TEX_0);

            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->SetData(&mtrlConst);
            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->Binding();

            Batch.Mesh->RenderInstanced(instanceCount);

            Batch.Texture->Clear();
            g_BillboardInstanceBuffer->Clear();
        }
    }

    g_BillboardDepthBuckets.clear();
}
