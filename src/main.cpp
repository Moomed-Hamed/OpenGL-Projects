#include "level.h"

#define TARGET_FRAMES_PER_SECOND ((float)120)
#define DRAW_DISTANCE 512.0f
#define FOV ToRadians(45.0f)

int main()
{
	Window   window = {};
	Mouse    mouse  = {};
	Keyboard keys   = {};

	init_window(&window, 1920, 1080, "tower defense game");
	init_keyboard(&keys);

	Camera camera = { vec3(5,5,5) };

	Level* level = Alloc(Level, 1);
	level->path_nodes[0] = { vec3(0,1,0) };

	Tile_Renderer*   tile_renderer   = Alloc(Tile_Renderer  , 1);
	Enemy_Renderer*  enemy_renderer  = Alloc(Enemy_Renderer , 1);
	Turret_Renderer* turret_renderer = Alloc(Turret_Renderer, 1);
	Bullet_Renderer* bullet_renderer = Alloc(Bullet_Renderer, 1);

	init(tile_renderer);
	init(enemy_renderer);
	init(turret_renderer);
	init(bullet_renderer);

	G_Buffer g_buffer = {};
	init_g_buffer(&g_buffer, window);
	Shader lighting_shader = make_lighting_shader();
	mat4 proj = glm::perspective(FOV, (float)window.screen_width / window.screen_height, 0.1f, DRAW_DISTANCE);

	// frame timer
	float frame_time = 1.f / 60;
	int64 target_frame_milliseconds = frame_time * 1000.f;
	Timestamp frame_start = get_timestamp(), frame_end;

	while (1)
	{
		update_window(window);
		update_mouse(&mouse, window);
		update_keyboard(&keys, window);

		if (keys.ESC.is_pressed) break;

		camera_update_dir(&camera, mouse.dx, mouse.dy);
		if (keys.W.is_pressed) camera_update_pos(&camera, DIR_FORWARD , 8 * frame_time);
		if (keys.A.is_pressed) camera_update_pos(&camera, DIR_BACKWARD, 8 * frame_time);
		if (keys.S.is_pressed) camera_update_pos(&camera, DIR_LEFT    , 8 * frame_time);
		if (keys.D.is_pressed) camera_update_pos(&camera, DIR_RIGHT   , 8 * frame_time);

		if (mouse.left_button.is_pressed && !mouse.left_button.was_pressed) {}
		if (mouse.right_button.is_pressed && !mouse.right_button.was_pressed){}

		if (keys.E.is_pressed && !keys.E.was_pressed)
		{
			//spawn_bullet(level->bullets, BULLET_SMALL, vec3(0, 2, 0), vec3(1, 0, 1));
			spawn_turret(level->turrets, TURRET_SMALL, vec3(1, 1, 1));
			spawn_enemy(level->enemies, 1, vec3(5, 1, 5));
		}

		if (keys.F.is_pressed) // flashlight
		{
			bind(lighting_shader);
			set_vec3(lighting_shader, "spt_light.position" , camera.position);
			set_vec3(lighting_shader, "spt_light.direction", camera.front);
		}
		else if (keys.X.is_pressed)
		{
			bind(lighting_shader);
			set_vec3(lighting_shader, "spt_light.position" , vec3(0, 0, 0));
			set_vec3(lighting_shader, "spt_light.direction", vec3(0, -1, 0));
		}

		// game state updates //
		update_level(level, frame_time);

		// rendering updates //
		update_renderer(tile_renderer  , level->tiles);
		update_renderer(enemy_renderer , level->enemies);
		update_renderer(turret_renderer, level->turrets);
		update_renderer(bullet_renderer, level->bullets);

		mat4 proj_view = proj * glm::lookAt(camera.position, camera.position + camera.front, camera.up);

		// Geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bind(tile_renderer->tile_shader);
		set_mat4(tile_renderer->tile_shader, "proj_view", proj_view);
		draw(tile_renderer->tile_mesh, tile_renderer->num_tiles);

		bind(enemy_renderer->shader);
		set_mat4(enemy_renderer->shader, "proj_view", proj_view);
		draw(enemy_renderer->mesh, enemy_renderer->num_enemies);

		bind(turret_renderer->shader);
		set_mat4(turret_renderer->shader, "proj_view", proj_view);
		draw(turret_renderer->mesh, turret_renderer->num_turrets);

		bind(bullet_renderer->shader);
		set_mat4(bullet_renderer->shader, "proj_view", proj_view);
		draw(bullet_renderer->mesh, bullet_renderer->num_bullets);

		// Lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bind(lighting_shader);
		set_vec3(lighting_shader, "view_pos", camera.position);
		draw_g_buffer(g_buffer);

		//Frame Time
		frame_end = get_timestamp();
		int64 milliseconds_elapsed = calculate_milliseconds_elapsed(frame_start, frame_end);

		//print("frame time: %02d ms | fps: %06f\n", milliseconds_elapsed, 1000.f / milliseconds_elapsed);
		if (target_frame_milliseconds > milliseconds_elapsed) // frame finished early
		{
			os_sleep(target_frame_milliseconds - milliseconds_elapsed);
		}
		
		frame_start = frame_end;
	}

	shutdown_window();
	return 0;
}