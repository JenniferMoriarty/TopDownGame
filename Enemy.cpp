#include "Enemy.h"


//constructor
Enemy::Enemy()
{
}

//destructor
Enemy::~Enemy()
{
}

int Enemy::getX() { return xPos; }
int Enemy::getY() { return yPos; }

void  Enemy :: initEnemy(int x, int y, ALLEGRO_BITMAP *pic) {
	xPos = x;
	yPos = y;
	image = pic;
	dead = false;
	dir = 1;

}

void Enemy::Move() {
	int num = rand() % 4 + 1;
	//right
	if (num == 1) {
		dir = 1; 
		xPos += 32;
	}
	//left
	if (num == 2) {
		dir = 2;
		xPos -= 32;
	}
	//up
	if (num == 3) {
		dir = 3;
		yPos -= 32;
	}
	//down
	if (num == 4) {
		dir = 4;
		yPos += 32;
	}

}
void Enemy::Draw(int cx, int cy) {

	al_draw_bitmap(image, xPos-cx, yPos-cy, 0);

}

bool  Enemy::isDead() {
	return dead;
}
void  Enemy::kill() {
	dead = true;
}