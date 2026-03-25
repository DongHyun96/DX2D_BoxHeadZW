#pragma once
#include "GameEngine/03.Manager/06.RenderMgr/RenderMgr.h"


// Task
void CreateObject(GameObject*_Object, int _LayerIdx);

// DebugRender 관련
void DrawDebugRect
(
    const Vec3& _Pos,
    const Vec3& _Scale,
    const Vec3& _Rot,
    const Vec4& _Color,
    float       _Duration,
    bool        EnableDepthTest = false
);


void DrawDebugRect
(
    const Matrix&   _matWorld,
    const Vec4&     _Color,
    float           _Duration,
    bool            EnableDepthTest = false
);


void DrawDebugCircle
(
    const Vec3& _Pos,
    float       _Radius,
    const Vec4& _Color,
    float       _Duration,
    bool        EnableDepthTest = false
);

void DrawDebugLine
(
    const Vec3& _Start,
    const Vec3& _End,
    const Vec4& _Color,
    float       _Duration,
    bool        EnableDepthTest = false
);


wstring GuidToWString(const GUID& _Guid);

void SaveWString(FILE* _File, const wstring& _String);
wstring LoadWString(FILE* _File);

void SaveAssetRef(FILE* _File, Asset* _Asset);

template<typename T>
Ptr<T> LoadAssetRef(FILE* _File);

/// <summary>
/// TaskMgr를 통한 레벨 전환 처리
/// </summary>
/// <param name="_NextLevelName"> : 다음 Level Key wstring </param>
void ChangeLevel(const wstring& _NextLevelName);

void ChangeLevelState(LEVEL_STATE _NextState);

/// <summary>
/// Path 포멧의 wstring에서 파일명 wstring 추출 반환
/// </summary>
wstring GetFileName(const wstring& _Path);

/// <summary>
/// Path 포멧의 wstring에서 extension이 제거된 파일명 wstring 추출 반환
/// </summary>
wstring GetFileNameWithoutExtension(const wstring& _Path);

/// <summary>
/// Asset_Type에 해당하는 폴더 wstring 반환 (ex. Material Type -> L"Material\\"
/// </summary>
wstring GetAssetTypeFolderWString(ASSET_TYPE _Type);

/// <summary>
/// AssetType에 대응되는 Extension return 
/// </summary>
wstring GetAssetTypeExtension(ASSET_TYPE _Type);

/// <summary>
/// <para> AssetName에 부합되는 AssetKey 포멧 형태의 키값 wstring 반환 </para>
/// <para> 만일 겹치는 AssetKey가 존재한다면 indexing 포함한 키값으로 반환 </para>
/// </summary>
wstring GenerateNewAssetKeyBasedOnAssetName(ASSET_TYPE _AssetType, const wstring& _AssetName);

/// <summary>
/// <para> AssetName에 부합되는 AssetKey 포멧 형태의 키값 OutGenKey로 반환 </para>
/// <para> 만일 겹치는 AssetKey가 존재한다면 return false </para>
/// </summary>
bool GenerateNewAssetKeyBasedOnAssetName(ASSET_TYPE _AssetType, const wstring& _AssetName, wstring& _OutGeneratedKey);

/// <summary>
/// 해당 Path에 존재하는 File Hash 값 계산
/// </summary>
/// <param name="_FullPath"></param>
/// <returns> : 해당 경로에 존재하는 파일의 파일해시값 (해당 경로에 파일이 없다면 0을 반환) </returns>
uint64_t CalculateFileHash64(const wstring& _FullPath);


/// <summary>
/// FilePath의 윈도우 파일 삭제하기
/// </summary>
/// <param name="_FilePath"> : 파일 절대경로 </param>
/// <returns> : 제대로 삭제되지 않았다면 return false </returns>
bool RemoveWindowFile(const wstring& _FilePath);

/// <summary>
/// 해당 FilePath에 있는 Asset파일명 수정
/// 해당 Path에 파일이 없거나, 같은 확장자명이 아닌 다른 확장자명의 새로운 이름(다른 Asset 종류)을 받았다면 처리 x
/// </summary>
/// <param name="_FilePath"> : Target file의 fullPath </param>
/// <param name="_NewFileName"> : 파일명.확장자명 형태로 넘길 것 </param>
/// <returns> : 해당 Path에 파일이 없거나, 같은 확장자명이 아닐경우 return false </returns>
bool RenameWindowAssetFileName(const wstring& _FilePath, const wstring& _NewFileName);

/// <summary>
/// 해당 FilePath에 있는 Asset파일명을 수정 가능한지 체크
/// </summary>
bool CanRenameWindowAssetFileName(const wstring& _FilePath, const wstring& _NewFileName);

wstring GetExtension(const wstring& _FilePath);

string RenderDomainTypeToString(RENDER_DOMAIN _RenderDomain);
RENDER_DOMAIN StringToRenderDomain(const string& _RenderDomainString);


string AssetTypeToString(ASSET_TYPE _Type);

