#pragma once
#include "GameEngine/06.Component/Component.h"

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

// 원하는 Type 추가 가능 -> ScriptUI에서 해당 ScriptParam에 대응되는 UI 제작
enum class SCRIPT_PARAM
{
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    COLOR,
    MATRIX,
    WSTRING,
    TEXTURE,
    MATERIAL,
    SOUND,
    ROUND_INFO_VECTOR,
    PROGRESS_BAR,
    FONT_STYLE,
    FONT_ALIGN,
};

struct tScriptParam
{
    SCRIPT_PARAM    Param{};
    void*           Data{};
    wstring         Desc{};
    bool            IsInput{};
    float           Step{};
    bool            Enabled{};
};

class CScript : public Component
{
private:
    
    const int               m_ScriptType{};
    vector<tScriptParam>    m_vecScriptParam{};
    
protected:
    
    Vec3 m_PrevRelativePosition{}; // Blocking 처리용 이전 PrevRelativePos 
    
public:
    
    CScript(int _ScriptType);
    CScript(enum SCRIPT_TYPE _ScriptType);
    CScript(const CScript& _Origin);
    virtual ~CScript() override;
    
public:
    
    virtual void Tick() = 0;
    
    // final - 최종구현이며 자식 클래스에서 더 이상 구현할 수 없다
    virtual void FinalTick() override final {}
    
public:
    
    int GetScriptType() const { return m_ScriptType; }
    
protected:
    
    /// <summary>
    /// <para> 멤버변수 ScriptUI 정보 추가 </para>
    /// <para> 주의 : Init시점에서 호출할 것 </para>
    /// </summary>
    /// <param name="_Type"> : 멤버변수 ParamType </param>
    /// <param name="_Data"> : 멤버변수 주소 </param>
    /// <param name="_Desc"> : 노출하는 Description </param>
    /// <param name="_IsInput"> : Input인지 Drag인지 </param>
    /// <param name="_Step"> : Drag Step </param>
    void AddScriptParam(SCRIPT_PARAM _Type, void* _Data, const wstring& _Desc, bool _IsInput = true, float _Step = 1.f, bool _Enabled = true)
    {
        m_vecScriptParam.push_back(tScriptParam{ _Type , _Data, _Desc, _IsInput, _Step, _Enabled });
    }
    
    void RemoveScriptParam(SCRIPT_PARAM _Type, void* _Data, const wstring& _Desc)
    {
        auto it = std::find_if(m_vecScriptParam.begin(), m_vecScriptParam.end(), [&](const tScriptParam& param) {
            return param.Param == _Type && param.Data == _Data && param.Desc == _Desc;
        });
        
        if (it != m_vecScriptParam.end())
        {
            m_vecScriptParam.erase(it);
        }
    }
    
public:
    
    const vector<tScriptParam>& GetScriptParam() const { return m_vecScriptParam; }

public:
    
    /*virtual void BeginOverlap
    (
        CCollider2D* _OwnerCollider,
        CCollider2D* _OtherCollider
    ) {}
    
    virtual void Overlap
    (
        CCollider2D* _OwnerCollider,
        CCollider2D* _OtherCollider
    ) {}
    
    virtual void EndOverlap
    (
        CCollider2D* _OwnerCollider,
        CCollider2D* _OtherCollider
    ) {}*/
    
    
protected:
    
    void Destroy();

public:
    
    virtual CScript* Clone() const = 0;
    
    
};

#define ADD_DYNAMIC_BEGIN_OVERLAP(MemFunc)   GetCollider2D()->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&MemFunc))
#define ADD_DYNAMIC_OVERLAP(MemFunc)         GetCollider2D()->AddDynamicOverlap(this, static_cast<COLLISION_EVENT>(&MemFunc))
#define ADD_DYNAMIC_END_OVERLAP(MemFunc)     GetCollider2D()->AddDynamicEndOverlap(this, static_cast<COLLISION_EVENT>(&MemFunc))

