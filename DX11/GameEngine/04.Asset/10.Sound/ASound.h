#pragma once

class ASound : public Asset
{
    
    friend class CAssetMgr;

private:
    
    uint64_t m_FileHash{}; // 파일 해시값
    
private:
    FMOD::Sound*            m_Sound{};        // Sound 객체
    list<FMOD::Channel*>	m_listChannel{};  // Sound 가 재생되고 있는 채널 리스트

public:
    
    ASound(bool _EngineRes = false);
    virtual ~ASound() override;
    CLONE(ASound);
    
public:
    // _iRoopCount : 0 (반복재생),  _fVolume : 0 ~ 1(Volume), _bOverlap : 같은 사운드를 중첩해서 켤 수 있는지
    int Play(int _iLoopCount, float _fVolume, bool _bOverlap);

    /// <summary>
    /// Overlap 허용하지 않는 Sound이고, Play 시 이미 재생 중인 Sound를 Rewind해서 처음부터 재생 처리해야할 때 사용할 것 
    /// </summary>
    int PlayNonOverlapFromStart(int _iLoopCount, float _fVolume);
    
    void RemoveChannel(FMOD::Channel* _pTargetChannel);
    void Stop();

    // 0 ~ 1
    void SetVolume(float _f, int _iChannelIdx);

public:
    virtual HRESULT Load(const wstring& _FilePath) override;
    virtual HRESULT Save(const wstring& _FilePath) override;


};
