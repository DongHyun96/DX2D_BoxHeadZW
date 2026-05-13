#include "pch.h"
#include "GameObjectRefHolder.h"

#include "GameEngine/04.Asset/04.Level/ALevel.h"

GameObjectRefHolder::GameObjectRefHolder()
{
}

GameObjectRefHolder::GameObjectRefHolder(const GameObjectRefHolder& _Origin)
    : m_RefGUID(_Origin.m_RefGUID) // GUID만 복사하고, GameObject는 다시금 연결처리를 해주어야 함
{
}

// 소유권 이전 및 기존 객체 사용불가 처리
GameObjectRefHolder::GameObjectRefHolder(GameObjectRefHolder&& _Origin) noexcept
    : m_GameObject(_Origin.m_GameObject)
    , m_RefGUID(_Origin.m_RefGUID)
    , m_DelegateOnGameObjectDestroyed(move(_Origin.m_DelegateOnGameObjectDestroyed))
{
    _Origin.m_RefGUID    = GUID_NULL;
    _Origin.m_GameObject = nullptr;
}

GameObjectRefHolder& GameObjectRefHolder::operator=(const GameObjectRefHolder& _Other)
{
    if (this == &_Other) return *this;

    this->m_GameObject                    = _Other.m_GameObject;
    this->m_RefGUID                       = _Other.m_RefGUID;
    this->m_DelegateOnGameObjectDestroyed = _Other.m_DelegateOnGameObjectDestroyed;
    
    return *this;
}

GameObjectRefHolder& GameObjectRefHolder::operator=(GameObjectRefHolder&& _Other) noexcept
{
    if (this == &_Other) return *this;

    // 소유권 이전 및 기존 객체 사용불가 처리

    this->m_GameObject                     = _Other.m_GameObject;
    this->m_RefGUID                        = _Other.m_RefGUID;
    this->m_DelegateOnGameObjectDestroyed  = move(_Other.m_DelegateOnGameObjectDestroyed); // 알아서 소유권 이전 처리됨(기존 원본의 Delegate nullptr로 처리됨)
    
    // 소유권 이전
    _Other.m_GameObject                    = nullptr;
    _Other.m_RefGUID                       = GUID_NULL;
    
    return *this;
}

GameObjectRefHolder::GameObjectRefHolder(GameObject* _Object)
    : m_GameObject(_Object)
{
    if (_Object) m_RefGUID = _Object->GetGUID();
}

GameObjectRefHolder::GameObjectRefHolder(GUID _Guid)
    : m_RefGUID(_Guid)
{
}

GameObjectRefHolder::~GameObjectRefHolder()
{
    if (m_GameObject) m_GameObject->RemoveDestroyDelegate(reinterpret_cast<DWORD_PTR>(this));
}

void GameObjectRefHolder::SetGameObject(GameObject* _GameObject)
{
    // 기존에 GameObject 레퍼런스가 잡혀있었을 때, 해당 GO의 DestroyDelegate 해제
    if (m_GameObject) m_GameObject->RemoveDestroyDelegate(reinterpret_cast<DWORD_PTR>(this));
    
    m_GameObject = _GameObject;
    m_RefGUID    = (m_GameObject == nullptr) ? GUID_NULL : m_GameObject->GetGUID();
    
    
    // 새로 들어온 GO가 Valid하다면, 새로이 Destroy Delegate를 잡아준다
    if (m_GameObject) m_GameObject->AddDestroyDelegate(reinterpret_cast<DWORD_PTR>(this), bind(&GameObjectRefHolder::OnGameObjectDestroyed, this));
}

void GameObjectRefHolder::OnGameObjectDestroyed()
{
    if (m_DelegateOnGameObjectDestroyed) m_DelegateOnGameObjectDestroyed(m_GameObject);
    
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
    if (!_Level) return;
    m_GameObject = _Level->GetObjectByGUID(m_RefGUID);

    // 정상적으로 불러와졌다면, Destroy 처리에 대한 Delegate binding 처리
    if (m_GameObject) m_GameObject->AddDestroyDelegate(reinterpret_cast<DWORD_PTR>(this), bind(&GameObjectRefHolder::OnGameObjectDestroyed, this));
}
