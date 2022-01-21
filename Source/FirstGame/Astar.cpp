#include "AStar.h"
#include <fstream>
#include <string>
#include <algorithm>

using namespace std::placeholders;

namespace pf
{
	AStar::AStar() : 
	m_weight(1),
	m_size(0),
	m_dimensions(0, 0),
	m_startPos(0, 0),
	m_targetPos(0, 0),
	m_nrOfDirections(4)
	{
		// m_directions = { { -1, 0 }, { 1, 0 }, { 0, 1 }, { 0, -1 },
		// 				 { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 } };
		m_directions = { { 0, -1 }, { 0, 1 }, { 1, 0 }, { -1, 0 },
						 { -1, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 } };
	}

	// 압축 그리드를 사용하고 있기 때문에 예를들어 직선상의 목적지를 찾더라도 직선이 아닌 살짝 압축된 크기만큼 돌아갈 수 있다.
	std::vector<Vec2i> AStar::findPath(const Vec2i& startPos, const Vec2i& targetPos, HeuristicFunction heuristicFunc, int weight)
	{
		m_startPos = startPos;
		m_targetPos = targetPos;

		m_startPos.x /= compressionSize;
		m_startPos.y /= compressionSize;

		m_targetPos.x /= compressionSize;
		m_targetPos.y /= compressionSize;
		
		m_weight = weight;
		m_heuristic = std::bind(heuristicFunc, _1, _2, _3);

		if (m_cameFrom.size() != m_size)
			m_cameFrom.resize(m_size);

		if (m_closedList.size() != m_size)
			m_closedList.resize(m_size);

		for (auto& elem : m_cameFrom)
		{
			elem.Clear();
		}

		for (int i = 0, k = m_closedList.size(); i < k; ++i)
		{
			m_closedList[i] = false;
		}
		
		std::priority_queue<Node> reset;
		std::swap(m_openList, reset);

		m_cameFrom[convertTo1D(m_startPos)].parent = m_startPos;
		m_openList.push(Node(m_startPos, 0));

		uint fNew, gNew, hNew;
		Vec2i currentPos;

		while (!m_openList.empty())
		{
			// Get the node with the least f value
			currentPos = m_openList.top().pos;

			if (currentPos == m_targetPos)
			{
				break;
			}

			m_openList.pop();
			m_closedList[convertTo1D(currentPos)] = true;

			// Check the neighbors of the current node
			for (uint i = 0; i < m_nrOfDirections; ++i)
			{
				const auto neighborPos = currentPos + m_directions[i];
				const auto neighborIndex = convertTo1D(neighborPos);

				// UE_LOG(LogTemp, Warning, TEXT("check pos : %d, %d / index : %d / w : %d / isValid : %d / isBlocked : %d / isCloseList : %d"),
				// 	neighborPos.x, neighborPos.y, neighborIndex, m_dimensions.x,
				// 	(isValid(neighborPos)) ? 1 : 0, (isBlocked(neighborIndex)) ? 1 : 0, (!isBlocked(neighborIndex) && m_closedList[neighborIndex]) ? 1 : 0);

				if (!isValid(neighborPos) || isBlocked(neighborIndex) || m_closedList[neighborIndex] == true)
				{
					continue;
				}

				gNew = m_cameFrom[convertTo1D(currentPos)].g + 1;
				hNew = m_heuristic(neighborPos, m_targetPos, m_weight);
				fNew = gNew + hNew;

				//UE_LOG(LogTemp, Warning, TEXT("g : %d, h : %d, f : %d"), gNew, hNew, fNew);

				if (m_cameFrom[neighborIndex].f == 0 || fNew < m_cameFrom[neighborIndex].f)
				{
					m_openList.push(Node(neighborPos, fNew));
					m_cameFrom[neighborIndex] = { neighborPos, currentPos, fNew, gNew, hNew };
				}
			}
		}

		return buildPath();
	}

	std::vector<Vec2i> AStar::buildPath() const
	{
		std::vector<Vec2i> path;
		auto currentPos = m_targetPos;
		auto currentIndex = convertTo1D(currentPos);
		// UE_LOG(LogTemp, Warning, TEXT("m tar pos %f, %f   idx : %d"), m_targetPos.x, m_targetPos.y, currentIndex);

		size_t checkSize = m_cameFrom.size();
		while (!(m_cameFrom[currentIndex].parent == currentPos))
		{
			path.push_back(currentPos);
			currentPos = m_cameFrom[currentIndex].parent;
			currentIndex = convertTo1D(currentPos);

			if (currentIndex < 0 || currentIndex >= checkSize)
			{
				break;
			}
		}

		std::reverse(path.begin(), path.end());

		for (auto& elem : path)
		{
			elem.x *= compressionSize;
			elem.y *= compressionSize;
		}

		return path;
	}

	void AStar::loadMap()
	{
		FString path = FPaths::ProjectContentDir();
		path.Append(TEXT("CustomFiles/pathGrid.bin"));
		std::ifstream pathGridFile(*path, std::ios::in | std::ios::binary);
		
		if (pathGridFile.is_open())
		{
			size_t exceptSize = sizeof(int32);
			int32 spacing = 0;
        	pathGridFile.read((char*)&spacing, exceptSize);
			compressionSize = spacing;

			pathGridFile.seekg(0, pathGridFile.end);
			size_t fileSize = pathGridFile.tellg();
			fileSize -= exceptSize;
			pathGridFile.seekg(exceptSize);

			size_t arrSize = fileSize / sizeof(bool);
			bool* buf = new bool[arrSize];
			pathGridFile.read((char *) buf, fileSize);

			// 정사각형이고 spacing 만큼 압축된 크기이다, 그리고 x,y 로부터 +spacing 만큼 사각영역을 색칠했다고 보면 된다.
			int compressionMapW = FMath::Sqrt(arrSize);
			m_dimensions.x = compressionMapW;
			m_dimensions.y = compressionMapW;
			m_size = m_dimensions.x * m_dimensions.y;

			// 압축한 그리드를 그냥 사용하고 결과 경로의 x,y 를 spacing 만큼 키워서 사용하기로 함.
			m_grid.clear();
			for (int i = 0; i < arrSize; ++i)
			{
				m_grid.push_back(buf[i] == true ? 1 : 0);
			}

			UE_LOG(LogTemp, Warning, TEXT("load grid spacing : %d, arrSize : %d, mapW : %d, mapSize : %d"), 
					spacing, arrSize, m_dimensions.x, m_size);

			delete[] buf;
        		
			pathGridFile.close();
			
		}
		// std::ifstream file(fileName);
		//
		// if (file.is_open())
		// {
		// 	std::string line;
		// 	while (std::getline(file, line))
		// 	{
		// 		if (line.find('w') != std::string::npos)
		// 		{
		// 			line.erase(std::remove_if(line.begin(), line.end(), 
		// 				[](unsigned char c) { return (c == 'w' || c == ':') ? true : false; }), line.end());
		// 			m_dimensions.x = std::stoi(line);
		// 		}
		// 		else if (line.find('h') != std::string::npos)
		// 		{
		// 			line.erase(std::remove_if(line.begin(), line.end(),
		// 				[](unsigned char c) { return (c == 'h' || c == ':') ? true : false; }), line.end());
		// 			m_dimensions.y = std::stoi(line);
		// 		}
		// 		else
		// 		{
		// 			line.erase(std::remove(line.begin(), line.end(), ','), line.end());
		//
		// 			for (const auto& c : line)
		// 			{
		// 				m_grid.push_back(c - 48);
		// 			}
		// 		}
		// 	}
		//
		// 	m_size = m_dimensions.x * m_dimensions.y;
		// 	file.close();
		// }
	}

	void AStar::setDiagonalMovement(bool enable)
	{
		m_nrOfDirections = (enable) ? 8 : 4;
	}

	bool AStar::isValid(const Vec2i& pos) const
	{
		return (pos.x >= 0) && (pos.x < m_dimensions.x) && 
			   (pos.y >= 0) && (pos.y < m_dimensions.y);
	}

	bool AStar::isValid(const float& x, const float& y) const
	{
		return (x >= 0) && (x < m_dimensions.x) && 
			   (y >= 0) && (y < m_dimensions.y);
	}


	bool AStar::isBlocked(int index) const
	{
		return (m_grid[index] == 0);
	}

	// Returns a 1D index based on a 2D coordinate using row-major layout
	int AStar::convertTo1D(const Vec2i& pos) const
	{
		return (pos.x * m_dimensions.x) + pos.y;
	}

	bool AStar::CanPos(const float& _x, const float& _y)
	{
		int x = _x / compressionSize;
		int y = _y / compressionSize;
		
		if (isValid(x, y) == false)
			return false;
		
		return !isBlocked(convertTo1D(Vec2i(x / compressionSize, y / compressionSize)));
	}


	uint heuristic::manhattan(const Vec2i& v1, const Vec2i& v2, int weight)
	{
		const auto delta = Vec2i::getDelta(v1, v2);
		return static_cast<uint>(weight * (delta.x + delta.y));
	}

	uint heuristic::euclidean(const Vec2i& v1, const Vec2i& v2, int weight)
	{
		const auto delta = Vec2i::getDelta(v1, v2);
		return static_cast<uint>(weight * sqrt((delta.x * delta.x) + (delta.y * delta.y)));
	}
}