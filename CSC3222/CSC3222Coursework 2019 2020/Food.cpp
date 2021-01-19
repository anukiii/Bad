#include "Food.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"



using namespace NCL;
using namespace CSC3222;

Vector4 foodFrames[] = {
	Vector4(0,0,15,16), //Apple
	Vector4(17,0,14,16),//Can
	Vector4(31,0,18,16),//Pizza
	Vector4(49,0,15,16),//Sundae
	Vector4(64,0,16,16) //Bread
};

Food::Food() : SimObject() {
	texture = texManager->GetTexture("food_items16x16.png");
	identity = 4;
	foodItem = rand() % 5;
	collider = new CollisionVolume(&position, 7);
	currentanimFrame = foodItem;
	remainingBounce = 1;

}

Food::~Food() {

}

bool Food::UpdateObject(float dt) {
	Lifetime -= dt;
	if (remainingBounce <= 0 || Lifetime <=0) {
		return false;
	}

	return true;
}

void Food::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	= Vector2(foodFrames[currentanimFrame].x, foodFrames[currentanimFrame].y);
	Vector2 texSize = Vector2(foodFrames[currentanimFrame].z, foodFrames[currentanimFrame].w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}