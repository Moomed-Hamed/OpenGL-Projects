#pragma once
#include "Everything.h"

struct Cube_Collider
{
	vec3 Min;
	vec3 Max;
};

void CubeColliderInit(Cube_Collider* cube, vec3 position, vec3 dimentions)
{
	cube->Min = position;
	cube->Max = position + dimentions;
}

bool PointInCube(vec3 point, Cube_Collider cube)
{
	if (point.x < cube.Min.x || point.x > cube.Max.x) return false;
	if (point.z < cube.Min.z || point.z > cube.Max.z) return false;
	if (point.y < cube.Min.y || point.y > cube.Max.y) return false;
	return true;
}

/*------------------------------- Bullets -------------------------------*/

#define NUM_BULLETS 112

#define FREE_BULLET 0
#define FRIENDLY_BULLET 1
#define ENEMY_BULLET 2

struct Bullet
{
	uint Type;

	vec3 Position;
	vec3 Velocity;

	float Damage;
	float Duration;
};

void BulletsInit(Bullet* bullets)
{
	for (uint i = 0; i < NUM_BULLETS; ++i) *(bullets + i) = {};
}

void FireBullet(Bullet* bullets, float damage, vec3 position, vec3 velocity)
{
	for (uint i = 0; i < NUM_BULLETS; ++i)
	{
		if ((bullets + i)->Type != FREE_BULLET) continue;

		(bullets + i)->Type = FRIENDLY_BULLET;
		(bullets + i)->Damage = damage;
		(bullets + i)->Position = position;
		(bullets + i)->Velocity = velocity;
		(bullets + i)->Duration = 3;
		break;
	}
}

void RemoveBullet(Bullet* bullet)
{
	*bullet = {};
}

void UpdateBullets(Bullet* bullets, float DeltaTime)
{
	for (uint i = 0; i < NUM_BULLETS; ++i)
	{
		if ((bullets + i)->Type == FREE_BULLET) continue;

		(bullets + i)->Duration -= DeltaTime;

		if ((bullets + i)->Duration <= 0) RemoveBullet(bullets + i);//destroy it if it's been too long

		(bullets + i)->Position += (bullets + i)->Velocity * DeltaTime;
	}
}

/*------ Rendering ------*/

struct Bullet_Renderable
{
	vec3 Position;
	vec3 Color;
};

struct Bullet_Renderer
{
	Bullet_Renderable* BulletMem;
	uint NumBullets;
};

void BulletsToRenderable(Bullet* bullet, Bullet_Renderer* renderer)
{
	renderer->NumBullets = NUM_BULLETS;
	Bullet_Renderable* memory = renderer->BulletMem;
	for (uint i = 0; i < 64; ++i)
	{
		memory->Position = bullet->Position;
		memory->Color = vec3(1, 1, 0);
		++memory;
		++bullet;
	}
}

/*------------------------------- Bullet Shaders -------------------------------*/
const char* BulletVertSource = R"glsl(
    #version 330 core

	layout (location = 0) in vec3 Vertex;
	layout (location = 1) in vec3 Position;
	layout (location = 2) in vec3 Color;

	uniform mat4 Proj;
	uniform mat4 View;

	out vec3 color;

    void main()
    {
		color = Color;
		gl_Position = Proj * View * vec4(Position + (Vertex * vec3(.1,.1,.1)), 1);
    }
		)glsl";

const char* BulletFragSource = R"glsl(
    #version 330 core

	in vec3 color;

	out vec4 BulletColor;

    void main()
    {
		BulletColor = vec4(color,1);
    }
		)glsl";
