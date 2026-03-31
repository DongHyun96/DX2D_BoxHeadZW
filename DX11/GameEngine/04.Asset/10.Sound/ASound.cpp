#include "pch.h"
#include "ASound.h"

#include "GameEngine/01.Engine/Engine.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"


FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
                             , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                             , void* commanddata1, void* commanddata2);

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
	if (_iLoopCount <= -1)
	{
		assert(nullptr);
	}

	// 중첩재생 X + 이미 다른채널에서 재생중 ==> 재생 불가
	if (!_bOverlap && !m_listChannel.empty())
	{
		return E_FAIL;
	}

	_iLoopCount -= 1;

	FMOD::Channel* pChannel = nullptr;
	FMOD_SYSTEM->playSound(m_Sound, nullptr, false, &pChannel);

	// 재생을 했는데, 재생중인 채널이 없다 --> 실패
	if (nullptr == pChannel)
		return E_FAIL;

	pChannel->setVolume(_fVolume);

	pChannel->setCallback(&CHANNEL_CALLBACK);
	pChannel->setUserData(this);

	pChannel->setMode(FMOD_LOOP_NORMAL);
	pChannel->setLoopCount(_iLoopCount);

	// 어떤 채널에서 Sound 가 재생중인지 기록
	m_listChannel.push_back(pChannel);

	int iIdx = -1;
	pChannel->getIndex(&iIdx);

	return iIdx;
}

void ASound::Stop()
{
	list<FMOD::Channel*>::iterator iter{};

	while (!m_listChannel.empty())
	{
		iter = m_listChannel.begin();
		(*iter)->stop();
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

	string path(_FilePath.begin(), _FilePath.end());
	FMOD_RESULT result = FMOD_SYSTEM->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_Sound);
	if (FMOD_OK != result)
	{
		assert(nullptr);
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

// =========
// Call Back
// =========
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2)
{
	FMOD::Channel* cppchannel = (FMOD::Channel*)channelcontrol;
	ASound* pSound = nullptr;

	switch (controltype)
	{
	// 사운즈 재생 종료시 발생하는 이벤트
	case FMOD_CHANNELCONTROL_CALLBACK_END:
	{
		cppchannel->getUserData((void**)&pSound);
		pSound->RemoveChannel(cppchannel);
	}
	break;
	}

	return FMOD_OK;
}