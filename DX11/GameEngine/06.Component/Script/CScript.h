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
    
    friend class GameObject;
    
private:
    
    const int               m_ScriptType{};
    vector<tScriptParam>    m_vecScriptParam{};
    
protected:
    
    Vec3 m_PrevRelativePosition{}; // Blocking 처리용 이전 PrevRelativePos

private:
    
    bool m_bUseEditingTick{}; // Editing 환경에서 EditingTick을 사용하는 Script라면 true로 켜줄 것
    
public:
    
    CScript(int _ScriptType);
    CScript(enum SCRIPT_TYPE _ScriptType);
    CScript(const CScript& _Origin);
    virtual ~CScript() override;
    
public:
    
    virtual void Tick() = 0;
    
    // final - 최종구현이며 자식 클래스에서 더 이상 구현할 수 없다
    virtual void FinalTick() override final {}
    
protected:
    
    /// <summary>
    /// Editing 환경에서 EditingTick을 통한 Tick 처리 받기 처리 활성화 
    /// </summary>
    /// <returns> : 실패했다면 return false </returns>
    bool RegisterEditingTickEnabled();

    /// <summary>
    /// <para> Editing Tick 비활성화 처리 </para>
    /// <para> 이 처리를 해야하는 두 가지 상황 </para>
    /// <para> 1. Owner GO에서 EditingTick을 사용하는 Script가 모두(꼭 모두 지워졌을 때를 확인해야 함) Remove 처리 되었을 때 </para>
    /// <para> 2. Owner Go 자체가 Destroy 요청이 들어왔을 때 -> </para>
    /// <para> GO 자체의 Tick은 상관 없겠지만, ALevel의 EditingTick set에서 해당 GO를 지우는 처리를 해주어야 함 </para>
    /// </summary>
    void DeRegisterEditingTickEnabled();
    
    void RequestLevelToRetrySave();
    
public:
    
    int GetScriptType() const { return m_ScriptType; }
    
    bool GetIsUseEditingTick() const { return m_bUseEditingTick; }
    
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
    
protected:
    
    /// <summary>
    /// Owner GO에서 RemoveScript로 이 Script가 Remove당했을 때 Callback 받는 함수 
    /// </summary>
    virtual void OnRemoveScript() { SetOwner(nullptr); }

    /// <summary>
    /// Owner Go가 Destroy되었을 때 호출처리될 함수 (만약 Owner Destroy 이전에 할 일이 있다면 이 함수 override 받아서 실행)
    /// </summary>
    virtual void OnOwnerDestroy() {}
    
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

