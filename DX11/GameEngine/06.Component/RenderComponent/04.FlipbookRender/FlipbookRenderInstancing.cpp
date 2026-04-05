#include "pch.h"
#include "FlipbookRenderInstancing.h"

#include <map>
#include <unordered_map>

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/02.Device/StructuredBuffer/StructuredBuffer.h"
#include "GameEngine/04.Asset/01.Mesh/AMesh.h"
#include "GameEngine/04.Asset/02.Texture/ATexture.h"
#include "GameEngine/04.Asset/03.GraphicShader/AGraphicShader.h"

namespace
{
    constexpr UINT FLIPBOOK_INSTANCE_DATA_REGISTER = 21;

    struct FlipbookInstanceData
    {
        Matrix  World{};
        Vec4    RenderTransform{};
        Vec4    UV0{}; // xy: LeftTop, zw: Slice
        Vec4    UV1{}; // xy: Background, zw: Offset
        Vec4    TintColor{};
    };

    struct FlipbookBatchKey
    {
        AMesh*          Mesh{};
        AGraphicShader* Shader{};
        ATexture*       Atlas{};

        bool operator==(const FlipbookBatchKey& _Other) const
        {
            return Mesh == _Other.Mesh
                && Shader == _Other.Shader
                && Atlas == _Other.Atlas;
        }
    };

    struct FlipbookBatchKeyHasher
    {
        size_t operator()(const FlipbookBatchKey& _Key) const
        {
            const size_t meshHash    = hash<AMesh*>()(_Key.Mesh);
            const size_t shaderHash  = hash<AGraphicShader*>()(_Key.Shader);
            const size_t atlasHash   = hash<ATexture*>()(_Key.Atlas);
            return meshHash ^ (shaderHash << 1) ^ (atlasHash << 2);
        }
    };

    struct FlipbookBatch
    {
        AMesh*                       Mesh{};
        AGraphicShader*              Shader{};
        ATexture*                    Atlas{};
        vector<FlipbookInstanceData> Instances{};
    };

    using FlipbookBatchMap = unordered_map<FlipbookBatchKey, FlipbookBatch, FlipbookBatchKeyHasher>;

    struct FlipbookDepthBucket
    {
        FlipbookBatchMap Batches{};
    };

    // Transparent는 Back-to-Front 순서(먼 쪽 -> 가까운 쪽)로 렌더링한다.
    // 현재 2D 카메라 구성에서 World Z가 클수록 카메라에서 더 멀다고 보고 내림차순으로 순회한다.
    map<float, FlipbookDepthBucket, greater<float>> g_FlipbookDepthBuckets{};

    Ptr<StructuredBuffer> g_FlipbookInstanceBuffer{};
    UINT g_FlipbookInstanceCapacity{};

    void EnsureFlipbookInstanceBuffer(UINT _RequiredCount)
    {
        if (!g_FlipbookInstanceBuffer)
            g_FlipbookInstanceBuffer = new StructuredBuffer;

        if (_RequiredCount <= g_FlipbookInstanceCapacity) return;

        g_FlipbookInstanceCapacity = max(64u, _RequiredCount);
        g_FlipbookInstanceBuffer->Create(sizeof(FlipbookInstanceData), g_FlipbookInstanceCapacity, SB_TYPE::SRV_ONLY, true);
    }
}

void FlipbookRenderInstancing::BeginFrame()
{
    g_FlipbookDepthBuckets.clear();
}

void FlipbookRenderInstancing::Submit
(
    AMesh*              _Mesh,
    AGraphicShader*     _Shader,
    ATexture*           _Atlas,
    const Matrix&       _World,
    const Vec4&         _RenderTransform,
    const Vec4&         _UV0,
    const Vec4&         _UV1,
    const Vec4&         _TintColor
)
{
    if (!_Mesh || !_Shader || !_Atlas) return;

    const float depthZ = _World._43;

    FlipbookInstanceData data{};
    data.World           = _World;
    data.RenderTransform = _RenderTransform;
    data.UV0             = _UV0;
    data.UV1             = _UV1;
    data.TintColor       = _TintColor;

    FlipbookBatchKey Key{};
    Key.Mesh      = _Mesh;
    Key.Shader    = _Shader;
    Key.Atlas     = _Atlas;

    FlipbookDepthBucket& depthBucket = g_FlipbookDepthBuckets[depthZ];
    auto [iter, inserted] = depthBucket.Batches.try_emplace(Key);
    if (inserted)
    {
        iter->second.Mesh   = _Mesh;
        iter->second.Shader = _Shader;
        iter->second.Atlas  = _Atlas;
    }

    iter->second.Instances.push_back(data);
}

void FlipbookRenderInstancing::Flush()
{
    if (g_FlipbookDepthBuckets.empty()) return;

    MtrlConst mtrlConst{};
    mtrlConst.IsTex[TEX_0] = true;

    for (auto& depthPair : g_FlipbookDepthBuckets)
    {
        FlipbookDepthBucket& depthBucket = depthPair.second;

        for (auto& batchPair : depthBucket.Batches)
        {
            FlipbookBatch& Batch = batchPair.second;
            const UINT instanceCount = static_cast<UINT>(Batch.Instances.size());
            if (0 == instanceCount) continue;
            if (!Batch.Mesh || !Batch.Shader || !Batch.Atlas) continue;

            EnsureFlipbookInstanceBuffer(instanceCount);
            if (!g_FlipbookInstanceBuffer) continue;

            g_FlipbookInstanceBuffer->SetData(Batch.Instances.data(), sizeof(FlipbookInstanceData) * instanceCount);
            g_FlipbookInstanceBuffer->Binding(FLIPBOOK_INSTANCE_DATA_REGISTER);

            Batch.Shader->Binding();
            Batch.Atlas->Binding(TEX_0);

            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->SetData(&mtrlConst);
            Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->Binding();

            Batch.Mesh->RenderInstanced(instanceCount);

            Batch.Atlas->Clear();
            g_FlipbookInstanceBuffer->Clear();
        }
    }

    g_FlipbookDepthBuckets.clear();
}
