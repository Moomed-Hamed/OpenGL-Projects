#pragma once
#include "World.h"

/*------------------------------- Pathfinding -------------------------------*/
#define MAX_PATH_NODES 16

struct Path_Node
{
	vec3 Position;
};

struct Enemy_Path
{
Path_Node Nodes[MAX_PATH_NODES];
};

void EnemyPathInit(Enemy_Path* path, Path_Node* nodes, uint NumNodes)
{

	for (uint i = 0; i < NumNodes; ++i)
	{
		path->Nodes[i] = *(nodes + i);
	}

	for (uint i = NumNodes; i < MAX_PATH_NODES; ++i)
	{
		path->Nodes[i].Position = vec3(-1, -1, -1);
	}
}

/*------------------------------- Enemies -------------------------------*/
#define NUM_ENEMIES 64

#define FREE_ENEMY  0
#define GRUNT_ENEMY 1
#define SMALL_ENEMY 2
#define HEAVY_ENEMY 3
#define DONE_ENEMY  4
#define DEAD_ENEMY  5

struct Enemy
{
	uint Type;

	vec3 Position;

	float Health;
	float Shield;

	Path_Node* Next_Node;
};

void EnemiesInit(Enemy* enemies)
{
	for (uint i = 0; i < NUM_ENEMIES; ++i) *(enemies + i) = {};
}

void SpawnEnemy(Enemy* enemies, uint type, vec3 spawnpos, Path_Node* pathnode)
{
	for (uint i = 0; i < NUM_ENEMIES; ++i)
	{
		Enemy* enemy = enemies + i;
		if (enemy->Type != FREE_ENEMY) continue;

		enemy->Type = type;
		enemy->Position = spawnpos;
		enemy->Next_Node = pathnode;

		switch (type)
		{
		case GRUNT_ENEMY:
		{
			enemy->Health = 100;
			enemy->Shield = 50;
		} break;

		case SMALL_ENEMY:
		{
			enemy->Health = 100;
			enemy->Shield = 0;
		} break;

		case HEAVY_ENEMY:
		{
			enemy->Health = 200;
			enemy->Shield = 100;
		} break; //from the switch statement
		}
		break; //from the for loop
	}
}

void RemoveEnemy(Enemy* enemy) { *enemy = {}; }

void UpdateEnemies(Enemy* enemies, float DeltaTime)
{
	for (uint i = 0; i < NUM_ENEMIES; ++i)
	{
		Enemy* enemy = enemies + i;
		if (enemy->Type == FREE_ENEMY) continue; //skip inactive enemies

		if (enemy->Health <= 0)
		{
			enemy->Type = DEAD_ENEMY;
			continue;
		}

		vec3 Direction = enemy->Next_Node->Position - enemy->Position;
		if (glm::length(Direction) < .1) enemy->Next_Node += 1;
		if (enemy->Next_Node->Position.x != -1)
		{			
			Direction = enemy->Next_Node->Position - enemy->Position;
		}
		else
		{
			enemy->Type = DONE_ENEMY; //if enemy has reached the end flag them
			continue;
		}

		Direction = glm::normalize(Direction) * .5f;
		enemy->Position += Direction * DeltaTime;
	}
}

/*------ Rendering ------*/

struct Enemy_Renderable
{
	vec3 Position;
	vec3 Scale;
	vec3 Color;
};

struct Enemy_Renderer
{
	Enemy_Renderable* EnemyMem;

	uint NumEnemies;
};

void EnemiesToRenderable(Enemy* enemy, Enemy_Renderer* renderer)
{
	Enemy_Renderable* memory = renderer->EnemyMem;
	renderer->NumEnemies = 64;

	for (uint i = 0; i < 64; ++i)
	{
		if (enemy->Type == FREE_ENEMY) //if enemy is inactive, don't render it
		{
			renderer->NumEnemies -= 1;
			++enemy;
			continue;
		}

		switch (enemy->Type) {
		case GRUNT_ENEMY:
		{
			float health = (enemy->Health + enemy->Shield) / 150;

			memory->Position = enemy->Position;
			memory->Scale = vec3(.5, 1, .5);
			memory->Color = vec3(health, 0, 0);
		} break;

		case SMALL_ENEMY:
		{
			float health = (enemy->Health + enemy->Shield) / 100;

			memory->Position = enemy->Position;
			memory->Scale = vec3(.25, .75, .25);
			memory->Color = vec3(health, 0, 0);
		} break;

		case HEAVY_ENEMY:
		{
			float health = (enemy->Health + enemy->Shield) / 300;

			memory->Position = enemy->Position;
			memory->Scale = vec3(.65, 1.25, .65);
			memory->Color = vec3(health, 0, 0);
		} break; }

		++enemy;
		++memory;
	}	
}

/*------------------------------- Enemy Shaders -------------------------------*/
const char* EnemyVertSource = R"glsl(
    #version 330 core

	layout (location = 0) in vec3 Vertex;
	layout (location = 1) in vec3 Position;
	layout (location = 2) in vec3 Scale;
	layout (location = 3) in vec3 Color;

	uniform mat4 Proj;
	uniform mat4 View;

	out vec3 color;

    void main()
    {
		vec4 FinalPos = vec4(Scale * Vertex, 1);
		FinalPos.xyz += Position;

		gl_Position = Proj * View * FinalPos;
		color = Color;
    }
		)glsl";

const char* EnemyFragSource = R"glsl(
    #version 330 core

	in vec3 color;

	out vec4 EnemyColor;

    void main()
    {
		EnemyColor = vec4(color, 1);
    }
		)glsl";
