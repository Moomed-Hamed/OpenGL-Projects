#pragma once
#include "User.h"

struct Game_Level
{
	float Health;
	float Shield;

	float Energy;
	uint Kills;

	Game_World World;
	Enemy_Path Path;

	Bullet Bullets[NUM_BULLETS];

	Enemy Enemies[NUM_ENEMIES];
	Turret Turrets[NUM_TURRETS];

	Cursor_3D Cursor;
	Level_Stats Stats;
};

struct Level_Data
{
	uint TileTypes[64];
	Enemy_Path Path;
};

//This will (probably(not)) be replaced with a function that loads from a file
void LoadLevelData(Level_Data* level_data)
{
	uint TestMap[64] = {
	4, 5, 5, 5, 5, 5, 5, 4,
	4, 5, 5, 5, 5, 5, 5, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3,
	8, 8, 8, 8, 8, 8, 8, 8,
	3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3 };

	for (uint i = 0; i < NUM_TILES; ++i) level_data->TileTypes[i] = TestMap[i];

	Path_Node PathNodes[8] = { vec3(0.5,1,4.5), vec3(1.5,1,4.5), vec3(2.5,1,4.5), vec3(3.5,1,4.5),
							   vec3(4.5,1,4.5), vec3(5.5,1,4.5), vec3(6.5,1,4.5), vec3(7.5,1,4.5) };
	Enemy_Path LevelPath = {};
	EnemyPathInit(&LevelPath, PathNodes, 8);

	level_data->Path = LevelPath;
}

void GameLevelInit(Game_Level* level, Level_Data* level_data)
{
	level->Health = 750;
	level->Shield = 250;

	level->Energy = 100;
	level->Kills = 0;

	GameWorldInit(&level->World, level_data->TileTypes);
	level->Path = level_data->Path;

	BulletsInit(level->Bullets);

	EnemiesInit(level->Enemies);
	TurretsInit(level->Turrets);

	CursorInit(&level->Cursor);

	level->Stats = {};
}

void SimulateCollisions(Bullet* bullets, Enemy* enemies)
{
	for (uint i = 0; i < NUM_BULLETS; ++i)
	{
		Bullet* bullet = bullets + i;
		if (bullet->Type == FREE_BULLET) continue;

		for (uint j = 0; j < NUM_ENEMIES; ++j)
		{
			Enemy* enemy = enemies + j;
			if (enemy->Type == FREE_ENEMY) continue;

			Cube_Collider EnemyHitbox = {};
			CubeColliderInit(&EnemyHitbox, enemy->Position - vec3(.25,0,.25), vec3(.5, 1, .5));

			if (PointInCube(bullet->Position, EnemyHitbox))
			{
				enemy->Health -= bullet->Damage;
				RemoveBullet(bullet);
			}
		}
	}
}

void UpdateLevel(Game_Level* level, float deltatime)
{
	level->Stats.RoundTime += deltatime;

	for (uint i = 0; i < NUM_TURRETS; ++i)
	{
		if ((level->Turrets + i)->Cooldown < 0)
		{
			level->Stats.BulletsFired += 1;
			level->Stats.DamageDealt += (level->Turrets + i)->Bullet_Damage;
		}
	}

	for (uint i = 0; i < NUM_ENEMIES; ++i)
	{
		Enemy* enemy = level->Enemies + i;

		if (enemy->Type == DEAD_ENEMY)
		{
			level->Stats.EnemiesKilled += 1;
			RemoveEnemy(enemy);

			level->Energy += 50;
			level->Stats.EnergyCollected += 50;
			continue;
		}

		if (enemy->Type == DONE_ENEMY)
		{
			if (level->Shield > 0)
			{
				level->Stats.DamageTaken += 50;
				level->Shield -= 50;
				if (level->Shield < 0) level->Shield = 0;
			}
			else
			{
				level->Stats.DamageTaken += 50;
				level->Health -= 50;
				if (level->Health < 0) level->Health = 0;
			}
			RemoveEnemy(enemy);
		}
	}
}

/*------------------------------- User Interface -------------------------------*/

#define NUM_QUADS 5

struct Quad_Renderable
{
	vec2 Position;
	vec2 Scale;
	vec3 Color;
};

struct HUD_Renderer
{
	Quad_Renderable* QuadMem;
	uint NumQuads;
};

void HUDToRenderable(Game_Level* level, HUD_Renderer* renderer)
{
	renderer->NumQuads = NUM_QUADS;

	Quad_Renderable* memory = renderer->QuadMem;
	float factor = 0; //used for calculations

	// Energy bar bar
	factor = (level->Energy / 1000);
	memory->Position = vec2(.8, -.8);
	memory->Scale = vec2(1, factor * 15);
	memory->Color = vec3(.83, .68, .21);
	++memory;

	// Energy bar base
	memory->Position = vec2(.8, -.8);
	memory->Scale = vec2(1, 15);
	memory->Color = vec3(.1, .1, .1);
	++memory;

	// Shield bar
	factor = (level->Shield / 250);
	memory->Position = vec2(-.8, -.8);
	memory->Scale = vec2(factor * 26, 1);
	memory->Color = vec3(.1, 1, 1);
	++memory;

	// Health bar
	factor = (level->Health / 750);
	memory->Position = vec2(-.8, -.8);
	memory->Scale = vec2(factor * 26, 1);
	memory->Color = vec3(1, 0, .1);
	++memory;

	// Health bar base
	memory->Position = vec2(-.8, -.8);
	memory->Scale = vec2(26, 1);
	memory->Color = vec3(.1, .1, .1); // this quad is farthest back
	//++memory;
}

/*------------------------------- HUD Shaders -------------------------------*/
const char* QuadVertSource = R"glsl(
    #version 330 core

	layout (location = 0) in vec2 Vertex;

	layout (location = 1) in vec2 Position;
	layout (location = 2) in vec2 Scale;
	layout (location = 3) in vec3 Color;

	out vec3 color;

    void main()
    {
		color = Color;
		gl_Position = vec4( (Vertex * Scale) + Position, 0.0, 1.0);
    }
		)glsl";

const char* QuadFragSource = R"glsl(
    #version 330 core

	in vec3 color;

	out vec4 QuadColor;

    void main()
    {
		QuadColor = vec4(color, 1);
    }
		)glsl";

/*------------------------------- Webpage Creation -------------------------------*/
#include <stdio.h>

void SaveToWebpage(Level_Stats* stats)
{
	remove("Last_Round.html");
	FILE* File;
	fopen_s(&File, "Last_Round.html", "w");
	if (!File) { out("Error: Could not create file"); return; }

	fprintf(File, "<!DOCTYPE html><html style='background-color:MidnightBlue'>");
	fprintf(File, "<title>Round Stats</title> <head> <style>");

	fprintf(File, "body { font-family: 'Verdana'; } paragraph { font-family: 'Verdana'; } ");
	fprintf(File, "header { font-family: 'Verdana'; background-color: #0a0a2d; padding: 20px; text-align: center; font-size: 35px; color: white; border: 2px solid white; }");
	fprintf(File, "table, th, td { border: none; border-collapse: collapse; }");
	fprintf(File, "th, td { padding: 2px; text-align: left; border: 2px solid white; }");
	fprintf(File, "button { background-color: #4CAF50; border: 1px solid black; color: black; padding: 6px 6px; text-align:center; display: inline-block; font-size: 16px; }");
	
	fprintf(File, "</style> </head> <header>Last Round Breakdown</header> <body>");

	fprintf(File, "<table border='1' style='width:100%%'>");
	fprintf(File, "<tr style='background-color:Tomato'> <th style='width:30%%'>Statistic</th> <th style='width:50%%'>Description</th> <th style='width:20%%'>Value</th> </tr>");

	fprintf(File, "<tr  style='background-color:DarkCyan'>");
	fprintf(File, "<td>Round Time</td> <td>Round time in seconds</td>");
	fprintf(File, "<td>%f</td> </tr>", stats->RoundTime);

	fprintf(File, "<tr style='background-color:DarkCyan'>");
	fprintf(File, "<td>Enemies Killed</td> <td>Number of enemies killed</td>");
	fprintf(File, "<td>%u</td> </tr>", stats->EnemiesKilled);

	fprintf(File, "<tr style='background-color:SteelBlue'>");
	fprintf(File, "<td>Energy Collected</td> <td>Total Energy Collected</td>");
	fprintf(File, "<td>%f</td> </tr>", stats->EnergyCollected);

	fprintf(File, "<tr style='background-color:SteelBlue'>");
	fprintf(File, "<td>Turrets Placed</td> <td>Number of turrets placed</td>");
	fprintf(File, "<td>%u</td> </tr>", stats->TurretsPlaced);

	fprintf(File, "<tr style='background-color:SteelBlue'>");
	fprintf(File, "<td>Damage dealt</td> <td>Total Inflicted Damege</td>");
	fprintf(File, "<td>%f</td> </tr>", stats->DamageDealt);

	fprintf(File, "<tr style='background-color:SteelBlue'>");
	fprintf(File, "<td>Damage taken</td> <td>Total Damage Taken</td>");
	fprintf(File, "<td>%f</td> </tr>", stats->DamageTaken);

	fprintf(File, "<tr style='background-color:SteelBlue'>");
	fprintf(File, "<td>Bullets fired</td> <td>Number of bullets fired</td>");
	fprintf(File, "<td>%u</td> </tr>", stats->BulletsFired);

	fprintf(File, "<tr style='background-color:Tomato;'> <th border='3' colspan='3'><br></th> </tr>");
	fprintf(File, "</table> </body> </html>");
}