#pragma once

#include "GameEngine/04.Asset/Asset.h"

class APrefab : public Asset
{
    
    friend class PrefabUI;
    friend class ContentUI;
    
private:

    Ptr<GameObject> m_ProtoObj{}; // 프로토타입 오브젝트
    
    static set<GameObject*> m_setPrefabObjects; // 프리팹 ProtoObj의 경우, Layer에 속해 있지 않지만 예외적으로 LayerIdx를 수정 가능하도록 하게끔 set에 담아둔다.
    
public:
    
    APrefab();
    virtual ~APrefab() override;
    
    CLONE(APrefab);
    
public:
    
    // 원본 GameObject에서 Clone시킨 GameObject로 프로토타입 오브젝트 새로 생성
    bool SetProtoObj(const Ptr<GameObject>& _Object);

    /// <summary>
    /// 프로토타입 클론 시켜 생성한 객체 Return
    /// </summary>
    GameObject* Instantiate();

    /// <summary>
    /// m_SpawnLayerIdx로 현재 Level에 CreateObject 처리까지 처리 
    /// </summary>
    GameObject* InstantiateAndSpawnToCurLevel();

    /// <summary>
    /// 해당 GameObject가 Prefab set에 있는지 검사하여, Prefab ProtoObject 원본인지 검사 
    /// </summary>
    static bool IsPrefabPrototypeObject(const Ptr<GameObject>& _Object); 

public:
    virtual HRESULT Save(const wstring& _FilePath) override;
    virtual HRESULT Load(const wstring& _FilePath) override;
    
};
