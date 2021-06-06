#include "turrets.h"

#define MAX_BULLETS 256

#define BULLET_SMALL 1

struct Bullet
{
	uint type;
	vec3 position;
	vec3 velocity;
	float damage;
};

void spawn_bullet(Bullet* bullets, uint type, vec3 position, vec3 velocity)
{
	for (uint i = 0; i < MAX_TURRETS; i++)
	{
		if (bullets[i].type == NULL)
		{
			bullets[i].type = type;
			bullets[i].position = position;
			bullets[i].velocity = velocity;

			return;
		}
	}
}

void update_bullets(Bullet* bullets, float dtime)
{
	for (uint i = 0; i < MAX_TURRETS; i++)
	{
		if (bullets[i].type != NULL)
		{
			// do stuff
			bullets[i].position += bullets[i].velocity * dtime;
		}
	}
}

// rendering

struct Bullet_Drawable
{
	vec3 position;
	vec3 color;
};

struct Bullet_Renderer
{
	uint num_bullets;
	Bullet_Drawable bullets[MAX_TURRETS];

	Drawable_Mesh mesh;
	Shader shader;
};


void init(Bullet_Renderer* renderer)
{
	load(&renderer->mesh, "assets/meshes/ball.mesh", sizeof(renderer->bullets));
	mesh_add_attrib_vec3(2, sizeof(Bullet_Drawable), 0); // world pos
	mesh_add_attrib_vec3(3, sizeof(Bullet_Drawable), sizeof(vec3)); // color

	load(&(renderer->shader), "assets/shaders/cell.vert", "assets/shaders/cell.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
}
void update_renderer(Bullet_Renderer* renderer, Bullet* bullets)
{
	uint num_bullets = 0;
	Bullet_Drawable* memory = renderer->bullets;

	for (uint i = 0; i < MAX_BULLETS; i++)
	{
		if (bullets[i].type != NULL)
		{
			memory->position = bullets[i].position;
			memory->color = vec3(0, 0, 1);

			num_bullets++;
			memory++;
		}
	}

	renderer->num_bullets = num_bullets;
	update(renderer->mesh, num_bullets * sizeof(Bullet_Drawable), (byte*)renderer->bullets);
}