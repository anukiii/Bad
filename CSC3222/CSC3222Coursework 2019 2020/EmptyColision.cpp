#include "EmptyColision.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"
using namespace NCL;
using namespace CSC3222;


EmptyColision::EmptyColision(Vector2 v, int id):SimObject(){
	identity = id; // for debug
	collider = new CollisionVolume(&position,v.x, v.y);
	inverseMass = 0;
	//std::cout << "SQUARE" << '\n';
}

EmptyColision::EmptyColision(float x, int id) : SimObject() {
	identity = id; // for debug
	collider = new CollisionVolume(&position,x);
	inverseMass = 0;
}

EmptyColision::~EmptyColision() {

}

bool EmptyColision::UpdateObject(float dt) {
	return true;
}

void EmptyColision::DrawObject(GameSimsRenderer& r) {
	//no implementation needed
}
