#pragma once

#define INF INT_MAX	// 거리 무한대 defined

struct ASNode
{
    ASNode() {}
    ASNode(const CellCoord& coord)
        : Coord(coord) {}


    CellCoord   Coord{};	// 이 node의 위치
    ASNode*	    Parent{};

    UINT G{};				// 현 발자국 점수
    UINT H{};				// 휴리스틱 값
    int F = INF;			// 합산 거리 점수
    
    bool Visited{};
    UINT SearchEpoch{}; // 이 노드가 마지막으로 초기화된 길찾기 회차
};

/* A* 알고리즘 내에서 prioriy_queue의 Priority 조건 */
struct CompareNode
{
    bool operator()(const ASNode* _Node1, const ASNode* _Node2) const
    {
        return _Node1->F > _Node2->F;
    }
};

class AStarPathFinder : public Singleton<AStarPathFinder>
{
    
    SINGLE(AStarPathFinder);
    
private:

    /* Game field 각 cell 위치에 해당하는 ASNode들 */
    static ASNode* m_FieldNodes[CELL_ROW_COUNT][CELL_ROW_COUNT];
    static UINT m_CurrentEpoch; // 현재 길찾기 회차 ID
    
public:
    /// <summary>
    /// A* 알고리즘을 통한 start부터 dest까지의 경로 찾기
    /// </summary>
    /// <param name="start"> : 출발점 </param>
    /// <param name="dest"> : 도착점 </param>
    /// <param name="_OutPath"> : 구한 경로 push(구하지 못했다면 empty) </param>
    /// <returns> : 해당 경로가 존재하지 않으면 return false </returns>
    static bool GetPath(const CellCoord& start, const CellCoord& dest, stack<CellCoord>& _OutPath); // TODO : Tick에 걸쳐서 계속 호출처리되면 부하가 일어남    
    
public:
    
    static void Init();

    /// <summary>
    /// AStar 알고리즘 내에서 사용할 거리 측정 함수 
    /// </summary>
    static UINT GetDiagonalDist(const CellCoord& _Coord1, const CellCoord& _Coord2);
    
};
