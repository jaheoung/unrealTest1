#pragma once

#include <queue>
#include <vector>
#include <functional>
#include <string>
#include "Vec2i.h"

// https://github.com/Rikora/A-star 에서 가져다 수정해서 씀.

namespace pf
{
	using uint = unsigned int;
	using HeuristicFunction = std::function<uint(const Vec2i&, const Vec2i&, int)>;

	struct Node
	{
		Node() : pos(0, 0), parent(-1, -1), f(0), g(0), h(0) {}
		Node(const Vec2i& pos, uint f) : pos(pos), parent(1, -1), f(f), g(0), h(0) {}
		Node(const Vec2i& pos, const Vec2i& parent, uint f, uint g, uint h) : pos(pos), parent(parent), f(f), g(g), h(h) {}

		Vec2i pos;
		Vec2i parent;
		uint f;
		uint g;
		uint h;

		void Clear()
		{
			pos.x = 0;
			pos.y = 0;
			parent.x = 0;
			parent.y = 0;
			f = 0;
			g = 0;
			h = 0;
		}
	};

	// Reverse std::priority_queue to get the smallest element on top
	inline bool operator< (const Node& a, const Node& b) { return a.f < b.f; }

	class AStar
	{
	public:
		AStar();

		std::vector<Vec2i> findPath(const Vec2i& startPos, const Vec2i& targetPos, HeuristicFunction heuristicFunc, int weight = 1);
		void loadMap();
		void setDiagonalMovement(bool enable);

		// 이 크기만큼 압축된 그리드를 사용.
		int compressionSize;
		std::vector<int> m_grid;

		bool CanPos(const float& _x, const float& _y);

	private:
		std::vector<Vec2i> buildPath() const;
		bool isValid(const Vec2i& pos) const;
		bool isValid(const float& x, const float& y) const;
		bool isBlocked(int index) const;
		int convertTo1D(const Vec2i& pos) const;

		int m_weight;
		int m_size;
		
		
		Vec2i m_dimensions;
		Vec2i m_startPos;
		Vec2i m_targetPos;
		std::priority_queue<Node> m_openList;
		std::vector<bool> m_closedList;
		std::vector<Node> m_cameFrom;
		
		uint m_nrOfDirections;
		std::vector<Vec2i> m_directions;
		HeuristicFunction m_heuristic;
	};

	namespace heuristic
	{
		uint manhattan(const Vec2i& v1, const Vec2i& v2, int weight);
		uint euclidean(const Vec2i& v1, const Vec2i& v2, int weight);
	}
}
