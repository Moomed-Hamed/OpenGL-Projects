#include "enemies.h"

#define MAX_TURRETS 256

#define TURRET_SMALL 1

struct Turret
{
	uint type;
	vec3 position;
	vec3 aim_direction; // where the turret is currently aiming
	vec3 target_pos; // where the turret is trying to aim
	float cooldown; // seconds until turret can fire
};

void spawn_turret(Turret* turrets, vec3 pos, uint type = TURRET_SMALL)
{
	for (uint i = 0; i < MAX_TURRETS; i++)
	{
		if (turrets[i].type == NULL)
		{
			turrets[i].type = type;
			turrets[i].position = pos + vec3(0.5, 0, 0.5);
			turrets[i].aim_direction = glm::normalize(vec3(1, 0, 1));
			turrets[i].target_pos = {};
			turrets[i].cooldown = -1;

			return;
		}
	}
}

void update_turrets(Turret* turrets)
{
	for (uint i = 0; i < MAX_TURRETS; i++)
	{
		if (turrets[i].type != NULL)
		{
			// do stuff
		}
	}
}

// rendering

struct Turret_Drawable
{
	vec3 position;
	vec3 color;
	mat3 rotation;
};

struct Turret_Renderer
{
	uint num_turrets;
	Turret_Drawable turrets[MAX_TURRETS];

	Drawable_Mesh mesh;
	Shader shader;
};

void init(Turret_Renderer* renderer)
{
	load(&renderer->mesh, "assets/meshes/turret.mesh", sizeof(renderer->turrets));
	mesh_add_attrib_vec3(2, sizeof(Turret_Drawable), 0); // world pos
	mesh_add_attrib_vec3(3, sizeof(Turret_Drawable), sizeof(vec3)); // color
	mesh_add_attrib_mat3(4, sizeof(Turret_Drawable), sizeof(vec3) * 2); // rotation

	load(&(renderer->shader), "assets/shaders/rot.vert", "assets/shaders/cell.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
}
void update_renderer(Turret_Renderer* renderer, Turret* turrets)
{
	uint num_turrets = 0;
	Turret_Drawable* memory = renderer->turrets;

	for (uint i = 0; i < MAX_TURRETS; i++)
	{
		if (turrets[i].type != NULL)
		{
			memory->position = turrets[i].position;
			memory->color = vec3(.5, 1, 1);
			memory->rotation = point_at(turrets[i].aim_direction, vec3(0, 1, 0));

			num_turrets++;
			memory++;
		}
	}

	renderer->num_turrets = num_turrets;
	update(renderer->mesh, num_turrets * sizeof(Turret_Drawable), (byte*)renderer->turrets);
}