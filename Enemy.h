#pragma once
#include<allegro5\allegro.h>
#include<allegro5\allegro_image.h>
class Enemy
{
private:
	int xPos;
	int yPos;
	int dir;
	bool dead;
	char type; //to be used later
	ALLEGRO_BITMAP *image;


public:
	Enemy();
	~Enemy();
	void initEnemy(int x, int y, ALLEGRO_BITMAP *pic);
	void Move();
	void Draw(int cx, int cy);
	bool isDead();
	void kill();
	int getX();
	int getY();

};

