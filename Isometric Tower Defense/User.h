#pragma once
#include "Towers.h"

struct Level_Stats
{
	float RoundTime;
	uint EnemiesKilled;
	float EnergyCollected;
	uint TurretsPlaced;
	float DamageDealt;
	float DamageTaken;
	uint BulletsFired;
};

struct Game_Camera
{
	vec3 Position;
	vec3 Velocity;

	float Yaw;
	float Pitch;

	vec3 Front;
	vec3 Up;
	vec3 Right;

	float MouseSensitivity;
};

void CameraInit(Game_Camera* camera)
{
	camera->Position = vec3(3, 3, 3);
	camera->Velocity = vec3(0, 0, 0);

	camera->Pitch = 0.0f;
	camera->Yaw   = 0.0f;

	camera->MouseSensitivity = .005f;
}

void CameraProcessMouseMovement(Game_Camera* camera, float xoffset, float yoffset)
{
	camera->Yaw   += (xoffset * camera->MouseSensitivity) / TWOPI;
	camera->Pitch += (yoffset * camera->MouseSensitivity) / TWOPI;

	if (camera->Pitch >  PI / 2.01) camera->Pitch =  PI / 2.01;
	if (camera->Pitch < -PI / 2.01) camera->Pitch = -PI / 2.01;

	camera->Front.y = sin(camera->Pitch);
	camera->Front.x = cos(camera->Pitch) * cos(camera->Yaw);
	camera->Front.z = cos(camera->Pitch) * sin(camera->Yaw);

	camera->Front = glm::normalize(camera->Front);
	camera->Right = glm::normalize(glm::cross(camera->Front, vec3(0, 1, 0)));
	camera->Up = glm::normalize(glm::cross(camera->Right, camera->Front));
}

#define CAM_FORWARD  0
#define CAM_BACKWARD 1
#define CAM_LEFT	 2
#define CAM_RIGHT	 3
void CameraProcessKeyboard(Game_Camera* camera, int direction, float velocity, float DeltaTime)
{
	if (direction == CAM_FORWARD ) camera->Position += camera->Front * velocity * DeltaTime;
	if (direction == CAM_LEFT    ) camera->Position -= camera->Right * velocity * DeltaTime;
	if (direction == CAM_RIGHT   ) camera->Position += camera->Right * velocity * DeltaTime;
	if (direction == CAM_BACKWARD) camera->Position -= camera->Front * velocity * DeltaTime;
}

/*------------------------------- Cursor -------------------------------*/

struct Cursor_3D
{
	vec3 Position;
	bool Connected;
};

void CursorInit(Cursor_3D* cursor)
{
	cursor->Position = vec3(5, 5, 5);
}

vec3 CursorGetSelectedTile(Cursor_3D* cursor)
{
	int x = (int)cursor->Position.x;
	int z = (int)cursor->Position.z;

	if (x > 8) x = 8;
	if (x < 0) x = 0;
	if (z > 8) z = 8;
	if (z < 0) z = 0;

	return vec3(x, 3, z);
}

/*------ Rendering ------*/

struct Cursor_Renderable
{
	vec3 Position;
	vec3 Color;
};

struct Cursor_Renderer
{
	Cursor_Renderable Renderable;
};

void CursorToRenderable(Cursor_3D* cursor, Cursor_Renderer* renderer)
{
	renderer->Renderable.Position = cursor->Position;
	if (cursor->Connected) renderer->Renderable.Color = vec3(1, 1, 1);
	else renderer->Renderable.Color = vec3(.8, .1, .1);
}

/*------------------------------- Cursor Shaders -------------------------------*/
const char* CursorVertSource = R"glsl(
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
		gl_Position = Proj * View * vec4(Position + (Vertex * vec3(.2,.2,.2)), 1);
    }
		)glsl";

const char* CursorFragSource = R"glsl(
    #version 330 core

	in vec3 color;

	out vec4 CursorColor;

    void main()
    {
		CursorColor = vec4(color,1);
    }
		)glsl";