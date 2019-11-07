#pragma once
#include "Enemies.h"

#define NUM_TURRETS 64

#define FREE_TURRET  0
#define SMALL_TURRET 1
#define LARGE_TURRET 2

struct Turret
{
	uint Type;
	uint x, z; // between [0,7] each

	float Health;
	float Shield;

	float Bullet_Damage; // Damage per bullet
	float Fire_Rate;	 // Bullets per second

	float Cooldown;

	Enemy* Target; //who to shoot
};

void TurretsInit(Turret* turrets)
{
	for (uint i = 0; i < NUM_TURRETS; ++i) *(turrets + i) = {};
}

void UpdateTurrets(Turret* turrets, Bullet* bullets, Enemy* enemies, float DeltaTime)
{
	for (uint i = 0; i < NUM_TURRETS; ++i)
	{
		Turret* turret = (turrets + i);
		if (turret->Type == FREE_TURRET) continue;

		if (turret->Target && turret->Target->Type) // is aiming at an active enemy
		{
			if (turret->Cooldown <= 0)
			{
				vec3 Position = vec3(turret->x, 1.75, turret->z);
				vec3 Velocity = glm::normalize(turret->Target->Position - Position);
				Velocity.y = 0;
				Velocity *= 8;
				vec3 EnemyDirection = glm::normalize(turret->Target->Next_Node->Position - turret->Target->Position);
				EnemyDirection *= DeltaTime; //try to compensate for enemy movement
				Velocity += 2.f * EnemyDirection;
				FireBullet(bullets, turret->Bullet_Damage, Position, Velocity);

				turret->Cooldown = (float)1 / turret->Fire_Rate;
			} else turret->Cooldown -= DeltaTime;
		}
		else
		{
			for (uint i = 0; i < NUM_ENEMIES; ++i) // look for an enemy to shoot
			{
				if ((enemies + i)->Type == FREE_ENEMY) continue;
				turret->Target = (enemies + i); break;
			}
		}
	}
}

void SpawnTurret(Turret* turrets, uint xPos, uint zPos, uint type)
{
	for (uint i = 0; i < NUM_TURRETS; ++i)
	{
		Turret* turret = turrets + i;

		if (turret->Type != FREE_TURRET) continue;

		turret->Type = type;

		turret->x = xPos;
		turret->z = zPos;

		turret->Target = 0;

		turret->Health = 100;
		turret->Shield = 100;

		turret->Bullet_Damage = 10;
		turret->Fire_Rate = 4;
		break;
	}
}

/*------ Rendering ------*/

struct Turret_Renderable
{
	vec3 Position;
};

struct Turret_Renderer
{
	Turret_Renderable* TurretMem;
	uint NumTurrets;
};

void TurretsToRenderable(Turret* turret, Turret_Renderer* renderer)
{
	renderer->NumTurrets = NUM_TURRETS;
	Turret_Renderable* memory = renderer->TurretMem;
	for (uint i = 0; i < NUM_TURRETS; ++i)
	{
		if (turret->Type == FREE_TURRET)
		{
			renderer->NumTurrets -= 1;
			++turret;
			continue;
		}
		else
		{
			memory->Position = vec3(turret->x, 1, turret->z);
			++memory;
			++turret;
		}
	}
}
/*------------------------------- Turret Shaders -------------------------------*/
const char* TurretVertSource = R"glsl(
    #version 330 core

	layout (location = 0) in vec3 Vertex;
	layout (location = 1) in vec3 Position;

	uniform mat4 Proj;
	uniform mat4 View;

    void main()
    {
		gl_Position = Proj * View * vec4(Position + (Vertex * vec3(.2,1,.2)), 1);
    }
		)glsl";

const char* TurretFragSource = R"glsl(
    #version 330 core

	out vec4 TurretColor;

    void main()
    {
		TurretColor = vec4(0, 1, 1, 1);
    }
		)glsl";