#include "pch.h"
#include "ASound.h"

#include "GameEngine/01.Engine/Engine.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include <FMOD/fmod_errors.h>
#include <algorithm>
#include <cctype>


/*FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
                             , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                             , void* commanddata1, void* commanddata2);*/

namespace
{
    int ConvertToFmodLoopCount(int _iLoopCount)
    {
        if (_iLoopCount <= -1)
        {
            assert(nullptr);
            return 0;
        }

        // 사용자 입력: 0(무한), 1(1회), 2(2회) ...
        // FMOD 입력: -1(무한), 0(1회), 1(2회) ...
        return _iLoopCount - 1;
    }

    void PruneStoppedChannels(list<FMOD::Channel*>& _ChannelList)
    {
        for (auto it = _ChannelList.begin(); it != _ChannelList.end();)
        {
            FMOD::Channel* channel = *it;
        	
            bool isPlaying = false;

            if (channel == nullptr || FMOD_OK != channel->isPlaying(&isPlaying) || !isPlaying)
            {
                it = _ChannelList.erase(it);
                continue;
            }

            ++it;
        }
    }

    int SetupChannelAndGetIndex(FMOD::Channel* _Channel, ASound* _OwnerSound, int _FmodLoopCount, float _Volume)
    {
        if (_Channel == nullptr)
            return E_FAIL;

        _Channel->setMode(FMOD_LOOP_NORMAL);
        _Channel->setLoopCount(_FmodLoopCount);
        _Channel->setVolume(_Volume);

        // _Channel->setCallback(&CHANNEL_CALLBACK);
        _Channel->setUserData(_OwnerSound);

        int channelIdx = -1;
        _Channel->getIndex(&channelIdx);
        return channelIdx;
    }
}

ASound::ASound(bool _EngineRes)
	: Asset(ASSET_TYPE::SOUND)
	, m_Sound(nullptr)
{
}

ASound::~ASound()
{
	if (nullptr != m_Sound)
	{
		FMOD_RESULT result = m_Sound->release();
		m_Sound = nullptr;
	}
}

int ASound::Play(int _iLoopCount, float _fVolume, bool _bOverlap)
{
    const int fmodLoopCount = ConvertToFmodLoopCount(_iLoopCount);

    // 이미 끝난 채널 포인터 정리
    PruneStoppedChannels(m_listChannel);

    // 비중첩 모드 + 현재 재생 중이면 신규 요청 무시
    if (!_bOverlap && !m_listChannel.empty())
        return E_FAIL;

	DebugUtil::AddDebugLog("Channel size before : " + to_string(m_listChannel.size()), DEF_COLOR_CYAN, 5.f);
	
    FMOD::Channel* pChannel = nullptr;
    const FMOD_RESULT playResult = FMOD_SYSTEM->playSound(m_Sound, nullptr, false, &pChannel);
    if (FMOD_OK != playResult)
    {
        DebugUtil::AddDebugLog(string("[ASound::Play] playSound failed : ") + FMOD_ErrorString(playResult), DEF_COLOR_RED, 5.f);
        return E_FAIL;
    }

    const int channelIdx = SetupChannelAndGetIndex(pChannel, this, fmodLoopCount, _fVolume);
    if (channelIdx == E_FAIL)
        return E_FAIL;

    m_listChannel.push_back(pChannel);
	
	DebugUtil::AddDebugLog("Channel size After : " + to_string(m_listChannel.size()), DEF_COLOR_CYAN, 5.f);
	DebugUtil::AddDebugLog("", DEF_COLOR_CYAN, 5.f);
    return channelIdx;
}

int ASound::PlayNonOverlapFromStart(int _iLoopCount, float _fVolume)
{
    const int fmodLoopCount = ConvertToFmodLoopCount(_iLoopCount);

    // 이미 끝난 채널 포인터 정리
    PruneStoppedChannels(m_listChannel);

    // 이미 재생 중인 채널이 있으면 해당 채널을 처음부터 재생 (신규 채널 생성 안 함)
    if (!m_listChannel.empty())
    {
        auto itPlaying = m_listChannel.begin();
        FMOD::Channel* targetChannel = *itPlaying;

        vector<FMOD::Channel*> channelsToStop;
        for (auto it = next(itPlaying); it != m_listChannel.end(); ++it)
        {
            if (*it != nullptr)
                channelsToStop.push_back(*it);
        }
        m_listChannel.erase(next(itPlaying), m_listChannel.end());

        for (FMOD::Channel* ch : channelsToStop)
            ch->stop();

        const int channelIdx = SetupChannelAndGetIndex(targetChannel, this, fmodLoopCount, _fVolume);
        if (channelIdx == E_FAIL)
            return E_FAIL;

        targetChannel->setPosition(0, FMOD_TIMEUNIT_MS);
        targetChannel->setPaused(false);
        return channelIdx;
    }

    FMOD::Channel* pChannel = nullptr;
    const FMOD_RESULT playResult = FMOD_SYSTEM->playSound(m_Sound, nullptr, false, &pChannel);
    if (FMOD_OK != playResult)
    {
        DebugUtil::AddDebugLog(string("[ASound::PlayNonOverlapFromStart] playSound failed : ") + FMOD_ErrorString(playResult), DEF_COLOR_RED, 5.f);
        return E_FAIL;
    }

    const int channelIdx = SetupChannelAndGetIndex(pChannel, this, fmodLoopCount, _fVolume);
    if (channelIdx == E_FAIL)
        return E_FAIL;

    m_listChannel.push_back(pChannel);
    return channelIdx;
}

void ASound::Stop()
{
	
	auto tempChannelList = m_listChannel;
	m_listChannel.clear();

	for (FMOD::Channel* pChannel : tempChannelList)
	{
		pChannel->stop();
	}
}

void ASound::SetVolume(float _f, int _iChannelIdx)
{
	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();

	int iIdx = -1;
	for (; iter != m_listChannel.end(); ++iter)
	{
		(*iter)->getIndex(&iIdx);
		if (_iChannelIdx == iIdx)
		{
			(*iter)->setVolume(_f);
			return;
		}
	}
}

void ASound::RemoveChannel(FMOD::Channel* _pTargetChannel)
{
	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();
	for (; iter != m_listChannel.end(); ++iter)
	{
		if (*iter == _pTargetChannel)
		{
			m_listChannel.erase(iter);
			return;
		}
	}
}

HRESULT ASound::Load(const wstring& _FilePath)
{

	// MetaData에서 Asset GUID 및 파일해시값 찾기
	m_FileHash = CalculateFileHash64(_FilePath); 
	
	GUID Guid{};
	if (!AssetMgr::GetInst()->GetSoundAssetGuidByFileHash(m_FileHash, Guid))
	{
		// 해당 FileHash값에 대응하는 메타 데이터가 없었던 상황 (새로 추가된 Sound)
		// 새로운 Guid 할당해서 메타데이터 파일 새로 저장
		GetGuid();
		this->Save(_FilePath);
	}
	else SetGuid(Guid);
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 기존 Sound Load 처리
	/*string path(_FilePath.begin(), _FilePath.end());
	FMOD_RESULT result = FMOD_SYSTEM->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_Sound);
	if (FMOD_OK != result)
	{
		assert(nullptr);
	}

	return S_OK;*/
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	string path = string(_FilePath.begin(), _FilePath.end()); 
    
	// 확장자 검사
	size_t extPos = path.find_last_of('.');
	string ext = (extPos != string::npos) ? path.substr(extPos + 1) : "";
    transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
    
	// MP3 / OGG SFX 는 압축샘플로 로드해서 Overlap 재생이 가능하도록 처리
	// (Stream은 동일 Sound 동시 재생 시 제약이 있어 Overlap 체감이 사라짐)
	FMOD_MODE mode = FMOD_DEFAULT;
	if (ext == "mp3" || ext == "ogg")
		mode |= FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD_RESULT result = FMOD_SYSTEM->createSound(path.c_str(), mode, nullptr, &m_Sound);

	if (FMOD_OK != result)
	{
        DebugUtil::AddDebugLog(string("[ASound::Load] createSound failed : ") + FMOD_ErrorString(result), DEF_COLOR_RED, 10.f);
		assert(false); // 로딩 실패 처리
		return E_FAIL;
	}

	return S_OK;
	
}

HRESULT ASound::Save(const wstring& _FilePath)
{
	// Sound 용 MetaData 먼저 저장
    
	// FileHash 값이 초기화 되지 않은 Asset -> 파일해시값 계산해서 넣어주기
	if (m_FileHash == 0) m_FileHash = CalculateFileHash64(_FilePath);
    
	FILE* pFile{}; // 파일스트림 커널
	// 추후 Sound 파일명을 바꿨을 때, 기존의 metadata가 남아서 쌓일 수 있기 때문에 고유의 파일해시값으로 파일명을 잡아줌
	const wstring MetaFilePath = CONTENT_PATH + L"\\_Meta\\_SoundMeta\\" + to_wstring(m_FileHash) + L".soundmeta";
    
	// Sound 메타파일 저장
    
	if (_wfopen_s(&pFile, MetaFilePath.c_str(), L"wb") != 0 || !pFile)
	{
		DebugUtil::AddDebugLog(L"[ASound::Save] : Open MetaFile failed!");
		return E_FAIL;
	}

	fwrite(&m_FileHash,     sizeof(uint64_t),   1, pFile);    // 파일해시값 저장
	fwrite(&GetGuidRef(),   sizeof(GUID),       1, pFile);      // Asset Guid 저장
    
	fclose(pFile);
    
	return S_OK;
}

/*
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2)
{
	FMOD::Channel* cppchannel = (FMOD::Channel*)channelcontrol;
	ASound* pSound = nullptr;

    if (controltype == FMOD_CHANNELCONTROL_CHANNEL &&
        callbacktype == FMOD_CHANNELCONTROL_CALLBACK_END)
    {
        cppchannel->getUserData((void**)&pSound);
        if (pSound)
        {
        	pSound->RemoveChannel(cppchannel);
        	// DebugUtil::AddDebugLog("Sound End", DEF_COLOR_WHITE, 10.f);
        }
            
    }

	return FMOD_OK;
}
*/
