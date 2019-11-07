#pragma once
#include "LeapMotion.h"

#define MEM_PAGE_SIZE 4096

struct Mem_Page
{
	byte Memory[MEM_PAGE_SIZE];
};

/* num pages each game system needs */
#define TILE_MEM_PAGES   1 // 1 page = ?
#define TOWER_MEM_PAGES  1 // 1 page = ?
#define ENEMY_MEM_PAGES  1 // 1 page = ?
#define BULLET_MEM_PAGES 1 // 1 page = ?

#define TOTAL_MEM_PAGES (TILE_MEM_PAGES + TOWER_MEM_PAGES + ENEMY_MEM_PAGES\
						+ BULLET_MEM_PAGES)

struct Game_Memory
{
	Mem_Page* TilePointers[TILE_MEM_PAGES];
	Mem_Page* TowerPointers[TOWER_MEM_PAGES];
	Mem_Page* EnemyPointers[ENEMY_MEM_PAGES];
	Mem_Page* BulletPointers[ENEMY_MEM_PAGES];
};

void GameMemoryInit(Game_Memory* GameMemory, Mem_Page* memory)
{
	//Give each game system the appropriate amount of memory pages
	for (uint i = 0; i < TILE_MEM_PAGES ;  ++i) GameMemory->TilePointers[i]   = memory++;
	for (uint i = 0; i < TOWER_MEM_PAGES;  ++i) GameMemory->TowerPointers[i]  = memory++;
	for (uint i = 0; i < ENEMY_MEM_PAGES;  ++i) GameMemory->EnemyPointers[i]  = memory++;
	for (uint i = 0; i < BULLET_MEM_PAGES; ++i) GameMemory->BulletPointers[i] = memory++;
}

/*------------------ Misc, consider moving to another file ------------------*/

struct GameMouse
{
	double RawX;
	double RawY;
	double NormX;
	double NormY;
};

struct GameButton
{
	uint ID;
	uint FirstPress;
};