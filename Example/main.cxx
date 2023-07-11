//
//  main.c
//  F&EYSS
//
//  Created by Finxx1 on 4/22/23.
//

// Tip of the Day: Ow.

/*

 ##  ##  ##
   ######
 ##########
	 ##
 ####  ####

 */

#define _Bool bool

extern "C" {
#include "Neon.h"
}

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <vector>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

float proximity(float x1, float y1, float x2, float y2) {
	return sqrt(fabsf(x1 - x2) * fabsf(x1 - x2) + fabsf(y1 - y2) * fabsf(y1 - y2));
}

short* mx, * my;

float px, py, pdx, pdy;

bool start = true;

bool* keys;
bool* keysdown;

int dashtimer = 0;
int firetimer = 0;
int runphysics = 0;
int hp = 5;

size_t invulnerable = 0;

typedef struct {
	double hp;
	float x, y;
	float dx, dy;
	unsigned char type;
} enemy;

std::vector<enemy> enemies;

typedef struct {
	float x, y;
	float d;
	float v;
	float pen;
	double damage;
	unsigned char type;
} bullet;

std::vector<bullet> bullets;

void dostart(void) {
	NeonAddImage((char*)"1070913083222007939.png");
	
	srand((unsigned int)time(NULL));

	mx = NeonMouseX();
	my = NeonMouseY();
	px = 1440 / 2;
	py = 1080 / 2;
	pdx = 0;
	pdy = 0;



	keys = NeonKeys();
	keysdown = NeonKeysDown();
}

void doenemy(void) {
	for (unsigned short i = 0; i < enemies.size(); i++) {
		double foo = 3.141592 * MIN(ceil(fabsf((py - enemies[i].y) - fabsf((py - enemies[i].y)))), 1);
		double d = atan((px - enemies[i].x) / (py - enemies[i].y)) + foo;
		enemies[i].dx = sin(d) * 5;
		enemies[i].dy = cos(d) * 5;
	}
}

void dobullet(void) {
	for (unsigned short i = 0; i < bullets.size(); i++) {

		bullets[i].x -= sin(bullets[i].d) * bullets[i].v;
		bullets[i].y -= cos(bullets[i].d) * bullets[i].v;
	}

next:
	for (unsigned short i = 0; i < bullets.size(); i++) {
		for (int j = 0; j < enemies.size(); j++) {
			if (proximity(enemies[j].x, enemies[j].y, bullets[i].x, bullets[i].y) < 30) {
				enemies[j].hp--;
				if (enemies[j].hp <= 0) {
					enemies.erase(enemies.begin() + j);
				}
				if (bullets[i].pen == 0) {
					bullets.erase(bullets.begin() + i);
					goto next;
				}
				bullets[i].pen--;
			}
		}
	}
}

void doplayer(void) {
	if (!keys[0xA2]) {
		if (keys[0x41] && pdx > -5)
			pdx -= 2;
		if (keys[0x44] && pdx < 5)
			pdx += 2;
		if (keys[0x53] && pdy < 5)
			pdy += 2;
		if (keys[0x57] && pdy > -5)
			pdy -= 2;

		pdy *= 0.83;
		pdx *= 0.83;

		if (keys[0xA0]) {
			if (dashtimer == 0) {
				dashtimer = 30;
				pdx *= 5;
				pdy *= 5;
				invulnerable = 20;
			}
		}
	}

	if (firetimer) firetimer--;

	if (keys[0x1] && !firetimer) {
		firetimer = 7;

		bullet bar;
		bar.type = 1;
		bar.pen = 0;
		bar.x = px;
		bar.y = py;
		bar.damage = 3.0;
		bar.v = 10;

		double foo = 3.141592 * MIN(ceil(fabsf((py - *my) - fabsf((py - *my)))), 1);
		double d = atan((px - *mx) / (py - *my)) + foo;
		bar.d = d;

		bullets.push_back(bar);
	}

	if (!invulnerable) {
		for (int i = 0; i < enemies.size(); i++) {
			if (proximity(enemies[i].x, enemies[i].y, px, py) < 25) {
				hp--;
				invulnerable = 60;
				break;
			}
		}
	}

	if (dashtimer > 0) {
		dashtimer--;
	}
	if (invulnerable > 0) {
		invulnerable--;
	}
}

int spawning = 0;

void update(double DeltaTime) {
	if (start) {
		start = false;
		dostart();
	}

	runphysics = (runphysics + 1) % 2; // update physics every other frame

	if (runphysics) {
		doplayer(); // yes, no deltatime. in f&eyss, not slowing down during lag would make the game _much_ harder to play.
		doenemy();
	}

	dobullet();

	if (!spawning) {
		enemy foo;
		foo.x = -25;
		foo.y = rand() % 1080;
		foo.dx = 0;
		foo.dy = 0;
		foo.hp = 3.0;

		enemies.push_back(foo);
	}

	px += pdx;
	py += pdy;
	for (unsigned short i = 0; i < enemies.size(); i++) {
		enemies[i].x += enemies[i].dx;
		enemies[i].y += enemies[i].dy;
	}

	spawning = (spawning + 1) % 60;
}

NeonDraw* draw(void) {
	NeonDraw* r = (NeonDraw*)NeonCreateDraw();

	NeonDrawImage(r, (int)px - 25, (int)py - 25, 50, 50, 0);

	for (unsigned short i = 0; i < enemies.size(); i++) {
		NeonDrawImage(r, (int)enemies[i].x - 25, (int)enemies[i].y - 25, 50, 50, 0);
	}

	for (unsigned short i = 0; i < bullets.size(); i++) {
		NeonDrawImage(r, (int)bullets[i].x - 5, (int)bullets[i].y - 5, 10, 10, 0);
	}

	NeonDrawRect(r, 10, 10, (double)(960 - 20) * ((double)hp / 5.0), 25, NeonColor(127, 255, 127, 255));

	return r;
}

int main(int argc, const char* argv[]) {
	NeonRun((char*)"Forever and Ever You Shall Slaughter", 100, 100, 960, 720, draw, update, 60);
	return 0;
}