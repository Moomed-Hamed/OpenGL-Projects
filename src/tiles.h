#include "camera.h"

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
	vec2 tex_offset;
};

struct Tile_Renderer
{
	uint num_tiles;
	Tile_Drawable tiles[NUM_MAP_TILES];

	Drawable_Mesh_UV tile_mesh;
	Shader tile_shader;
};

vec2 tile_get_tex_offset(uint tile)
{
	switch (tile)
	{
	case 0: return vec2(.50, 0.00);
	case TILE_ROAD: return vec2(0.0, 0.00);
	case 2: return vec2(.25, 0.00);
	case 3: return vec2(.50, 0.00);
	case 4: return vec2(.75, 0.00);
	case 5: return vec2(0.0, -.25);
	case 6: return vec2(.25, -.25);
	case 7: return vec2(.50, -.25);
	case 8: return vec2(.75, -.25);
	case 9: return vec2(0.0, -.50);
	//case 10: return vec2(.25, -.50);
	//case 11: return vec2(.50, -.50);
	//case 12: return vec2(.75, -.50);
	//case 13: return vec2(0.0, -.75);
	//case 14: return vec2(.25, -.75);
	//case 15: return vec2(.50, -.75);
	default: return vec2(.75, -.75); // invalid block
	}
}

void init(Tile_Renderer* renderer)
{
	load(&renderer->tile_mesh, "assets/meshes/tile.mesh_uv", "assets/textures/texture_atlas.bmp", sizeof(renderer->tiles));
	mesh_add_attrib_vec3(3, sizeof(Tile_Drawable), 0); // world pos
	mesh_add_attrib_vec2(4, sizeof(Tile_Drawable), sizeof(vec3)); // tex coord

	load(&(renderer->tile_shader), "assets/shaders/tile.vert", "assets/shaders/tile.frag");
	bind(renderer->tile_shader);
	set_int(renderer->tile_shader, "positions", 0);
	set_int(renderer->tile_shader, "normals"  , 1);
	set_int(renderer->tile_shader, "albedo"   , 2);
	set_int(renderer->tile_shader, "texture_sampler", 3);
}
void update_renderer(Tile_Renderer* renderer, TileID* tiles)
{
	uint num_tiles= 0;
	Tile_Drawable* tile_mem = renderer->tiles;

	for (int x = 0; x < MAP_X; x++) {
	for (int z = 0; z < MAP_Z; z++)
	{
		tile_mem->position = vec3(x, 0, z);
		tile_mem->tex_offset = tile_get_tex_offset(tiles[TILE_INDEX(x, z)]);

		num_tiles++;
		tile_mem++;
	} }

	renderer->num_tiles = num_tiles;
	update(renderer->tile_mesh, num_tiles * sizeof(Tile_Drawable), (byte*)renderer->tiles);
}