#pragma once
#include <GameEngine/06.Component/Component.h>
#include "Header/enum.h"
#include "Module/Util.h"

class CTransform : public Component
{
private:

    bool        m_bUpdateZDepthToYCoordOnEveryTick{}; // z depth를 매번 y로 갱신해야하는 GameObject들의 경우 해당 옵션을 켜줌
    Vec3        m_PrevRelativePos{}; // Blocking 용 PreRelativePos
    
private:

    Vec3        m_RelativePos{};
    Vec3        m_RelativeScale = Vec3::One;
    Vec3        m_RelativeRot{};
                
    Vec3        m_Pivot{};
    bool        m_IndependentScale{}; // 부모 오브젝트의 크기는 무시, 주의 : 상위 부모의 Scale z값 0이 되지 않도록 주의할 것
    
private:
    
    Matrix  m_MatWorld{};

private:

    // 방향 벡터
    Vec3    m_Dir[static_cast<UINT>(DIR::END)]{};
    
public:

    CTransform();
    virtual ~CTransform() override;
    
    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CTransform)
    
public:

    Vec3 GetRelativePos()       const { return m_RelativePos; }
    Vec3 GetRelativeScale()     const { return m_RelativeScale; }
    Vec3 GetRelativeRot()       const { return m_RelativeRot; }

    void SetPrevRelativePos(const Vec3& pos) { m_PrevRelativePos = pos; }
    
    void SetRelativePos(const Vec3& pos) { m_RelativePos = pos; }
    void SetRelativePos(float x, float y, float z) { m_RelativePos = Vec3(x, y, z); }

    /// <summary>
    /// 특정 WorldPos의 위치로 나의 상대위치값 계산해서 넣기
    /// </summary>
    void SetRelativePosFromWorldPos(const Vec3& _DesiredWorldPos);

    /// <summary>
    /// 특정 WorldPos의 위치로 나의 상대위치값이 얼만지 계산
    /// </summary>
    Vec3 CalculateRelativePosFromWorldPos(const Vec3& _DesiredWorldPos);
    
    void SetRelativeScale(const Vec3& scale) { m_RelativeScale = scale; }
    void SetRelativeScale(float x, float y, float z) { m_RelativeScale = Vec3(x, y, z); }
    
    void SetRelativeRot(const Vec3& rotation) { m_RelativeRot = rotation; }
    void SetRelativeRot(float x, float y, float z) { m_RelativeRot = Vec3(x, y, z); }

    float GetRelativePosX() const { return m_RelativePos.x; }
    float GetRelativePosY() const { return m_RelativePos.y; }
    float GetRelativePosZ() const { return m_RelativePos.z; }
    Vec2 GetRelativePosXY() const { return { m_RelativePos.x, m_RelativePos.y }; }
    
    void SetRelativePosX(float x) { m_RelativePos.x = x; }
    void SetRelativePosY(float y) { m_RelativePos.y = y; }
    void SetRelativePosZ(float z) { m_RelativePos.z = z; }
    void SetRelativePosXY(float x, float y) { m_RelativePos.x = x; m_RelativePos.y = y; }
    void SetRelativePosXY(const Vec2& _V) { m_RelativePos.x = _V.x; m_RelativePos.y = _V.y; }
    
    float GetRelativeScaleX() const { return m_RelativeScale.x; }
    float GetRelativeScaleY() const { return m_RelativeScale.y; }
    float GetRelativeScaleZ() const { return m_RelativeScale.z; }
    Vec2 GetRelativeScaleXY() const { return { m_RelativeScale.x, m_RelativeScale.y}; }
    
    void SetRelativeScaleX(float x) { m_RelativeScale.x = x; }
    void SetRelativeScaleY(float y) { m_RelativeScale.y = y; }
    void SetRelativeScaleZ(float z) { m_RelativeScale.z = z; }
    void SetRelativeScaleXY(float x, float y) { m_RelativeScale.x = x; m_RelativeScale.y = y; }
    void SetRelativeScaleXY(const Vec2& _V) { m_RelativeScale.x = _V.x; m_RelativeScale.y = _V.y; }
    
    float GetRelativeRotX() const { return m_RelativeRot.x; }
    float GetRelativeRotY() const { return m_RelativeRot.y; }
    float GetRelativeRotZ() const { return m_RelativeRot.z; }
    
    void SetRelativeRotX(float x) { m_RelativeRot.x = x; }
    void SetRelativeRotY(float y) { m_RelativeRot.y = y; }
    void SetRelativeRotZ(float z) { m_RelativeRot.z = z; }

    Vec3 GetWorldPos() const { return m_MatWorld.Translation(); }
    Vec2 GetWorldPos2D() const { return ToVec2(m_MatWorld.Translation()); }
    Vec3 GetWorldScale() const;
    Vec2 GetWorldScale2D() const { return ToVec2(GetWorldScale()); }
    

    Vec3 GetPivot() const { return m_Pivot; }
    void SetPivot(const Vec3& pivot) { this->m_Pivot = pivot; }

    const Matrix& GetWorldMatrix() const { return m_MatWorld; }
    void SetWorldMatrix(const Matrix& _matWorld) { m_MatWorld = _matWorld; }
    
    GET_SET(bool, IndependentScale)
    
    bool GetUpdateZDepthToYCoordOnEveryTick() const { return m_bUpdateZDepthToYCoordOnEveryTick; }
    void SetUpdateZDepthToYCoordOnEveryTick(bool _UpdateTrue) { m_bUpdateZDepthToYCoordOnEveryTick = _UpdateTrue; } 
    
    
public:
    
    /// <summary>
    /// Right, Up, Front 방향벡터 구하기 
    /// </summary>
    Vec3 GetDir(DIR _Type) const { return m_Dir[static_cast<UINT>(_Type)]; }

public:
    
    static CTransform Lerp(const CTransform& a, const CTransform& b, float alpha);
    static CTransform Lerp(CTransform* a, CTransform* b, float alpha) { return Lerp(*a, *b, alpha); }
    static CTransform Lerp(const Ptr<CTransform>& a, const Ptr<CTransform>& b, float alpha) { return Lerp(*(a.Get()), *(b.Get()), alpha); }

public:
    
    CTransform& operator=(const CTransform& _Other)
    {
        m_bUpdateZDepthToYCoordOnEveryTick  = _Other.m_bUpdateZDepthToYCoordOnEveryTick;
        m_RelativePos                       = _Other.m_RelativePos;
        m_RelativeScale                     = _Other.m_RelativeScale;
        m_RelativeRot                       = _Other.m_RelativeRot;
        m_Pivot                             = _Other.m_Pivot;
        m_IndependentScale                  = _Other.m_IndependentScale;
        return *this;
    }
    
public:
    
    /// <summary> 세팅된 위치, 크기, 회전 정보를 하나의 World행렬로 묶어준다 </summary>
    virtual void FinalTick() override;

    /// <summary>
    /// 데이터를 GPU 메모리로 전송
    /// </summary>
    void Binding();
    
public:
    
    /// <summary>
    /// 충돌 처리 Block 반응으로 Prev RelativePos로 돌아갈 시에, 호출시킨다 
    /// </summary>
    void UpdateTransformToPrevRelativePos()
    {
        m_RelativePos = m_PrevRelativePos;
        FinalTick();
    }
    
};
