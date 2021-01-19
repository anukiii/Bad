#pragma once
#include <string>
#include <vector>

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"
#include "BadFoodGame.h"

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsRenderer;
		class SimObject;
		class TextureManager;

		enum MapStructureType {
			Tree,
			BlueBuilding,
			RedBuilding,
			GreenBuilding
		};

		struct StructureData {
			Vector2				startPos;
			MapStructureType	structureType;
		};

		struct MapNode {
			MapNode* bestParent;
			int posX;
			int posY;
			float g;
			float h;
			bool closed;
		};
		

		
		class GameMap	{
		public:
			GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, BadFoodGame *bfg);
			~GameMap();

			void DrawMap(GameSimsRenderer & r);
			
			int CalcH(Vector2 temp, Vector2 final) const;
			Vector2 GetRandomPosition() const;
			Vector2 GetRandomPositionWithinRadius(Vector2 pos,float r) const;
			Vector2 WorldToTile(Vector2 world) const;
			Vector2 TileToWorld(Vector2 tile) const;
			bool GeneratePath(Vector2 from, Vector2 to, std::vector < Vector2 >& path)const;

			int GetMapWidth() const {
				return mapWidth;
			}

			int GetMapHeight() const {
				return mapHeight;
			}

			


		protected:
			void BuildMapMesh();
			void AddNewTilePos(int x, int y, std::vector<Vector3>& pos);
			void AddNewTileTex(int x, int y, std::vector<Vector2>& tex);

			int mapWidth;
			int mapHeight;
			int structureCount;

			Rendering::TextureBase*	tileTexture;
			char*	mapData;
			int*			mapCosts;
			Vector2*		mapTexCoords;
			StructureData*	structureData;		

			MapNode* nodeData;
			MeshGeometry* mapMesh;
			BadFoodGame* bfg;



		};
	}
}

