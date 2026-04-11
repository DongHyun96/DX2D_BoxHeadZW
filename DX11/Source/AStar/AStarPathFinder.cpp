#include "pch.h"
#include "AStarPathFinder.h"

#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"

ASNode* AStarPathFinder::m_FieldNodes[CELL_ROW_COUNT][CELL_ROW_COUNT];
UINT AStarPathFinder::m_CurrentEpoch{};
unordered_map<PathCacheKey, stack<CellCoord>, PathCacheKeyHash> AStarPathFinder::m_PathCache;
const UINT AStarPathFinder::MAX_CACHE_SIZE = 5000;

AStarPathFinder::AStarPathFinder()
{
    for (UINT i = 0; i < CELL_ROW_COUNT; i++)
        for (UINT j = 0; j < CELL_ROW_COUNT; j++)
            m_FieldNodes[i][j] = new ASNode({ j, i });
}

AStarPathFinder::~AStarPathFinder()
{
    for (UINT i = 0; i < CELL_ROW_COUNT; i++)
        for (UINT j = 0; j < CELL_ROW_COUNT; j++)
            delete m_FieldNodes[i][j];
        
}

bool AStarPathFinder::GetPath(const CellCoord& start, const CellCoord& dest, stack<CellCoord>& _OutPath)
{
	// _OutPath 스택 초기화
	stack<CellCoord>().swap(_OutPath);

	// 이미 도달한 경로
	if (start == dest) return true;

	// start나 dest 자체가 이동 불가능한 경로
	CBackgroundTile* BackgroundCellManager = GM->GetBackgroundCellManager();
	if (!BackgroundCellManager->IsCellAvailable(dest) || !BackgroundCellManager->IsCellAvailable(start)) return false;

	// 캐시에서 경로 확인
	PathCacheKey cacheKey{ start, dest };
	auto cacheIter = m_PathCache.find(cacheKey);
	if (cacheIter != m_PathCache.end())
	{
		_OutPath = cacheIter->second; // 캐시된 경로 복사
		return !_OutPath.empty(); // 경로가 있으면 true, 없으면 false 반환
	}

	if (!BackgroundCellManager) return false;

	// 8 way movements (상, 하, 좌, 우, 좌상, 우상, 좌하, 우하)
	static const int dx[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };
	static const int dy[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };

	// 이동 방향에 따른 비용 (대각의 경우 루트2 -> 대략 10을 곱해서 처리)
	static const int cost[8] = { 10, 10, 10, 10, 14, 14, 14, 14 };

	// 이번 길찾기 회차 증가 시키기
	++m_CurrentEpoch;

	priority_queue<ASNode*, vector<ASNode*>, CompareNode> pq{};

	UINT startH         = GetDiagonalDist(start, dest);
	ASNode* StartNode   = m_FieldNodes[start.y][start.x];

	// StartNode값 이번 Epoch으로 초기화 처리
	StartNode->SearchEpoch  = m_CurrentEpoch;
	StartNode->Visited      = false;
	StartNode->G            = 0;
	StartNode->H            = startH;
	StartNode->F            = startH;
	StartNode->Parent       = nullptr;

	pq.push(StartNode);

	while (!pq.empty())
	{
		ASNode* curNode = pq.top();
		pq.pop();

		// 이미 방문한 Node일 경우
		if (curNode->Visited) continue;

		curNode->Visited = true; // 방문 마크 처리

		if (curNode->Coord == dest) // 최단경로 탐색 완료
		{
			_OutPath.push(curNode->Coord); // 도착 지점까지도 넣어준다
			curNode = curNode->Parent;

			// parent를 타고가며, 탐색한 경로를 모두 stack에 넣어준다
			while (curNode->Coord != start)
			{
				_OutPath.push(curNode->Coord);
				curNode = curNode->Parent;
			}

			// 캐시에 저장 (캐시 크기 제한 체크)
			if (m_PathCache.size() >= MAX_CACHE_SIZE)
			{
				m_PathCache.clear(); // 캐시 초과 시 전체 비우기
			}
			m_PathCache[cacheKey] = _OutPath;

			return true;
		}

		for (UINT i = 0; i < 8; i++)
		{
			int nx = curNode->Coord.x + dx[i];
			int ny = curNode->Coord.y + dy[i];

			CellCoord nextCoord = { nx, ny };

			// 범위 판정 및, 장애물이 존재하는 좌표인지 체크 모두 들어가 있다
			if (!BackgroundCellManager->IsCellAvailable(nextCoord)) continue;

			// 대각선 이동일 경우, 코너 커팅 검사도 추가로 해주어야 한다
			if (i >= 4) // 대각선 이동(인덱스 4~7)일 경우 코너 커팅 검사
			{
				CellCoord check1 = { curNode->Coord.x + dx[i], curNode->Coord.y }; // X축 방향 타일
				CellCoord check2 = { curNode->Coord.x, curNode->Coord.y + dy[i] }; // Y축 방향 타일

				// 둘 중 하나라도 막혀있으면 대각선으로 진입 불가
				if (!BackgroundCellManager->IsCellAvailable(check1) || !BackgroundCellManager->IsCellAvailable(check2))
					continue;
			}

			ASNode* NextNode = m_FieldNodes[nextCoord.y][nextCoord.x];

			// 이번 길찾기 회차에서 처음 등록된 노드 -> 새로이 초기화 처리를 먼저 해줌
			if (NextNode->SearchEpoch != m_CurrentEpoch)
			{
				NextNode->SearchEpoch   = m_CurrentEpoch;
				NextNode->F             = INF;
				NextNode->Visited       = false;
			}

			// f, g, h값 구하기
			const UINT G = curNode->G + cost[i]; // 발자국 점수 처리 (대각의 경우 살짝 늘어난 값 합산 처리됨)
			const UINT H = GetDiagonalDist(nextCoord, dest);
			const int  F = G + H;

			if (F < NextNode->F)
			{
				NextNode->G = G;
				NextNode->H = H;
				NextNode->F = F;
				NextNode->Parent = curNode;
				pq.push(NextNode);
			}
		}
	}

	// 도달할 수 있는 경로가 없다 -> 실패한 경로도 캐시에 저장 (다시 탐색 안 함)
	if (m_PathCache.size() >= MAX_CACHE_SIZE)
	{
		m_PathCache.clear();
	}
	m_PathCache[cacheKey] = _OutPath; // empty stack 저장
	return false;
}

void AStarPathFinder::Init()
{
	for (UINT i = 0; i < CELL_ROW_COUNT; i++)
		for (UINT j = 0; j < CELL_ROW_COUNT; j++)
		{
			ASNode* Node        = m_FieldNodes[i][j]; 
			Node->Parent        = nullptr;
			Node->G             = 0;
			Node->H             = 0;
			Node->F             = INF;
			Node->Visited       = false;
			Node->SearchEpoch   = 0;
		}

	m_CurrentEpoch = 0;
	m_PathCache.clear(); // 경로 캐시 초기화
}

UINT AStarPathFinder::GetDiagonalDist(const CellCoord& _Coord1, const CellCoord& _Coord2)
{
    const int dx = abs(static_cast<int>(_Coord1.x) - static_cast<int>(_Coord2.x));
    const int dy = abs(static_cast<int>(_Coord1.y) - static_cast<int>(_Coord2.y));
    
    // 직선 이동 비용(10)과 대각선 이동 비용(14)을 적용한 공식
    return 10 * (dx + dy) + (14 - 2 * 10) * min(dx, dy); 
}
