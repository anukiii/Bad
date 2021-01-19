#include "GameMap.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>
#include "CollisionVolume.h"
#include "EmptyColision.h"
#include <vector>


using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

Vector4 buildingTypes[4] = {
	Vector4(320,16,64,80),	//tree
	Vector4(16,16,96,80), //blue building
	Vector4(128,32,64,64), //red building
	Vector4(208,32,96,64)  //green building
};

int GameMap::CalcH(Vector2 temp, Vector2 final) const {
	return abs(temp.x - final.x) + abs(temp.y - final.y);
}

//Missing something... Code is there but ran out of time
bool GameMap::GeneratePath(Vector2 from, Vector2 to, std::vector < Vector2 >& path)const {

	//tileIndex = (y * mapWidth) + x; secret Formula
	Vector2 fromTile = WorldToTile(from);
	Vector2 toTile = WorldToTile(to); 
	std::vector<MapNode*> openList;
	int tileIndex = (fromTile.y * mapWidth) + fromTile.x;
	MapNode n = nodeData[tileIndex];
	openList.push_back(&n);
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			nodeData[tileIndex].h = CalcH(Vector2{ (float)nodeData[tileIndex].posX,(float)nodeData[tileIndex].posX }, toTile);
		}
	}

	std::vector<MapNode*> activeNeighbours; //where to expand to
	float bestF = openList[0]->g + openList[0]->h;
	MapNode* bestNode = openList[0];
	while (bestNode->posX == toTile.x && bestNode->posY == toTile.y) {
		

		for (auto& i : openList) {
			float iF = i->g + i->h;
			if (iF < bestF) {
				bestF = iF;
				bestNode = i;
			}
		}
		if (bestNode->posX == toTile.x && bestNode->posY) {
			//Create list
			return true;
		}

		if (bestNode->posX < mapWidth - 2) { // right
			int tileType = mapCosts[((bestNode->posY) * mapWidth) + (bestNode->posX - 1)];
			if (tileType != 9 /*my impenetrables*/) {
				MapNode* neighbours = &nodeData[((bestNode->posY) * mapWidth) + (bestNode->posX - 1)];
				activeNeighbours.emplace_back(neighbours);
			}
		}
		if (bestNode->posX < mapWidth - 2) {//left
			int tileType = mapCosts[((bestNode->posY) * mapWidth) + (bestNode->posX + 1)];
			if (tileType != 9 /*my impenetrables*/) {
				MapNode* neighbours = &nodeData[((bestNode->posY) * mapWidth) + (bestNode->posX + 1)];
				activeNeighbours.emplace_back(neighbours);
			}

		}
		if (bestNode->posY < mapHeight - 2) {//up
			int tileType = mapCosts[((bestNode->posY-1) * mapWidth) + (bestNode->posX)];
			if (tileType != 9 /*my impenetrables*/) {
				MapNode* neighbours = &nodeData[((bestNode->posY-1) * mapWidth) + (bestNode->posX)];
				activeNeighbours.emplace_back(neighbours);
			}

		}
		if (bestNode->posY < mapHeight - 2) {//down
			int tileType = mapCosts[((bestNode->posY + 1) * mapWidth) + (bestNode->posX)];
			if (tileType != 9 /*my impenetrables*/) {
				MapNode* neighbours = &nodeData[((bestNode->posY + 1) * mapWidth) + (bestNode->posX)];
				activeNeighbours.emplace_back(neighbours);
			}
		}
	}
return false;
}


Vector2 GameMap::WorldToTile(Vector2 world)const {
	world.x = round(world.x);
	world.y = round(world.y);
	
	return world / 16;
}

Vector2 GameMap::TileToWorld(Vector2 pos)const {
	return pos*16;
}


GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, BadFoodGame* bfg){
	
	tileTexture = texManager.GetTexture("badfoodTiles.png");
	this->bfg = bfg;

	std::ifstream mapFile(Assets::DATADIR + filename);

	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	mapData		= new char[mapWidth * mapHeight];
	mapCosts	= new int[mapWidth * mapHeight];
	nodeData	= new MapNode[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			mapFile >> mapData[tileIndex];


		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';

			nodeData[tileIndex].posX = x;
			nodeData[tileIndex].posY = y;
			nodeData[tileIndex].closed = false;
			nodeData[tileIndex].g = c - '0';
		}
	}
	

	BuildMapMesh();
	std::cout << mapCosts[0];
	mapFile >> structureCount;
	
	structureData = new StructureData[structureCount];
	//std::cout << structureCount << '\n';
	for (int i = 0; i < structureCount; ++i) {
		//std::cout << structureCount << '\n';
		int type  = 0;
		int xTile = 0;
		int yTile = 0;

		mapFile >> type;
		mapFile >> xTile;
		mapFile >> yTile;

		structureData[i].structureType = (MapStructureType)type;
		structureData[i].startPos.x = xTile * 16.0f;
		structureData[i].startPos.y = yTile * 16.0f;

	}



	EmptyColision* ec0 = new EmptyColision(32,8);// tree top
	EmptyColision* ec00 = new EmptyColision({ 25,17 },9);//tree bottom
	EmptyColision* ec000 = new EmptyColision({ 5,5 },7);//tree root
	EmptyColision* ec1 = new EmptyColision({ 31,31 },10);// top left house
	EmptyColision* ec2 = new EmptyColision({ 47,40 },11);// top right house
	EmptyColision* ec3 = new EmptyColision({ 47,31 },12);//bottom right house
	EmptyColision* door1 = new EmptyColision({ 8,8 }, 20); // food trade in 
	EmptyColision* door2 = new EmptyColision({ 8,8 }, 21);


	ec0->SetPosition(Vector2{ (float)structureData[0].startPos.x,(float)structureData[0].startPos.y - 15.0f });
	ec00->SetPosition(Vector2{ (float)structureData[0].startPos.x,(float)structureData[0].startPos.y +12.0f });
	ec000->SetPosition(Vector2{ (float)structureData[0].startPos.x,(float)structureData[0].startPos.y +33.0f });
	ec1->SetPosition(Vector2{ (float)structureData[1].startPos.x,(float)structureData[1].startPos.y });
	ec2->SetPosition(Vector2{ (float)structureData[2].startPos.x,(float)structureData[2].startPos.y });
	ec3->SetPosition(Vector2{ (float)structureData[3].startPos.x,(float)structureData[3].startPos.y });
	door1->SetPosition(Vector2{ (float)structureData[3].startPos.x,(float)structureData[3].startPos.y+30 });
	door2->SetPosition(Vector2{ (float)structureData[2].startPos.x,(float)structureData[2].startPos.y + 40 });
	bfg->AddNewObject(ec0);
	bfg->AddNewObject(ec00);
	bfg->AddNewObject(ec000);
	bfg->AddNewObject(ec1);
	bfg->AddNewObject(ec2);
	bfg->AddNewObject(ec3);
	bfg->AddNewObject(door1);
	bfg->AddNewObject(door2);
}

GameMap::~GameMap()	{
	delete[] mapData;
	delete[] mapCosts;
	delete[] mapTexCoords;
	delete mapMesh;
}

void GameMap::DrawMap(GameSimsRenderer & r) {
	
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
	//structures then go on top
	for (int i = 0; i < structureCount; ++i) {
		Vector4 buildingData = buildingTypes[structureData[i].structureType];

		Vector2 texPos  = Vector2(buildingData.x, buildingData.y);
		Vector2 texSize = Vector2(buildingData.z, buildingData.w);
		Vector2 screenPos = structureData[i].startPos;

		r.DrawTextureArea((OGLTexture*)tileTexture, texPos, texSize, screenPos, false);
	}
}

void GameMap::BuildMapMesh() {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 flatGrassTile = Vector2(288, 144);
	Vector2 primaryTile;
	Vector2 secondaryTile = flatGrassTile; //some sit on top of another tile type, usually grass!
	//EmptyColision *ec = new EmptyColision();
	// ec->SetPosition(Vector2{ (float)x,(float)y })


	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];
			bool doSecondary = false;
			bool doThird = false;
			bool doFourth = false;
			
			switch (tileType) {
				//the normal tile types first
			case 'A':primaryTile = Vector2(272, 112); break; //top left grass
				case 'B':primaryTile = Vector2(288, 112); break;//top grass
				case 'C':primaryTile = Vector2(320, 112); break;//top right grass
				case 'D':primaryTile = Vector2(272, 128); break;//left grass
				case 'E':primaryTile = Vector2(320, 128); break;//right grass
				case 'F':primaryTile = Vector2(272, 160); break;//bottom left
				case 'G':primaryTile = Vector2(320, 160); break;//bottom right
				case 'H':primaryTile = Vector2(288, 160); break;//bottom grass

				case 'I':primaryTile = Vector2(336, 112); break; //top left grass
				case 'J':primaryTile = Vector2(352, 112); break; //top right grass
				case 'K':primaryTile = Vector2(336, 128); break; //bottom left grass
				case 'L':primaryTile = Vector2(352, 128); break; //bottom right grass

				case 'M':primaryTile = Vector2(208, 112); doSecondary = true; doThird = true;  break; //top left fence
				case 'N':primaryTile = Vector2(224, 112); doSecondary = true; doThird = true; break; //top fence
				case 'O':primaryTile = Vector2(240, 112); doSecondary = true; doThird = true; break; //top right fence
				case 'P':primaryTile = Vector2(208, 128); doSecondary = true; doThird = true; break; //left fence
				case 'Q':primaryTile = Vector2(208, 144); doSecondary = true; doThird = true; break; //bottom left fence
				case 'R':primaryTile = Vector2(240, 144); doSecondary = true; doThird = true; break; //bottom right fence
//Stone wall bits
				case 'a':primaryTile = Vector2(16, 112); doSecondary = true; doThird = true; break; //top left grass
				case 'b':primaryTile = Vector2(32, 112); doThird = true; break;//Horizontal A
				case 'c':primaryTile = Vector2(32, 128); doThird = true; break;//Horizontal B
				case 'd':primaryTile = Vector2(144, 112); doThird = true; doSecondary = true; break;//top right grass
				case 'e':primaryTile = Vector2(144, 128); doThird = true; break;//Side
				case 'f':primaryTile = Vector2(16, 160); doThird = true; break;//bottom left A
				case 'g':primaryTile = Vector2(16, 176); doSecondary = true; doThird = true; break;//bottom left B
	
				case 'h':primaryTile = Vector2(64, 160); doSecondary = true; doThird = true; break;//right hand corner A
				case 'i':primaryTile = Vector2(64, 176); doSecondary = true; doThird = true; break;//right hand corner B

				case 'j':primaryTile = Vector2(96, 160); doSecondary = true; doThird = true; break;//Left hand corner A
				case 'k':primaryTile = Vector2(96, 176); doSecondary = true; doThird = true; break;//Left hand corner B

				case 'l':primaryTile = Vector2(144, 160); doThird = true; break;//Bottom right A
				case 'm':primaryTile = Vector2(144, 176); doSecondary = true; doThird = true;  break;//Bottom right B
				case 'n':primaryTile = Vector2(32, 176); doSecondary = true; doThird = true; break;//bottom horizontal

				case 'o':primaryTile = Vector2(80, 176); doSecondary = true; break;//Stone to grass transition
				case 'p':primaryTile = Vector2(80, 176); doSecondary = true; break;//stone to road transition

				case '1':primaryTile = Vector2(352, 144); break; //Road Tile
				case '2':primaryTile = Vector2(64, 144);  break;//Stone Tile
				case '3':primaryTile = Vector2(16, 192); doSecondary  = true; break; //Flower A
				case '4':primaryTile = Vector2(32, 192); doSecondary  = true; break;//Flower B
				case '5':primaryTile = Vector2(48, 192); doSecondary  = true; break;//Flower C
				case '6':primaryTile = Vector2(64, 192); doSecondary  = true; break;//Flower D
				case '7':primaryTile = Vector2(176, 176); doSecondary = true; doFourth = true; break;//Sign A
				case '8':primaryTile = Vector2(192, 176); doSecondary = true; doFourth = true; break;//Sign B
				case '9':primaryTile = Vector2(208, 176); doSecondary = true; doFourth = true; break;//Sign C	
				case '0': {
					static Vector2 grassTiles[] = {
						Vector2(288,128),	//	
						Vector2(304,144),	//
						Vector2(304,128),	//
						Vector2(288,144),
					};
					primaryTile = grassTiles[Maths::Clamp(rand() % 20, 0, 3)];
				}break;//Random grass tiles
			}			
			if (doSecondary) {
				AddNewTilePos(x, y, positions);
				AddNewTileTex((int)secondaryTile.x, (int)secondaryTile.y, texCoords);
				secondaryTile = flatGrassTile;
			}
			if (doThird) {
				EmptyColision* ec = new EmptyColision({ 6.5,5.5 },7);
				ec->SetPosition(Vector2{ (float)x * 16 +7.5f,(float)y * 16 +5.0f });
				bfg->AddNewObject(ec);
				//std::cout << mapWidth << '\n';
			}			
			else if (doFourth) {
				EmptyColision* ec = new EmptyColision({ 7,5 },13);
				ec->SetPosition(Vector2{ (float)x * 16 +7.5f,(float)y * 16 +5.0f });
				bfg->AddNewObject(ec);
				//std::cout << mapWidth << '\n';
			}
			AddNewTilePos(x, y, positions);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
	}
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();
}

void GameMap::AddNewTilePos(int x, int y, std::vector<Vector3>& pos) {
	Vector3 topLeft		= Vector3((x + 0) * 16.0f, (y + 0) * 16.0f, 0);
	Vector3 topRight	= Vector3((x + 1) * 16.0f, (y + 0) * 16.0f, 0);
	Vector3 bottomLeft	= Vector3((x + 0) * 16.0f, (y + 1) * 16.0f, 0);
	Vector3 bottomRight = Vector3((x + 1) * 16.0f, (y + 1) * 16.0f, 0);

	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex) {
	Vector2 topLeft		= Vector2((x + 0.0f) , (y + 0.0f));
	Vector2 topRight	= Vector2((x + 16.0f), (y + 0.0f));
	Vector2 bottomLeft	= Vector2((x + 0.0f) , (y + 16.0f));
	Vector2 bottomRight = Vector2((x + 16.0f), (y + 16.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft); 
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}