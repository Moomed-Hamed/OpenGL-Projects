#pragma once
#include "Physics.h"

#define NUM_TILES_X 8
#define NUM_TILES_Z 8
#define NUM_TILES (NUM_TILES_X * NUM_TILES_Z)

#define NO_TILE 0
#define STONE_TILE 1
#define DIRT_TILE  2
#define GRASS_TILE 3
#define SAND_TILE  4
#define WATER_TILE 5
#define WOOD_TILE  6
#define CLAY_BRICK_TILE  7
#define STONE_BRICK_TILE 8

struct Tile
{
	uint Type;
};

struct Game_World
{
	Tile Tiles[NUM_TILES];
};

void GameWorldInit(Game_World* world, uint* tile_id)
{
	for (uint i = 0; i < NUM_TILES; ++i)
	{
		world->Tiles[i].Type = *(tile_id + i);
	}
}

/*------ Rendering ------*/

struct Solid_Renderable
{
	vec3 Position;
	vec2 TexOffset;
};

struct Fluid_Renderable
{
	vec3 Position;
};

struct World_Renderer
{
	Solid_Renderable SolidMem[64];
	Fluid_Renderable FluidMem[64];

	uint NumSolidBlocks;
	uint NumFluidBlocks;
};

void GameWorldToRenderable(Game_World* world, World_Renderer* renderer)
{
#define SOLID_TYPE 1
#define FLUID_TYPE 2

	Solid_Renderable* solidmem = renderer->SolidMem;
	Fluid_Renderable* fluidmem = renderer->FluidMem;

	renderer->NumSolidBlocks = 0;
	renderer->NumFluidBlocks = 0;

	for (uint x = 0; x < 8; ++x) {
		for (uint z = 0; z < 8; ++z)
		{
			Tile Tile = world->Tiles[x + (8 * z)];

			uint RenderType = 0; //Terrain, Fluid

			vec3 Position = vec3(x, 0, z);

			switch (Tile.Type)
			{
			case STONE_BRICK_TILE:
			case CLAY_BRICK_TILE:
			case WOOD_TILE:
			case SAND_TILE:
			case GRASS_TILE:
			case DIRT_TILE:
			case STONE_TILE: RenderType = SOLID_TYPE; break;

			case WATER_TILE: RenderType = FLUID_TYPE;  break;
			}

			if (RenderType == SOLID_TYPE)
			{
				switch (Tile.Type)
				{
				case STONE_TILE: solidmem->TexOffset = vec2(0.0, 0.0); break;
				case DIRT_TILE:  solidmem->TexOffset = vec2(.25, 0.0); break;
				case GRASS_TILE: solidmem->TexOffset = vec2(.50, 0.0); break;
				case SAND_TILE:  solidmem->TexOffset = vec2(.75, 0.0); break;
				
				case WOOD_TILE: solidmem->TexOffset = vec2(.25, -.25); break;
				case CLAY_BRICK_TILE:  solidmem->TexOffset = vec2(.50, -.25); break;
				case STONE_BRICK_TILE: solidmem->TexOffset = vec2(.75, -.25); break;

				default: solidmem->TexOffset = vec2(.75, -.75); //Texture Not Found
				}

				++renderer->NumSolidBlocks;
				solidmem->Position = Position;
				++solidmem;
			}
			else if (RenderType == FLUID_TYPE)
			{
				switch (Tile.Type)
				{
				case WATER_TILE: break;
				}

				++renderer->NumFluidBlocks;
				fluidmem->Position = Position;
				++fluidmem;
			}
		}
	}
#undef SOLID_TYPE
#undef FLUID_TYPE
}

/*------------------------------- Solid Shaders -------------------------------*/
const char* SolidVertSource = R"glsl(
    #version 330 core

	layout (location = 0) in vec3 BasePos;
	layout (location = 1) in vec2 BaseTex;
	layout (location = 2) in vec3 PosOffset;
	layout (location = 3) in vec2 TexOffset;

	uniform mat4 Proj;
	uniform mat4 View;
	out vec2 TexCoord;

    void main()
    {
		gl_Position = Proj * View * vec4( BasePos + PosOffset , 1.0);
		TexCoord.x = (TexOffset.x + BaseTex.x);
		TexCoord.y = (TexOffset.y + BaseTex.y);
    }
		)glsl";

const char* SolidFragSource = R"glsl(
    #version 330 core

	in vec2 TexCoord;

	uniform sampler2D TileTextures;

	out vec4 FragColor;

    void main()
    {
		FragColor = texture(TileTextures, TexCoord);
    }
		)glsl";

/*------------------------------- Water Shaders -------------------------------*/
const char* WaterVertSource = R"glsl(
    #version 330 core

	layout (location = 0) in vec3 Position;
	layout (location = 1) in vec3 Offset;

	uniform mat4 Proj;
	uniform mat4 View;
    uniform float Timer;

	out float Height;

    void main()
    {
		float x1 = (Position.x + 1) + Offset.x;
		float x2 = (Position.z + 1) + Offset.z;
		float factor = sqrt( (x1 * x1) + (x2 * x2) );

		vec3 FinalPos = Position + Offset;

		FinalPos.y += .05 * sin( (-Timer * .5 *  3.14159) + (3.1415 * factor) );
		FinalPos.y -= .1;

		Height = sin( (-Timer * 3.14159) + (3.1415 * factor) );

		gl_Position = Proj * View * vec4( FinalPos, 1);
	}
		)glsl";

const char* WaterFragSource = R"glsl(
    #version 330 core

	out vec4 FragColor;

	in float Height;

    void main()
    {
		float Might = Height * .06;
		FragColor = vec4(Might,Might,1,1);
    }
		)glsl";