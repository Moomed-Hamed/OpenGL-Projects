#include "tiles.h"

#define MAX_ENEMIES 256

#define ENEMY_GRUNT 1
#define ENEMY_HEAVY 2

struct Enemy
{
	uint type;
	vec3 position;

	float health; // shield?
	int node_index; // index of path node this enemy is travelling to
};

void spawn_enemy(Enemy* enemies, vec3 pos, uint type = ENEMY_GRUNT)
{
	for (uint i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].type == NULL)
		{
			enemies[i].type = type;
			enemies[i].position = pos + vec3(0.5, 0, 0.5);

			switch (type)
			{
			case ENEMY_GRUNT: enemies[i].health = 100; break;
			case ENEMY_HEAVY: enemies[i].health = 200; break;
			}

			return;
		}
	}
}

void update_enemies(Enemy* enemies)
{
	for (uint i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].type != NULL)
		{
			// do stuff
		}
	}
}

// rendering

struct Enemy_Drawable
{
	vec3 position;
};

struct Enemy_Renderer
{
	uint num_enemies;
	Enemy_Drawable enemies[MAX_ENEMIES];

	Drawable_Mesh_UV mesh;
	Shader shader;
};

void init(Enemy_Renderer* renderer)
{
	load(&renderer->mesh, "assets/meshes/enemy.mesh_uv", "assets/textures/pallete.bmp", sizeof(renderer->enemies));
	mesh_add_attrib_vec3(3, sizeof(Enemy_Drawable), 0); // world pos

	load(&(renderer->shader), "assets/shaders/enemy.vert", "assets/shaders/tile.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
	set_int(renderer->shader, "texture_sampler", 4);
}
void update_renderer(Enemy_Renderer* renderer, Enemy* enemies)
{
	uint num_enemies = 0;
	Enemy_Drawable* enemy_mem = renderer->enemies;

	for (uint i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].type != NULL)
		{
			enemy_mem->position = enemies[i].position;

			num_enemies++;
			enemy_mem++;
		}
	}

	renderer->num_enemies = num_enemies;
	update(renderer->mesh, num_enemies * sizeof(Enemy_Drawable), (byte*)renderer->enemies);
}