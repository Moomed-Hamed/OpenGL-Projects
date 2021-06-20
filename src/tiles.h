#include "renderer.h"

#define MAP_X 16
#define MAP_Z 16
#define NUM_MAP_TILES (MAP_X * MAP_Z)
#define TILE_INDEX(x,z) ((x) + (MAP_Z * (z)))

#define TILE_ROAD 1 // where enemies travel

typedef uint32 TileID;

// rendering

struct Tile_Drawable
{
	vec3 position;
	vec3 color;
};

struct Tile_Renderer
{
	uint num_tiles;
	Tile_Drawable tiles[NUM_MAP_TILES];

	Drawable_Mesh tile_mesh;
	Shader tile_shader;
};

void init(Tile_Renderer* renderer)
{
	load(&renderer->tile_mesh, "assets/meshes/tile.mesh", sizeof(renderer->tiles));
	mesh_add_attrib_vec3(2, sizeof(Tile_Drawable), 0); // world pos
	mesh_add_attrib_vec3(3, sizeof(Tile_Drawable), sizeof(vec3)); // color

	load(&(renderer->tile_shader), "assets/shaders/cell.vert", "assets/shaders/cell.frag");
	bind(renderer->tile_shader);
	set_int(renderer->tile_shader, "positions", 0);
	set_int(renderer->tile_shader, "normals"  , 1);
	set_int(renderer->tile_shader, "albedo"   , 2);
}
void update_renderer(Tile_Renderer* renderer, TileID* tiles)
{
	uint num_tiles= 0;
	Tile_Drawable* tile_mem = renderer->tiles;

	for (int x = 0; x < MAP_X; x++) {
	for (int z = 0; z < MAP_Z; z++)
	{
		tile_mem->position = vec3(x, 0, z);

		switch (tiles[TILE_INDEX(x,z)])
		{
		case TILE_ROAD: tile_mem->color = vec3(.8, .8, .8); break;
		default: tile_mem->color = vec3(.83, .69, .22);
		}

		num_tiles++;
		tile_mem++;
	} }

	renderer->num_tiles = num_tiles;
	update(renderer->tile_mesh, num_tiles * sizeof(Tile_Drawable), (byte*)renderer->tiles);
}