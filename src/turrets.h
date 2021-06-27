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

struct Turret_Cannon_Drawable
{
	vec3 position;
	vec3 color;
	mat3 rotation;
};

struct Turret_Platform_Drawable
{
	vec3 position;
	vec3 color;
};

struct Turret_Renderer
{
	uint num_turrets;
	Turret_Cannon_Drawable cannons[MAX_TURRETS];
	Turret_Platform_Drawable platforms[MAX_TURRETS];

	Drawable_Mesh cannon_mesh, platform_mesh;
	Shader cannon_shader, platform_shader;
};

void init(Turret_Renderer* renderer)
{
	load(&renderer->cannon_mesh, "assets/meshes/turret.mesh", sizeof(renderer->cannons));
	mesh_add_attrib_vec3(2, sizeof(Turret_Cannon_Drawable), 0); // world pos
	mesh_add_attrib_vec3(3, sizeof(Turret_Cannon_Drawable), sizeof(vec3)); // color
	mesh_add_attrib_mat3(4, sizeof(Turret_Cannon_Drawable), sizeof(vec3) * 2); // rotation

	load(&(renderer->cannon_shader), "assets/shaders/rot.vert", "assets/shaders/cell.frag");
	bind(renderer->cannon_shader);
	set_int(renderer->cannon_shader, "positions", 0);
	set_int(renderer->cannon_shader, "normals"  , 1);
	set_int(renderer->cannon_shader, "albedo"   , 2);

	load(&renderer->platform_mesh, "assets/meshes/turret_platform.mesh", sizeof(renderer->platforms));
	mesh_add_attrib_vec3(2, sizeof(Turret_Platform_Drawable), 0); // world pos
	mesh_add_attrib_vec3(3, sizeof(Turret_Platform_Drawable), sizeof(vec3)); // color

	load(&(renderer->platform_shader), "assets/shaders/cell.vert", "assets/shaders/cell.frag");
	bind(renderer->platform_shader);
	set_int(renderer->platform_shader, "positions", 0);
	set_int(renderer->platform_shader, "normals"  , 1);
	set_int(renderer->platform_shader, "albedo"   , 2);
}
void update_renderer(Turret_Renderer* renderer, Turret* turrets)
{
	uint num_turrets = 0;
	Turret_Cannon_Drawable*   cannon_memory   = renderer->cannons;
	Turret_Platform_Drawable* platform_memory = renderer->platforms;

	for (uint i = 0; i < MAX_TURRETS; i++)
	{
		if (turrets[i].type != NULL)
		{
			cannon_memory->position = turrets[i].position;
			cannon_memory->color = vec3(.5, 1, 1);
			cannon_memory->rotation = point_at(turrets[i].aim_direction, vec3(0, 1, 0));

			platform_memory->position = turrets[i].position;
			platform_memory->color = vec3(.5, 3, 1);

			num_turrets++;
			cannon_memory++;
			platform_memory++;
		}
	}

	renderer->num_turrets = num_turrets;
	update(renderer->cannon_mesh  , num_turrets * sizeof(Turret_Cannon_Drawable)  , (byte*)renderer->cannons);
	update(renderer->platform_mesh, num_turrets * sizeof(Turret_Platform_Drawable), (byte*)renderer->platforms);
}