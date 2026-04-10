#include "pch.h"
#include "SpriteRenderInstancing.h"

#include <map>
#include <unordered_map>

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/02.Device/StructuredBuffer/StructuredBuffer.h"
#include "GameEngine/04.Asset/01.Mesh/AMesh.h"
#include "GameEngine/04.Asset/02.Texture/ATexture.h"
#include "GameEngine/04.Asset/03.GraphicShader/AGraphicShader.h"

namespace
{
    constexpr UINT SPRITE_INSTANCE_DATA_REGISTER = 21;

    struct SpriteInstanceData
    {
        Matrix  World{};
        Vec2    LeftTopUV{};
        Vec2    SliceUV{};
    };

    struct SpriteBatchKey
    {
        AMesh*          Mesh{};
        AGraphicShader* Shader{};
        ATexture*       Atlas{};

        bool operator==(const SpriteBatchKey& _Other) const
        {
            return Mesh == _Other.Mesh
                && Shader == _Other.Shader
                && Atlas == _Other.Atlas;
        }
    };

    struct SpriteBatchKeyHasher
    {
        size_t operator()(const SpriteBatchKey& _Key) const
        {
            const size_t meshHash   = hash<AMesh*>()(_Key.Mesh);
            const size_t shaderHash = hash<AGraphicShader*>()(_Key.Shader);
            const size_t atlasHash  = hash<ATexture*>()(_Key.Atlas);
            return meshHash ^ (shaderHash << 1) ^ (atlasHash << 2);
        }
    };

    struct SpriteBatch
    {
        AMesh*                      Mesh{};
        AGraphicShader*             Shader{};
        ATexture*                   Atlas{};
        vector<SpriteInstanceData>  Instances{};
    };

    using SpriteBatchMap = unordered_map<SpriteBatchKey, SpriteBatch, SpriteBatchKeyHasher>;

    struct SpriteDepthBucket
    {
        SpriteBatchMap Batches{};
    };

    map<float, SpriteDepthBucket, greater<float>> g_SpriteDepthBuckets{};

    Ptr<StructuredBuffer> g_SpriteInstanceBuffer{};
    UINT g_SpriteInstanceCapacity{};

    void EnsureSpriteInstanceBuffer(UINT _RequiredCount)
    {
        if (!g_SpriteInstanceBuffer)
            g_SpriteInstanceBuffer = new StructuredBuffer;

        if (_RequiredCount <= g_SpriteInstanceCapacity) return;

        g_SpriteInstanceCapacity = max(64u, _RequiredCount);
        g_SpriteInstanceBuffer->Create(sizeof(SpriteInstanceData), g_SpriteInstanceCapacity, SB_TYPE::SRV_ONLY, true);
    }
}

void SpriteRenderInstancing::BeginInstancing()
{
    g_SpriteDepthBuckets.clear();
}

void SpriteRenderInstancing::Submit
(
    AMesh*              _Mesh,
    AGraphicShader*     _Shader,
    ATexture*           _Atlas,
    const Matrix&       _World,
    const Vec2&         _LeftTopUV,
    const Vec2&         _SliceUV
)
{
    if (!_Mesh || !_Shader || !_Atlas) return;

    const float depthZ = _World._43;

    SpriteInstanceData data{};
    data.World       = _World;
    data.LeftTopUV   = _LeftTopUV;
    data.SliceUV     = _SliceUV;

    SpriteBatchKey Key{};
    Key.Mesh   = _Mesh;
    Key.Shader = _Shader;
    Key.Atlas  = _Atlas;

    SpriteDepthBucket& depthBucket = g_SpriteDepthBuckets[depthZ];
    auto [iter, inserted] = depthBucket.Batches.try_emplace(Key);
    if (inserted)
    {
        iter->second.Mesh   = _Mesh;
        iter->second.Shader = _Shader;
        iter->second.Atlas  = _Atlas;
    }

    iter->second.Instances.push_back(data);
}

void SpriteRenderInstancing::FlushInstancing()
{
    if (g_SpriteDepthBuckets.empty()) return;

    MtrlConst mtrlConst{};
    mtrlConst.IsTex[TEX_0] = true;

    for (auto& depthPair : g_SpriteDepthBuckets)
    {
        SpriteDepthBucket& depthBucket = depthPair.second;

        for (auto& batchPair : depthBucket.Batches)
        {
            SpriteBatch& Batch = batchPair.second;
            const UINT instanceCount = static_cast<UINT>(Batch.Instances.size());
            if (0 == instanceCount) continue;
            if (!Batch.Mesh || !Batch.Shader || !Batch.Atlas) continue;

            EnsureSpriteInstanceBuffer(instanceCount);
            if (!g_SpriteInstanceBuffer) continue;

            g_SpriteInstanceBuffer->SetData(Batch.Instances.data(), sizeof(SpriteInstanceData) * instanceCount);
            g_SpriteInstanceBuffer->Binding(SPRITE_INSTANCE_DATA_REGISTER);

            Batch.Shader->Binding();
            Batch.Atlas->Binding(TEX_0);

            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->SetData(&mtrlConst);
            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->Binding();

            Batch.Mesh->RenderInstanced(instanceCount);

            Batch.Atlas->Clear();
            g_SpriteInstanceBuffer->Clear();
        }
    }

    g_SpriteDepthBuckets.clear();
}
