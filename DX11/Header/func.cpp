#include "pch.h"
#include "func.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/07.TaskMgr/TaskMgr.h"
#include "GameEngine/04.Asset/04.Level/ALevel.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderCircle.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderPoint.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderRect.h"

#include "Source/Scripts/CCamMoveScript.h"
#include "Source/Scripts/CMonsterScript.h"
#include "Source/Scripts/CPlayerScript.h"

namespace
{
    bool RenameWindowAssetFileName(const wstring& _FilePath, const wstring& _NewFileName, bool _ExecuteRenamingIfPossible)
    {
        try
        {
            filesystem::path TargetPath(_FilePath);

            // 파일 존재 여부 확인
            if (!filesystem::exists(TargetPath) || !filesystem::is_regular_file(TargetPath))
                return false;
        
            // extension 일치여부 확인 (만약 다른 확장자명일 경우, 수정 불가하게끔 처리
            if (TargetPath.extension().wstring() != GetExtension(_NewFileName)) return false; 
        
            // 새로운 경로 생성 (기존 경로의 부모 디렉토리 + 새 파일명)
            filesystem::path NewPath = TargetPath.parent_path() / _NewFileName;
        
            // 타겟 경로에 이미 동일한 이름의 파일이 존재하는지 확인 (덮어쓰기 불가)
            if (filesystem::exists(NewPath)) return false;

            // 이름 변경 실행한다면 실질적으로 이름 바꾸기 처리
            if (_ExecuteRenamingIfPossible) filesystem::rename(TargetPath, NewPath);
            
            return true;
        }
        catch (const filesystem::filesystem_error& e)
        {
            return false;
        }
    }
}

void CreateObject(GameObject* _Object, int _LayerIdx)
{
    TaskInfo info = {};
    
    info.Type       = TASK_TYPE::CREATE_OBJECT;
    info.Param_0    = reinterpret_cast<DWORD_PTR>(_Object);
    info.Param_1    = _LayerIdx;
    
    TaskMgr::GetInst()->AddTask(info);
}

void DestroyObject(GameObject* _Object)
{
    if (!_Object || _Object->IsDead()) return;
    
    TaskInfo info = {};
    
    info.Type       = TASK_TYPE::DESTROY_OBJECT;
    info.Param_0    = reinterpret_cast<DWORD_PTR>(_Object);
    TaskMgr::GetInst()->AddTask(info);
}

void DrawDebugRect(const Vec3& _Pos, const Vec3& _Scale, const Vec3& _Rot, const Vec4& _Color, float _Duration, bool EnableDepthTest)
{
    DbgInfo info{};
    info.Shape      = DBG_SHAPE::RECT;
    info.Pos        = _Pos;
    info.Scale      = _Scale;
    info.Rotation   = _Rot;
    
    info.matWorld = XMMatrixIdentity();
    
    info.Color      = _Color;
    info.Life       = _Duration;
    info.Age        = 0.f;
    info.EnableDepthTest = EnableDepthTest;
        
    RenderMgr::GetInst()->AddDebugInfo(info);
}

void DrawDebugRect(const Matrix& _matWorld, const Vec4& _Color, float _Duration, bool EnableDepthTest)
{
    DbgInfo info{};
    info.Shape      = DBG_SHAPE::RECT;
    
    info.matWorld = _matWorld;
    
    info.Color      = _Color;
    info.Life       = _Duration;
    info.Age        = 0.f;

    info.EnableDepthTest = EnableDepthTest;
    
    RenderMgr::GetInst()->AddDebugInfo(info);
}

void DrawDebugCircle(const Vec3& _Pos, float _Radius, const Vec4& _Color, float _Duration, bool EnableDepthTest)
{
    DbgInfo info{};
    info.Shape      = DBG_SHAPE::CIRCLE;
    info.Pos        = _Pos;
    info.Scale      = Vec3(_Radius * 2.f, _Radius * 2.f, 0.f);
    info.Color      = _Color;
    info.Life       = _Duration;
    info.Age        = 0.f;

    info.EnableDepthTest = EnableDepthTest;
    
    RenderMgr::GetInst()->AddDebugInfo(info);
}

void DrawDebugLine(const Vec3& _Start, const Vec3& _End, const Vec4& _Color, float _Duration, bool EnableDepthTest)
{
    Vec3 dir = _End - _Start;
    dir.z = 0.f;

    const float len = dir.Length();
    if (len <= 0.0001f) return;

    const Vec3 mid  = (_Start + _End) * 0.5f;
    const float rad = atan2f(dir.y, dir.x);

    DbgInfo info{};
    info.Shape      = DBG_SHAPE::LINE;
    info.Pos        = mid;
    info.Scale      = Vec3(len, 1.f, 1.f);      // Y=두께(기본 1)
    info.Rotation   = Vec3(0.f, 0.f, rad);
    info.matWorld   = XMMatrixIdentity();

    info.Color           = _Color;
    info.Life            = _Duration;
    info.Age             = 0.f;
    info.EnableDepthTest = EnableDepthTest;

    RenderMgr::GetInst()->AddDebugInfo(info);
}

wstring GuidToWString(const GUID& _Guid)
{
    wchar_t szGuid[40] = {}; 
    if (StringFromGUID2(_Guid, szGuid, _countof(szGuid)) == 0) return L""; // 변환 실패 (일어나지 않음)

    wstring strGuid = szGuid;
    
    // 양 끝의 중괄호 삭제 (해당 이름으로 저장은 되지만, cmd나 다른 툴에서 특수문자로 인식되어 버릴 수 있는 가능성이 있어서 날려버림)
    strGuid.erase(remove(strGuid.begin(), strGuid.end(), L'{'), strGuid.end());
    strGuid.erase(remove(strGuid.begin(), strGuid.end(), L'}'), strGuid.end());

    return strGuid;
}

void SaveWString(FILE* _File, const wstring& _String)
{
    
    int Len = _String.length();
    fwrite(&Len, sizeof(int), 1, _File); // 문자열의 길이정보 저장
    fwrite(_String.data(), sizeof(wchar_t), Len, _File); // 실제 문자열 저장
}

wstring LoadWString(FILE* _File)
{
    // 문자열의 길이정보 저장
    int Len{};
    fread(&Len, sizeof(int), 1, _File);
        
    wchar_t buff[255]{};
    fread(buff, sizeof(wchar_t), Len, _File);
    
    return buff;
}

void SaveAssetRef(FILE* _File, Asset* _Asset)
{
    // Asset이 nullptr인지 아닌지 저장
    bool IsValid = _Asset;
    fwrite(&IsValid, sizeof(bool), 1, _File);
    
    
    // Asset의 Key, RelativePath 저장
    if (IsValid)
    {
        // GUID 먼저 저장
        GUID AssetGuid = _Asset->GetGuid(); // 여기서 순서상 Guid가 초기화되지 않은 Guid 일 수 있음 & ATexture의 경우, 다른 방식으로 처리를 좀 해주어야 함
        fwrite(&AssetGuid, sizeof(GUID), 1, _File);
        
        SaveWString(_File, _Asset->GetKey());
        SaveWString(_File, _Asset->GetRelativePath());

    }
}

wchar_t Buff[255] = {};

void ChangeLevel(const wstring& _NextLevelName)
{
    TaskInfo info{};
    
    wcscpy_s(Buff, 255, _NextLevelName.c_str());
    
    info.Type = TASK_TYPE::CHANGE_LEVEL;
    info.Param_0 = reinterpret_cast<DWORD_PTR>(Buff);
    
    TaskMgr::GetInst()->AddTask(info);
}

void ChangeLevelState(LEVEL_STATE _NextState)
{
    TaskInfo info{};
    
    info.Type = TASK_TYPE::CHANGE_LEVEL_STATE;
    info.Param_0 = static_cast<DWORD_PTR>(_NextState);
    
    TaskMgr::GetInst()->AddTask(info);
}

wstring GetFileName(const wstring& _Path)
{
    size_t pos = _Path.find_last_of(L"\\/");
    
    if (pos == wstring::npos) return _Path;
    return _Path.substr(pos + 1);
}

wstring GetFileNameWithoutExtension(const wstring& _Path)
{
    const wstring fileName = GetFileName(_Path);
    
    size_t pos = fileName.find_last_of(L".");
    
    if (pos == wstring::npos) return fileName;
    
    return fileName.substr(0, pos);
}

uint64_t CalculateFileHash64(const std::wstring& _FullPath)
{
    FILE* pFile = nullptr;
    
    if (_wfopen_s(&pFile, _FullPath.c_str(), L"rb") != 0 || !pFile) return 0; // 파일 열기 실패

    // FNV-1a 64-bit 표준 상수
    uint64_t hash        = 14695981039346656037ull; 
    const uint64_t prime = 1099511628211ull;

    unsigned char buf[65536]; 
    size_t readBytes = 0;

    while ((readBytes = fread(buf, 1, sizeof(buf), pFile)) > 0)
    {
        for (size_t i = 0; i < readBytes; ++i)
        {
            hash ^= buf[i];
            hash *= prime;
        }
    }

    fclose(pFile);
    return hash;
}

bool RemoveWindowFile(const wstring& _FilePath)
{
    if (_FilePath.empty())
    {
        DebugUtil::AddDebugLog(L"[RemoveWindowFile] Empty Path received");
        return false;
    }

    error_code ec{};
    
    bool result = filesystem::remove(_FilePath, ec);

    if (ec || !result)
    {
        DebugUtil::AddDebugLog(L"[RemoveWindowFile] RemoveWindowFile Failed!");
        return false;
    }

    return true;
}

bool RenameWindowAssetFileName(const wstring& _FilePath, const wstring& _NewFileName)
{
   return RenameWindowAssetFileName(_FilePath, _NewFileName, true);
}

bool CanRenameWindowAssetFileName(const wstring& _FilePath, const wstring& _NewFileName)
{
    return RenameWindowAssetFileName(_FilePath, _NewFileName, false);
}

wstring GetExtension(const wstring& _FilePath)
{
    return filesystem::path(_FilePath).extension().wstring();
}

string AssetTypeToString(ASSET_TYPE _Type)
{
    switch (_Type)
    {
    case ASSET_TYPE::MESH:              return "Mesh";
    case ASSET_TYPE::MATERIAL:          return "Material";
    case ASSET_TYPE::TEXTURE:           return "Texture";
    case ASSET_TYPE::SPRITE:            return "Sprite";
    case ASSET_TYPE::FLIPBOOK:          return "Flipbook";
    case ASSET_TYPE::TILEMAP:           return "TileMap";
    case ASSET_TYPE::GRAPHICS_SHADER:   return "Graphic Shader";
    case ASSET_TYPE::LEVEL:             return "Level";
    default :                           return "Invalid";
    }
}

wstring GetAssetTypeFolderWString(ASSET_TYPE _Type)
{
    switch (_Type)
    {
    case ASSET_TYPE::MATERIAL:          return L"Material\\";
    case ASSET_TYPE::TEXTURE:           return L"Texture\\";
    case ASSET_TYPE::SPRITE:            return L"Sprite\\";
    case ASSET_TYPE::FLIPBOOK:          return L"Flipbook\\";
    case ASSET_TYPE::TILEMAP:           return L"TileMap\\";
    case ASSET_TYPE::GRAPHICS_SHADER:   return L"Shader\\";
    case ASSET_TYPE::PREFAB:            return L"Prefab\\";
    case ASSET_TYPE::LEVEL:             return L"Level\\";
    default :                           return L"";         // Invalid
    }
}

wstring GetAssetTypeExtension(ASSET_TYPE _Type)
{
    switch (_Type)
    {
    case ASSET_TYPE::MESH:
        return L".mesh";
    case ASSET_TYPE::MATERIAL:
        return L".mtrl";
    case ASSET_TYPE::TEXTURE:
    case ASSET_TYPE::SOUND:
    case ASSET_TYPE::GRAPHICS_SHADER:
    case ASSET_TYPE::COMPUTE_SHADER:
        assert(nullptr);
        return L"";
    case ASSET_TYPE::SPRITE:
        return L".sprite";
    case ASSET_TYPE::FLIPBOOK:
        return L".flip";
    case ASSET_TYPE::TILEMAP:
        return L".tilemap";
    case ASSET_TYPE::PREFAB:
        return L".pref";
    case ASSET_TYPE::LEVEL:
        return L".lv";
    }
}

wstring GenerateNewAssetKeyBasedOnAssetName(ASSET_TYPE _AssetType, const wstring& _AssetName)
{
    const wstring Ext           = GetAssetTypeExtension(_AssetType);
    const wstring AssetFolder   = GetAssetTypeFolderWString(_AssetType);
    const wstring AssetKey      = AssetFolder + _AssetName + Ext;
    
    if (!AssetMgr::GetInst()->Find(_AssetType, AssetKey, false)) return AssetKey; // 겹치는 키가 존재하지 않음
    
    // 겹치는 키가 존재한다면, indexing 붙여서 처리
    int i = 0;
    while (true)
    {
        wchar_t Num[50] = {};
        swprintf_s(Num, 50, L"_%d", i);

        const wstring AssetKey = wstring(AssetFolder + _AssetName + Num + Ext);
        
        if (!AssetMgr::GetInst()->Find(_AssetType, AssetKey))
            return AssetKey;

        i++;
    }
}

bool GenerateNewAssetKeyBasedOnAssetName(ASSET_TYPE _AssetType, const wstring& _AssetName, wstring& _OutGeneratedKey)
{
    _OutGeneratedKey = L"";
    
    const wstring Ext           = GetAssetTypeExtension(_AssetType);
    const wstring AssetFolder   = GetAssetTypeFolderWString(_AssetType);
    const wstring AssetKey      = AssetFolder + _AssetName + Ext;
    
    if (AssetMgr::GetInst()->Find(_AssetType, AssetKey, false)) return false; // 겹치는 키가 존재하지 않음
    
    _OutGeneratedKey = AssetKey;
}

bool IsValid(Ptr<GameObject>& _Object)
{
    if (!_Object || _Object->IsDead())
    {
        // Valid하지 않은 Object에 대한 IsValid 호출이 들어왔을 때, 원본값 또한 nullptr로 처리해버린다
        _Object = nullptr;
        return false;
    }
    
    return true;
}
