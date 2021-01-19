#include "Balloon.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

Balloon::Balloon() : SimObject() {
	texture = texManager->GetTexture("TL_Creatures.png");
	identity = 5;
	inverseMass = 10;
	collider = new CollisionVolume(&position, 0);
}

Balloon::~Balloon() {

}

bool Balloon::UpdateObject(float dt) {
	Lifetime -= dt;
	if (Lifetime <= 0) {
		return false;
	}
	return true;
}

void Balloon::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	= Vector2(73, 99);
	Vector2 texSize = Vector2(14, 28);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}