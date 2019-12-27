#include <stdio.h>
#include<iostream>
#include <allegro5/allegro.h>
#include<allegro5\allegro_image.h>
#include<allegro5\allegro_primitives.h>
#include<allegro5\allegro_font.h>
#include<allegro5\allegro_ttf.h>
#include<ctime>
#include"Enemy.h"
using namespace std;


int MapCollision(int direction, int X, int Y, int map[32][32]);
int InsideCollision(int direction, int X, int Y, int map[10][10]);
bool boxCollision(int x1, int y1, int x2, int y2);

//global maps
int map[32][32];
int inside[10][10];

const float FPS = 60;
const int player_SIZE = 32;
enum MYKEYS { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE };

enum directions { RIGHT, LEFT, UP, DOWN };

enum STATES { INTRO, FOREST, INTERIOR, BATTLE, END };

enum BATTLEOPS {PLAYER, FIGHT, RUN};

int main()
{
	srand(time(NULL));
	int state = INTRO;
	int BattleState = PLAYER;

	//SET UP INTERIOR MAP

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++)
			//draw "ring" of rocks around map so player sees he can't walk off
			if (i == 0 || j == 0 || i == 9 || j == 9)
				inside[i][j] = 100;
			else
				inside[i][j] = 0;
	}
	//set up door
	inside[5][9] = 3;
	inside[4][9] = 3;


	//SET UP FOREST MAP
	//each column/row will be a 32x32 box. 

	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++)
			//draw "ring" of rocks around map so player sees he can't walk off
			if (i == 0 || j == 0 || i == 31 || j == 31)
				map[i][j] = 100;
			else
				map[i][j] = 0;
	}

	//add random trees and rocks (with collision!)
	//trees
	for (int i = 0; i < 32; i++)
		map[rand() % 30 + 1][rand() % 30 + 1] = 100;
	//rocks
	for (int i = 0; i < 6; i++)
		map[rand() % 30 + 1][rand() % 30 + 1] = 103;
	//flowers
	for (int i = 0; i < 32; i++)
		map[rand() % 30 + 1][rand() % 30 + 1] = 2;


	//clear trees from house area
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			map[16 + i][16 + j] = 0;

	//set house collision
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			map[16 + i][16 + j] = 102; //no image, just collision

	//set up door
	map[18][20] = 3;
	map[18][19] = 3;


	//////////////////////////////////////////////////////////////////


	//player sprite info
	const int maxFrame = 8;
	int curFrame = 0;
	int frameCount = 0;
	int frameDelay = 5;
	int frameWidth = 32;
	int frameHeight = 48;
	int dir = RIGHT;


	//print to console for testing (looks weird because of big numbers)
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++)
			cout << map[i][j];
		cout << endl;
	}

	//variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *player = NULL;
	ALLEGRO_BITMAP *tree = NULL;
	ALLEGRO_BITMAP *flower = NULL;
	ALLEGRO_BITMAP *house = NULL;
	ALLEGRO_BITMAP *rock = NULL;
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_FONT *font2 = NULL;
	ALLEGRO_BITMAP *wall = NULL;
	ALLEGRO_BITMAP *enemyPic = NULL;
	ALLEGRO_BITMAP *BattleEnemy = NULL;
	ALLEGRO_BITMAP *BattlePlayer = NULL;
	ALLEGRO_BITMAP *slash = NULL;

	int counter = 0;//for monster movement delay
	bool sword = false; //player sword out or not
	int player_x = 300;
	int player_y = 300;

	//clear player's area of trees and stuff
	map[player_x / 32][player_y / 32] = 0;
	map[(player_x+32) / 32][player_y / 32] = 0;
	map[(player_x-32) / 32][player_y / 32] = 0;
	map[player_x / 32][(player_y+32) / 32] = 0;
	map[player_x / 32][(player_y-32) / 32] = 0;


	bool key[5] = { false, false, false, false, false };
	bool redraw = true;
	bool doexit = false;
	int life = 100;
	int enermyHealth = 100; //for RPG enemy
	bool enemyTurn = false; //for RPG enemy


	//CAMERA STUFF
	// the screen size (the actual resolution of our display or window)
	int screenWidth = 640;
	int screenHeight = 640;

	// the world size
	int worldWidth = 1024;
	int worldHeight = 1024;

	// the camera's position
	int cameraX = 0;
	int cameraY = 0;

	//set up Allegro
	al_init();
	al_init_image_addon();
	al_install_keyboard();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	timer = al_create_timer(1.0 / FPS);
	display = al_create_display(screenHeight, screenWidth);

	//load game assets
	tree = al_load_bitmap("tree.png");
	al_convert_mask_to_alpha(tree, al_map_rgb(255, 255, 255));
	flower = al_load_bitmap("flower1.png");
	house = al_load_bitmap("house.png");
	al_convert_mask_to_alpha(house, al_map_rgb(255, 0, 255));
	rock = al_load_bitmap("rock.png");
	al_convert_mask_to_alpha(rock, al_map_rgb(255, 255, 255));


	player = al_load_bitmap("link.png");
	al_convert_mask_to_alpha(player, al_map_rgb(255, 0, 255));
	
	BattleEnemy= al_load_bitmap("ogre.png");
	BattlePlayer = al_load_bitmap("ranger.png");
	al_convert_mask_to_alpha(BattlePlayer, al_map_rgb(255, 255, 255));

	enemyPic = al_load_bitmap("monster.png");
	al_convert_mask_to_alpha(enemyPic, al_map_rgb(255, 0, 255));

	wall = al_load_bitmap("wall.png");
	slash = al_load_bitmap("slash.png");


	font = al_load_font("Triforce.ttf", 80, 0);
	font2 = al_load_font("final.ttf", 30, 0);

	al_set_target_bitmap(al_get_backbuffer(display));

	//create event queue and set it up to look for close window buttons, timer ticks, and keyboard input
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());


	//start the game
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(timer);

	//instantiate enemy
	Enemy monster1;
	monster1.initEnemy(200, 500, enemyPic);

	//game loop
	while (!doexit)
	{
		
		if (state == INTRO) {
			al_draw_text(font, al_map_rgb(200, 200, 50), 50, 50, 0, "The Legend of");
			al_draw_text(font, al_map_rgb(200, 200, 50), 50, 250, 0, "Final ");
			al_draw_text(font, al_map_rgb(200, 200, 50), 100, 350, 0, "Pokemon");
			al_flip_display();
			al_rest(1);
			state = FOREST;
		}

		//cout << "player at " << player_x / 32 << " , " << player_y / 32 << endl;
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {

			//move to interior state if you go in the door
			if (state == FOREST && map[player_x / 32][player_y / 32] == 3) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				al_flip_display();
				al_rest(1);
				state = INTERIOR;
				player_x = 150;
				player_y = 275;
			}

			//move to exterior if you go out the door
			if (state == INTERIOR && inside[player_x / 32][(player_y + 6) / 32] == 3) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				al_flip_display();
				al_rest(1);
				state = FOREST;
				player_x = 576;
				player_y = 690;
			}

			//sword handling
			if (key[KEY_SPACE]) {
				//play *shink* sound here
				sword = true;
				if (dir == RIGHT && !monster1.isDead() && boxCollision(screenWidth / 2+32+16, screenHeight / 2+16, monster1.getX()-cameraX, monster1.getY()-cameraY)) {
					monster1.kill();
					cout << "monster killed!" << endl;
				}
				if (dir == LEFT && !monster1.isDead() && boxCollision(screenWidth / 2 + - 16, screenHeight / 2 + 16, monster1.getX() - cameraX, monster1.getY() - cameraY)) {
					monster1.kill();
					cout << "monster killed!" << endl;
				}
				if (dir == UP && !monster1.isDead() && boxCollision(screenWidth / 2 + 16, screenHeight / 2 + 16, monster1.getX() - cameraX, monster1.getY() - cameraY)) {
					monster1.kill();
					cout << "monster killed!" << endl;
				}
				if (dir == DOWN && !monster1.isDead() && boxCollision(screenWidth / 2  + 16, screenHeight / 2  +32 +16, monster1.getX() - cameraX, monster1.getY() - cameraY)) {
					monster1.kill();
					cout << "monster killed!" << endl;
				}
			}
			else sword = false;

			//cout << "key up is" << key[UP] << endl;
			if ((key[KEY_UP] && state == INTERIOR && !InsideCollision(UP, player_x, player_y, inside)) || (state == FOREST && key[KEY_UP] && !MapCollision(UP, player_x, player_y, map))) {
				dir = UP;
				player_y -= 4.0;
				if (++frameCount >= frameDelay)
				{
					if (++curFrame >= maxFrame)
						curFrame = 0;

					frameCount = 0;
				}
			}


			if ((key[KEY_DOWN] && state == INTERIOR && !InsideCollision(DOWN, player_x, player_y, inside)) || (state == FOREST && key[KEY_DOWN] && !MapCollision(DOWN, player_x, player_y, map))) {
				dir = DOWN;
				player_y += 4.0;
				if (++frameCount >= frameDelay)
				{
					if (++curFrame >= maxFrame)
						curFrame = 0;

					frameCount = 0;
				}
			}

			if ((key[KEY_LEFT] && state == INTERIOR && !InsideCollision(LEFT, player_x, player_y, inside)) || (state == FOREST && key[KEY_LEFT] && !MapCollision(LEFT, player_x, player_y, map))) {
				dir = LEFT;
				player_x -= 4.0;
				if (++frameCount >= frameDelay)
				{
					if (++curFrame >= maxFrame)
						curFrame = 0;

					frameCount = 0;
				}
			}

			if ((key[KEY_RIGHT] && state == INTERIOR && !InsideCollision(RIGHT, player_x, player_y, inside)) || (state == FOREST && key[KEY_RIGHT] && !MapCollision(RIGHT, player_x, player_y, map))) {
				dir = RIGHT;
				player_x += 4.0;

				//sprite stuff
				if (++frameCount >= frameDelay)
				{
					if (++curFrame >= maxFrame)
						curFrame = 0;

					frameCount = 0;
				}
			}

			frameCount++;


			//keep camera on screen
			if (cameraX < 0)
				cameraX = 0;

			if (cameraY < 0)
				cameraY = 0;

			if (cameraX > worldWidth - screenWidth)
				cameraX = worldWidth - screenWidth;

			if (cameraY > worldHeight - screenHeight)
				cameraY = worldHeight - screenHeight;


			// make the camera follow the player
			cameraX = player_x - screenWidth / 2;
			cameraY = player_y - screenHeight / 2;

			//check player position for testing
			//cout << "player is at " << player_x << " , " << player_y <<" , matrix slot "<<player_x/32<<" , "<<player_y/32<< endl;

			//move enemies
			if (state == FOREST) {
				counter++;
				if (counter > 90) {
					counter = 0;
					monster1.Move();
				}
				//check for collision with enemy
				//cout << "monster at " << monster1.getX() << " , " << monster1.getY() << endl;
				if (!monster1.isDead() &&boxCollision(screenWidth / 2, screenHeight / 2, monster1.getX()-cameraX, monster1.getY()-cameraY)) {
					//health--;
					//play "ouch" sound effect
					if (dir == RIGHT)
						player_x -= 30;
					if (dir == LEFT)
						player_x += 30;
					if (dir == UP)
						player_y -= 30;
					if (dir == DOWN)
						player_y += 30;
					life-=10;
				}

			}
			if (life <= 0)
				doexit = true;
			

			//if inside, generate possibility of random encounter with enemy, RPG-style
			if (state == INTERIOR && (KEY_DOWN || KEY_LEFT || KEY_RIGHT || KEY_UP)) {
				enermyHealth = 100;
				if (rand() % 200 == 0) {
					state = BATTLE;
					counter = 50;
				}

			}


			if (state == BATTLE) {
				cout << "enemyturn is " << enemyTurn << endl;
				if (enemyTurn) {
					counter--;
					cout << "ogre attacks" << counter<<endl;
					if (counter < 0) {
						enemyTurn = false;
						counter = 50;
						life -= 5;
					}
				}
				if (BattleState == PLAYER && key[KEY_DOWN]) {
					BattleState = FIGHT;
					key[KEY_DOWN] = false;
				}

				if (BattleState == FIGHT && key[KEY_DOWN])
					BattleState = RUN;

				if (BattleState == RUN && key[KEY_UP])
					BattleState = FIGHT;

				if (BattleState == FIGHT && key[KEY_SPACE]) {
					enermyHealth -= 10;
					//play sound effect here
					al_draw_bitmap(slash, 100, 100, 0);
					enemyTurn = true;
					al_flip_display();
					al_rest(1);
					key[KEY_SPACE] = false;
				}

				if (BattleState == RUN && key[KEY_SPACE]) {
					state = INTERIOR;
				}


			}

			if (state == BATTLE && enermyHealth == 0)
				state = INTERIOR;

			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = true;
				break;

			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = true;
				break;

			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = true;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = true;
				break;

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = true;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = false;
				break;

			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = false;
				break;

			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = false;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = false;
				break;

			case ALLEGRO_KEY_ESCAPE:
				doexit = true;
				break;

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = false;
				break;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;



			if (state == INTERIOR) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				for (int i = 0; i < 10; i++) {
					for (int j = 0; j < 10; j++) {
						if (inside[i][j] == 0) {
							al_draw_filled_rectangle(i * 32 - cameraX, j * 32 - cameraY, i * 32 - cameraX + 32, j * 32 - cameraY + 32, al_map_rgb(200, 200, 50));
						}
						if (inside[i][j] == 100) {
							al_draw_bitmap(wall, i * 32 - cameraX, j * 32 - cameraY, 0);
						}
						if (map[i][j] == 3) {
							al_draw_filled_rectangle(i * 32 - cameraX, j * 32 - cameraY, i * 32 - cameraX + 32, j * 32 - cameraY + 32, al_map_rgb(0, 0, 0));
						}
					}
				}
				

			}


			if (state == BATTLE) {
				al_clear_to_color(al_map_rgb(0, 0, 0));


				if (enemyTurn) {
					cout << "drawing ogre words";
					al_draw_text(font2, al_map_rgb(200, 200, 200), 50, 600, 0, "OGRE Attacks!");
					system("pause");
				
				}

				//cout << "BattleState is " << BattleState << endl;

				al_draw_bitmap(BattleEnemy, 80, 80, 0);

				al_draw_bitmap(BattlePlayer, 450, 50, 0);
				
				al_draw_textf(font2, al_map_rgb(200, 200, 200), 420, 150, 0, "%d", life);
				al_draw_text(font2, al_map_rgb(200, 200, 200), 420, 180, 0, "----");
				al_draw_text(font2, al_map_rgb(200, 200, 200), 420, 210, 0, "100");

				al_draw_rectangle(0, 0, 400, 400, al_map_rgb(250, 250, 250), 10);//top left
				al_draw_rectangle(0, 400, 400, 640, al_map_rgb(250, 250, 250), 10);//bottom left
				al_draw_rectangle(400, 0, 640, 400, al_map_rgb(250, 250, 250), 10);//top right
				al_draw_rectangle(400, 0, 640, 640, al_map_rgb(250, 250, 250), 10);//bottom right
				
				al_draw_text(font2, al_map_rgb(200, 200, 200), 50, 420, 0, "OGRE");
				al_draw_textf(font2, al_map_rgb(200, 200, 200), 50, 470, 0, "%d", enermyHealth);
				al_draw_text(font2, al_map_rgb(200, 200, 200), 50, 500, 0, "----");
				al_draw_text(font2, al_map_rgb(200, 200, 200), 50, 530, 0, "100");


				al_draw_text(font2, al_map_rgb(200, 200, 200), 450, 420, 0, "LINK");

				al_draw_text(font2, al_map_rgb(200,200,200), 450, 500, 0, "FIGHT");
				al_draw_text(font2, al_map_rgb(200,200,200), 450, 550, 0, "RUN");

				//selection boxes
				if(BattleState == PLAYER)
					al_draw_rectangle(440, 410, 580, 460, al_map_rgb(250, 250, 250), 3);
				if (BattleState == FIGHT)
					al_draw_rectangle(440, 490, 600, 540, al_map_rgb(250, 250, 250), 3);
				if (BattleState == RUN)
					al_draw_rectangle(440, 540, 560, 590, al_map_rgb(250, 250, 250), 3);
				

			}


			if (state == FOREST) {
				al_clear_to_color(al_map_rgb(50, 200, 50));
				al_draw_bitmap(house, 512 - cameraX, 25 + 512 - cameraY, 0);

				if (monster1.isDead() == false) {
					monster1.Draw(cameraX, cameraY);

				}

				for (int i = 0; i < 32; i++) {
					for (int j = 0; j < 32; j++) {

						if (map[i][j] == 100) {
							al_draw_bitmap(tree, i * 32 - cameraX, j * 32 - cameraY, 0);
						}
						if (map[i][j] == 2) {
							al_draw_bitmap(flower, i * 32 - cameraX, j * 32 - cameraY, 0);
						}
						/*if (map[i][j] == 3) {
							al_draw_filled_rectangle(i * 32 - cameraX, j * 32 - cameraY, i * 32 - cameraX+32, j * 32 - cameraY+32, al_map_rgb(0, 0, 0));
						}*/
						if (map[i][j] == 103) {
							al_draw_bitmap(rock, i * 32 - cameraX, j * 32 - cameraY, 0);
						}
					}

				}
			}//end render for state forest


			if(state != BATTLE){
			//always draw the player in the center of the map, make the map move around *him*!
			//al_draw_bitmap(player, screenWidth / 2, screenHeight / 2, 0);
			if (dir == RIGHT)
				al_draw_bitmap_region(player, curFrame * frameWidth, 48, frameWidth, frameHeight, screenWidth / 2, screenHeight / 2, 0);
			if (dir == LEFT)
				al_draw_bitmap_region(player, curFrame * frameWidth, 0, frameWidth, frameHeight, screenWidth / 2, screenHeight / 2, 0);
			if (dir == UP)
				al_draw_bitmap_region(player, curFrame * frameWidth, 48 * 2, frameWidth, frameHeight, screenWidth / 2, screenHeight / 2, 0);
			if (dir == DOWN)
				al_draw_bitmap_region(player, curFrame * frameWidth, 48 * 3, frameWidth, frameHeight, screenWidth / 2, screenHeight / 2, 0);

			if (sword) {
				if (dir == RIGHT) {

					al_draw_filled_rectangle(screenWidth / 2 + 32, screenHeight / 2 + 25, screenWidth / 2 + 32 + 32, screenHeight / 2 + 25 + 5, al_map_rgb(255, 255, 255));
				}
				if (dir == LEFT) {

					al_draw_filled_rectangle(screenWidth / 2, screenHeight / 2 + 25, screenWidth / 2 - 32, screenHeight / 2 + 25 + 5, al_map_rgb(255, 255, 255));
				}
				if (dir == UP) {

					al_draw_filled_rectangle(screenWidth / 2 + 16, screenHeight / 2, screenWidth / 2 + 16 + 5, screenHeight / 2 - 25, al_map_rgb(255, 255, 255));
				}
				if (dir == DOWN) {

					al_draw_filled_rectangle(screenWidth / 2 + 20, screenHeight / 2 + 30, screenWidth / 2 + 20 + 5, screenHeight / 2 + 30 + 32, al_map_rgb(255, 255, 255));
				}
			}

	//draw life bar
		
				al_draw_filled_rectangle(screenWidth - 200, 50, screenWidth - 100, 25, al_map_rgb(255, 255, 255));
				al_draw_filled_rectangle(screenWidth - 200, 50, screenWidth - 200 + life, 25, al_map_rgb(250, 0, 0));
			}

			al_flip_display();
		}
	}
	//game over screen
	al_rest(2);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_text(font, al_map_rgb(200, 200, 50), 50, 250, 0, "Game ");
	al_draw_text(font, al_map_rgb(200, 200, 50), 100, 350, 0, "Over");
	al_flip_display();
	al_rest(2);


	al_destroy_bitmap(player);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
int MapCollision(int direction, int X, int Y, int map[32][32]) {

	if (direction == RIGHT) {
		if ((map[(X + 32 + 2) / 32][Y / 32] >= 100) || (map[(X + 32 + 2) / 32][(Y + 16) / 32] >= 100) || (map[(X + 32 + 2) / 32][(Y + 32) / 32] >= 100)) {
			//cout << "right collision!";
			return true;
		}
		else
			return false;


	}
	if (direction == LEFT) {
		if ((map[(X - 2) / 32][Y / 32] >= 100) || (map[(X - 2) / 32][(Y + 16) / 32] >= 100) || (map[(X - 2) / 32][(Y + 32) / 32] >= 100)) {
			//cout << "left collision!";
			return true;
		}
		else
			return false;


	}
	if (direction == UP) {
		if ((map[(X) / 32][(Y - 2) / 32] >= 100) || (map[(X + 16) / 32][((Y - 2) + 16) / 32] >= 100) || (map[(X + 32) / 32][((Y - 2) + 32) / 32] >= 100)) {
			//cout << "up collision!";
			return true;
		}
		else
			return false;


	}
	if (direction == DOWN) {
		if ((map[(X) / 32][(Y + 32 + 2) / 32] >= 100) || (map[(X + 16) / 32][(Y + 32 + 2) / 32] >= 100) || (map[(X + 32) / 32][(Y + 32 + 2) / 32] >= 100)) {
			//cout << "down collision!";
			return true;
		}
		else
			return false;


	}

}

int InsideCollision(int direction, int X, int Y, int map[10][10]) {

	if (direction == RIGHT) {
		if ((map[(X + 32 + 5) / 32][Y / 32] >= 100) || (map[(X + 32 + 5) / 32][(Y + 16) / 32] >= 100) || (map[(X + 32 + 5) / 32][(Y + 32) / 32] >= 100)) {
			//cout << "right collision!";
			return true;
		}
		else
			return false;


	}
	if (direction == LEFT) {
		if ((map[(X - 5) / 32][Y / 32] >= 100) || (map[(X - 5) / 32][(Y + 16) / 32] >= 100) || (map[(X - 5) / 32][(Y + 32) / 32] >= 100)) {
			//cout << "left collision!";
			return true;
		}
		else
			return false;


	}
	if (direction == UP) {
		if ((map[(X) / 32][(Y - 5) / 32] >= 100) || (map[(X + 16) / 32][((Y - 5) + 16) / 32] >= 100) || (map[(X + 32) / 32][((Y - 5) + 32) / 32] >= 100)) {
			//cout << "up collision!";
			return true;
		}
		else
			return false;


	}
	if (direction == DOWN) {
		if ((map[(X) / 32][(Y + 32 + 5) / 32] >= 100) || (map[(X + 16) / 32][(Y + 32 + 5) / 32] >= 100) || (map[(X + 32) / 32][(Y + 32 + 5) / 32] >= 100)) {
			//cout << "down collision!";
			return true;
		}
		else
			return false;


	}

}

bool boxCollision(int x1, int y1, int x2, int y2) {

	if ((x1 + 32 > x2) &&
		(x1 < x2 + 32) &&
		(y1 + 32 > y2) &&
		(y1 < y2 + 32)) {
		cout << "collision!" << endl;
		return true;
	}

	else
		return false;

}

