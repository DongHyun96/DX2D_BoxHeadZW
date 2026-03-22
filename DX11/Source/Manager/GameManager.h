#pragma once

class GameManager : public Singleton<GameManager>
{
    
    SINGLE(GameManager);

    /// 주의 : 여기서 Ptr로 잡아둔 GameObject의 경우, Destroy 처리가 제대로 이루어지지 않는다 (Garbage를 비워도 여기에 Reference를 들고 있기 때문)
    
private:
    
    Ptr<GameObject> m_PlayerObject{};
    
public:
    
    Ptr<GameObject>	GetPlayerObject() const { return m_PlayerObject; }
    void SetPlayerObject(const Ptr<GameObject>& _PlayerObject) { m_PlayerObject = _PlayerObject; }
    
};
