#include "pch.h"
#include "GameObjectRefHolder.h"

#include "GameEngine/04.Asset/04.Level/ALevel.h"

GameObjectRefHolder::GameObjectRefHolder()
{
}

GameObjectRefHolder::~GameObjectRefHolder()
{
    if (m_GameObject) m_GameObject->RemoveDestroyDeleteDelegate(this);
}

GameObjectRefHolder::GameObjectRefHolder(const GameObjectRefHolder& _Origin)
    : m_GameObject(_Origin.m_GameObject) // 주의 : Level 복사 시(Stop to play), AfterInitGuidTable 시점에서 제대로 된 GO Reference 연결처리할 것
    , m_RefGUID(_Origin.m_RefGUID)
{
    // 전달받은 GO 래퍼런스가 유효하다면, OnDestroy 및 OnDelete Delegate 구독 처리
    if (m_GameObject)
    {
        m_GameObject->AddDestroyDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
        m_GameObject->AddDeleteDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
    }
}

// 소유권 이전 및 기존 객체 사용불가 처리
GameObjectRefHolder::GameObjectRefHolder(GameObjectRefHolder&& _Origin) noexcept
    : m_GameObject(_Origin.m_GameObject)
    , m_RefGUID(_Origin.m_RefGUID)
{
    // 전달받은 GO 래퍼런스가 유효하다면, OnDestroy 및 OnDelete Delegate 구독 처리
    if (m_GameObject)
    {
        m_GameObject->AddDestroyDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
        m_GameObject->AddDeleteDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
    }

    // 원본의 래퍼런스는 연결 해제
    _Origin.SetGameObject(nullptr);
}

GameObjectRefHolder& GameObjectRefHolder::operator=(const GameObjectRefHolder& _Other)
{
    if (this == &_Other) return *this;

    // 기존에 유효한 GameObject Reference를 들고 있었던 경우, 해당 GO의 OnDestroy, OnDelete Delegate 구독 해제
    if (m_GameObject)
        m_GameObject->RemoveDestroyDeleteDelegate(this);

    // 새롭게 받은 값으로 래퍼런스 잡기
    this->m_RefGUID    = _Other.m_RefGUID;
    this->m_GameObject = _Other.m_GameObject;

    // 새로이 받은 래퍼런스 GO가 유효하다면, OnDestroy및 OnDelete Delegate 구독 처리
    if (m_GameObject)
    {
        m_GameObject->AddDestroyDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
        m_GameObject->AddDeleteDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
    }
    
    return *this;
}

GameObjectRefHolder& GameObjectRefHolder::operator=(GameObjectRefHolder&& _Other) noexcept
{
    if (this == &_Other) return *this;

    // 기존에 유효한 GameObject Reference를 들고 있었던 경우, 해당 GO의 OnDestroy, OnDelete Delegate 구독 해제
    if (m_GameObject)
        m_GameObject->RemoveDestroyDeleteDelegate(this);
    
    // 소유권 이전 및 기존 객체 사용불가 처리
    m_RefGUID    = _Other.m_RefGUID;
    m_GameObject = _Other.m_GameObject;

    // 새로이 받은 래퍼런스 GO가 유효하다면, OnDestroy및 OnDelete Delegate 구독 처리
    if (m_GameObject)
    {
        m_GameObject->AddDestroyDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
        m_GameObject->AddDeleteDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
    }
    
    _Other.SetGameObject(nullptr); // 원본은 연결 해제
    
    return *this;
}

void GameObjectRefHolder::SetGameObject(GameObject* _GameObject)
{
    // 기존에 GameObject 레퍼런스가 잡혀있었을 때, 해당 GO의 DestroyDelegate 해제
    if (m_GameObject) m_GameObject->RemoveDestroyDeleteDelegate(this);
        
    
    m_GameObject = _GameObject;
    m_RefGUID    = (m_GameObject == nullptr) ? GUID_NULL : m_GameObject->GetGUID();
    
    
    // 새로 들어온 GO가 Valid하다면, 새로이 Destroy Delegate를 잡아준다
    if (m_GameObject)
    {
        m_GameObject->AddDestroyDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
        m_GameObject->AddDeleteDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
    }
}

void GameObjectRefHolder::OnGameObjectDestroyOrDelete()
{
    // 레퍼런스 연결 끊기
    m_GameObject = nullptr;
    m_RefGUID    = GUID_NULL;
}

void GameObjectRefHolder::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_RefGUID, sizeof(GUID), 1, _File);
}

void GameObjectRefHolder::LoadFromLevelFile(FILE* _File)
{
    fread(&m_RefGUID, sizeof(GUID), 1, _File);
}

void GameObjectRefHolder::LinkReferenceToGameObject(const Ptr<ALevel>& _Level)
{
    if (!_Level)
    {
        DebugUtil::AddDebugLog("[GameObjectRefHolder::LinkReferenceToGameObject] : Invalid Level received");
        return;
    }
    
    // 이전에 기록받은 GUID를 통해 Level의 GUIDTable에서 실질적인 GameObject Reference 연결
    m_GameObject = _Level->GetObjectByGUID(m_RefGUID);

    // 연결이 정상적으로 되었다면, Destroy 처리에 대한 Delegate binding 처리
    if (m_GameObject)
    {
        m_GameObject->AddDestroyDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
        m_GameObject->AddDeleteDelegate(this, bind(&GameObjectRefHolder::OnGameObjectDestroyOrDelete, this));
    }
}
