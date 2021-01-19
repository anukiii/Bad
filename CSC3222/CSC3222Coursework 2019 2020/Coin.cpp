#include "Coin.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

Vector2 animFrames[] = {
	Vector2(96 , 80),
	Vector2(112, 80),
	Vector2(128, 80),
	Vector2(144, 80),
};

Coin::Coin() : SimObject() {
	animFrameCount = 4;
	texture = texManager->GetTexture("Items.png");
	identity = 3;
	collider = new CollisionVolume(&position, 7);
	remainingBounce = 1;
}

Coin::~Coin() {

}

bool Coin::UpdateObject(float dt) {
	UpdateAnimFrame(dt);
	if (remainingBounce <= 0) {
		return false;
	}
	return true;
}

void Coin::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	 = animFrames[currentanimFrame];
	Vector2 texSize	 = Vector2(16,16);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}