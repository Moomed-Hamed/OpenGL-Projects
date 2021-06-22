#include "bullets.h"

struct Level
{
	TileID tiles[NUM_MAP_TILES];
	Bullet bullets[MAX_BULLETS];
	Turret turrets[MAX_TURRETS];
	Enemy enemies[MAX_ENEMIES];
	vec3 path_nodes[16]; // path enemies travel on
};

void update_level(Level* level, float dtime)
{
	Bullet* bullets = level->bullets;
	Turret* turrets = level->turrets;
	Enemy*  enemies = level->enemies;

	// bullet collisions
	for (int i = 0; i < MAX_BULLETS; i++) {
	for (int j = 0; j < MAX_ENEMIES; j++)
	{
		if (glm::length(bullets[i].position - enemies[j].position) < 1.5) // if bullet is inside an enemy
		{
			enemies[j].health -= bullets[i].damage;
			bullets[i] = {}; // destroy the bullet
		}
	} }

	// enemy movement/deaths
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].type == NULL) continue;

		if (enemies[i].health < 0) // then this enemy has died
		{
			enemies[i] = {}; // destroy the enemy
			// drop some coins or something idk
		}
		else // enemy is still alive
		{
			vec3 next_node_pos = level->path_nodes[enemies[i].node_index];

			if (glm::length(next_node_pos - enemies[i].position) < .1)
			{
				if (enemies[i].node_index == 15 || level->path_nodes[enemies[i].node_index + 1].x < 0)
				{
					enemies[i] = {};
					// level takes damage?
					continue;
				}
				else // start moving to next node in sequence
				{
					enemies[i].node_index++;
					next_node_pos = level->path_nodes[enemies[i].node_index];
				}
			}

			vec3 move_dir = glm::normalize(next_node_pos - enemies[i].position);
			enemies[i].position += (move_dir * 3.5f) * dtime;
		}
	}

	// turret aiming & firing
	for (int i = 0; i < MAX_TURRETS; i++)
	{
		if (turrets[i].type == NULL) continue;

		for (int j = 0; j < MAX_ENEMIES; j++)
		{
			if (enemies[j].type != NULL)
			{
				vec3 next_node_pos = level->path_nodes[enemies[i].node_index];
				vec3 enemy_velocity_dir = glm::normalize(next_node_pos - enemies[i].position);
				vec3 predict_assist = enemy_velocity_dir * 4.f;
				vec3 bullet_dir = glm::normalize((enemies[j].position + predict_assist) - turrets[i].position);
				
				turrets[i].aim_direction = vec3(bullet_dir.x, 0, bullet_dir.z);

				if (turrets[i].cooldown > 0)
				{
					turrets[i].cooldown -= dtime;
				}
				else
				{
					spawn_bullet(bullets, turrets[i].position + vec3(0, 1, 0), bullet_dir * 10.f);
					turrets[i].cooldown = .5;
				}

				break;
			}
		}
	}

	update_bullets(bullets, dtime);
	update_enemies(enemies);
	update_turrets(turrets);
}
