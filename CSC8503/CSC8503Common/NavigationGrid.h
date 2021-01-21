#pragma once
#include "NavigationMap.h"
#include <string>
#include<set>
namespace NCL {
	namespace CSC8503 {
		struct GridNode {
			GridNode* parent;

			GridNode* connected[4];
			int		  costs[4];

			Vector3		position;

			float f;
			float g;

			int type;

			GridNode() {
				for (int i = 0; i < 4; ++i) {
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = 0;
				parent = nullptr;
			}
			~GridNode() {	}

			bool operator < (GridNode& comp) const { return this->f < comp.f; }
		};

		class NavigationGrid : public NavigationMap	{
		public:
			NavigationGrid();
			NavigationGrid(const std::string&filename);
			~NavigationGrid();

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;

			int GetGridWidth() const { return gridWidth; }
			int GetGridHeight() const { return gridHeight; }
			int GetNodeSize() const { return nodeSize; }
				
		protected:
			bool		NodeInList(GridNode* n, std::vector<GridNode*>& list) const;
			bool		NodeInList(GridNode* n, std::set<GridNode*>& list) const;
			GridNode*	RemoveBestNode(std::vector<GridNode*>& list) const;
			float		Heuristic(GridNode* hNode, GridNode* endNode) const;
			int nodeSize;
			int gridWidth;
			int gridHeight;

			GridNode* allNodes;
		};
	}
}

