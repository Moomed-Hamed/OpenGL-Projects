/* Author: Mohamed Hamed */

#include <iostream>

#include "Memory.h"

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

int main()
{
	/*---------------- Aquiring Game Memory ----------------*/

	uint GameMemorySize = MEM_PAGE_SIZE * TOTAL_MEM_PAGES; // out(GameMemorySize << " Bytes"); stop;
	void* GameMemory = VirtualAlloc(0, GameMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); //ask 4 sum memory

	Game_Memory MemoryManager;
	GameMemoryInit(&MemoryManager, (Mem_Page*)GameMemory);

	/*---------------- initializing Leap Motion ----------------*/

	Controller LeapController;
	SampleListener LeapListener;
	LeapController.addListener(LeapListener);

	Frame LeapFrame;

	/*---------------- initializing GLEW & GLFW ----------------*/

	if (!glfwInit()) return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* WindowInstance = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "HCI Final Project", NULL, NULL);
	if (!WindowInstance) { glfwTerminate(); return 1; }

	glfwMakeContextCurrent(WindowInstance);
	glfwSwapInterval(1);

	//Capture the cursor
	glfwSetInputMode(WindowInstance, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	/*------------------------------- Prepare for game loop -------------------------------*/
#define FOV ToRadians(45.0f)
#define DRAW_DISTANCE 256.0f

	mat4 Proj = glm::perspective(FOV, 1920.0f / 1080.0f, 0.1f, DRAW_DISTANCE);
	mat4 Model = mat4(1.0f);
	mat4 View = mat4(1.0f);

	glEnable(GL_DEPTH_TEST);

	/*-------------- Solid Setup -----------*/

	float solid_vertices[]{
		// X     Y      Z     U     V
		0.0f,  1.0f,  1.0f, 0.0f, 1.0f, // Left  - Top - Near - 0
		0.0f,  1.0f,  0.0f, .25f, 1.0f, // Left  - Top - Far  - 1
		1.0f,  1.0f,  0.0f, 0.0f, 1.0f, // Right - Top - Far  - 2
		1.0f,  1.0f,  1.0f, .25f, 1.0f, // Right - Top - Near - 3

		0.0f,  0.0f,  1.0f, 0.0f, .75f, // Left  - Bottom - Near - 4
		0.0f,  0.0f,  0.0f, .25f, .75f, // Left  - Bottom - Far  - 5
		1.0f,  0.0f,  0.0f, 0.0f, .75f, // Right - Bottom - Far  - 6
		1.0f,  0.0f,  1.0f, .25f, .75f, // Right - Bottom - Near - 7

		//Duplicates for textures
		0.0f,  1.0f,  1.0f, .25f, .75f, // Left  - Top - Near - 8
		1.0f,  1.0f,  1.0f, 0.0f, .75f, // Right - Top - Near - 9

		1.0f,  0.0f,  0.0f, .25f, 1.0f, // Right - Bottom - Far  - 10
		0.0f,  0.0f,  0.0f, 0.0f, 1.0f, // Left  - Bottom - Far  - 11
	};

	//Element Buffer (Counter-Clockwise)
	GLuint solid_elements[]{
		4, 7, 3, //Near 1
		3, 0, 4, //Near 2

		5, 6, 2, //Far 1
		2, 1, 5, //Far 2

		8, 9, 2, //Top 1
		2, 1, 8, //Top 2

		4, 7, 10, //Bottom 1
	   10, 11, 4, //Bottom 2

		4, 0, 1, //Left 1
		1, 5, 4, //Left 2

		7, 6, 2, //Right 1
		2, 3, 7  //Right 2
	};

	GLuint SOLID_VAO;
	glGenVertexArrays(1, &SOLID_VAO);
	glBindVertexArray(SOLID_VAO);

#define SOLID_RENDER_MEM_SIZE sizeof(solid_vertices) + (sizeof(Solid_Renderable) * NUM_TILES)
	GLuint Solid_VBO;
	glGenBuffers(1, &Solid_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Solid_VBO);
	glBufferData(GL_ARRAY_BUFFER, SOLID_RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(solid_vertices), solid_vertices);

	GLuint Solid_EBO;
	glGenBuffers(1, &Solid_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Solid_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(solid_elements), solid_elements, GL_STATIC_DRAW);

	{
		GLint PosAttrib = 0;
		glVertexAttribPointer(PosAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(PosAttrib);

		GLint WorldTexAttrib = 1;
		glEnableVertexAttribArray(WorldTexAttrib);
		glVertexAttribPointer(WorldTexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		GLint PosOffsetAttrib = 2;
		glVertexAttribPointer(PosOffsetAttrib, 3, GL_FLOAT, GL_FALSE, (sizeof(Solid_Renderable)), (void*)(sizeof(solid_vertices)));
		glVertexAttribDivisor(PosOffsetAttrib, 1);
		glEnableVertexAttribArray(PosOffsetAttrib);

		GLint TexOffsetAttrib = 3;
		glVertexAttribPointer(TexOffsetAttrib, 2, GL_FLOAT, GL_FALSE, (sizeof(Solid_Renderable)), (void*)(sizeof(solid_vertices) + sizeof(vec3)));
		glVertexAttribDivisor(TexOffsetAttrib, 1);
		glEnableVertexAttribArray(TexOffsetAttrib);
	}

	GLuint SolidVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(SolidVertShader, 1, &SolidVertSource, NULL);//
	glCompileShader(SolidVertShader);

	GLuint SolidFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(SolidFragShader, 1, &SolidFragSource, NULL);//
	glCompileShader(SolidFragShader);

	GLuint SolidShaderProgram = glCreateProgram();
	glAttachShader(SolidShaderProgram, SolidVertShader);
	glAttachShader(SolidShaderProgram, SolidFragShader);
	glBindFragDataLocation(SolidShaderProgram, 0, "FragColor");
	glLinkProgram(SolidShaderProgram);
	glUseProgram(SolidShaderProgram);

	GLint SolidProjMatrix = glGetUniformLocation(SolidShaderProgram, "Proj");
	GLint SolidViewMatrix = glGetUniformLocation(SolidShaderProgram, "View");
	GLint SolidTexSampler = glGetUniformLocation(SolidShaderProgram, "TileTextures");

	glUniformMatrix4fv(SolidProjMatrix, 1, GL_FALSE, glm::value_ptr(Proj));
	glUniformMatrix4fv(SolidViewMatrix, 1, GL_FALSE, glm::value_ptr(View));
	glUniform1i(SolidTexSampler, 1);
	/*-------------- Fluid Setup -----------*/

	float fluid_vertices[]{
		// X     Y      Z
		0.0f,  1.0f,  0.2f, // Left  - Near - 0
		0.2f,  1.0f,  0.2f, // Right - Near - 1
		0.2f,  1.0f,  0.0f, // Right - Far  - 2
		0.0f,  1.0f,  0.0f, // Left  - Far  - 3

		0.0f,  1.0f,  0.4f, // Left  - Far  - 4
		0.2f,  1.0f,  0.4f, // Right - Far  - 5

		0.0f,  1.0f,  0.6f, // Left  - Far  - 6
		0.2f,  1.0f,  0.6f, // Right - Far  - 7

		0.0f,  1.0f,  0.8f, // Left  - Far  - 8
		0.2f,  1.0f,  0.8f, // Right - Far  - 9

		0.0f,  1.0f,  1.0f, // Left  - Far  - 10
		0.2f,  1.0f,  1.0f, // Right - Far  - 11

	  //Column 2 --------------------------------
		0.4f,  1.0f,  0.2f, // Right - Near - 12
		0.4f,  1.0f,  0.0f, // Right - Far  - 13

		0.4f,  1.0f,  0.4f, // Right - Far  - 14

		0.4f,  1.0f,  0.6f, // Right - Far  - 15

		0.4f,  1.0f,  0.8f, // Right - Far  - 16

		0.4f,  1.0f,  1.0f, // Right - Far  - 17

	  //Column 3 --------------------------------
		0.6f,  1.0f,  0.2f, // Right - Near - 18
		0.6f,  1.0f,  0.0f, // Right - Far  - 19

		0.6f,  1.0f,  0.4f, // Right - Far  - 20

		0.6f,  1.0f,  0.6f, // Right - Far  - 21

		0.6f,  1.0f,  0.8f, // Right - Far  - 22

		0.6f,  1.0f,  1.0f, // Right - Far  - 23

	  //Column 4 --------------------------------
		0.8f,  1.0f,  0.2f, // Right - Near - 24
		0.8f,  1.0f,  0.0f, // Right - Far  - 25

		0.8f,  1.0f,  0.4f, // Right - Far  - 26

		0.8f,  1.0f,  0.6f, // Right - Far  - 27

		0.8f,  1.0f,  0.8f, // Right - Far  - 28

		0.8f,  1.0f,  1.0f, // Right - Far  - 29

	  //Column 5 --------------------------------
		1.0f,  1.0f,  0.2f, // Right - Near - 30
		1.0f,  1.0f,  0.0f, // Right - Far  - 31

		1.0f,  1.0f,  0.4f, // Right - Far  - 32

		1.0f,  1.0f,  0.6f, // Right - Far  - 33

		1.0f,  1.0f,  0.8f, // Right - Far  - 34

		1.0f,  1.0f,  1.0f, // Right - Far  - 35
	};

	//Element Buffer (Counter-Clockwise)
	GLuint fluid_elements[]{
	/* --- column 1 --- */
		0, 1, 2,
		2, 3, 0,
		
		4, 5, 1,
		1, 0, 4,
		
		6, 7, 5,
		5, 4, 6,
		
		8, 9, 7,
		7, 6, 8,
		
		10,11,9,
		9, 8,10,

	/* --- column 2 --- */
		1,12,13,
		13,2, 1,
		
		5,14,12,
		12, 1, 5,
		
		7,15,14,
		14, 5,7,
		
		9,16,15,
		15, 7, 9,
		
		11,17,16,
		16, 9,11,

	/* --- column 3 --- */
		12,18,19,
		19,13,12,
		
		14,20,18,
		18,12,14,
		
		15,21,20,
		20,14,15,
		
		16,22,21,
		21,15,16,
		
		17,23,22,
		22,16,17,

	/* --- column 4 --- */
		18,24,25,
		25,19,18,
		
		20,26,24,
		24,18,20,
		
		21,27,26,
		26,20,21,
		
		22,28,27,
		27,21,22,
		
		23,29,28,
		28,22,23,

	/* --- column 5 --- */
		24,30,31,
		31,25,24,
		
		26,32,30,
		30,24,26,
		
		27,33,32,
		32,26,27,
		
		28,34,33,
		33,27,28,
		
		29,35,34,
		34,28,29,
	};

	GLuint FLUID_VAO;
	glGenVertexArrays(1, &FLUID_VAO);
	glBindVertexArray(FLUID_VAO);

#define FLUID_RENDER_MEM_SIZE sizeof(fluid_vertices) + (sizeof(Fluid_Renderable) * NUM_TILES)
	GLuint Fluid_VBO;
	glGenBuffers(1, &Fluid_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Fluid_VBO);
	glBufferData(GL_ARRAY_BUFFER, FLUID_RENDER_MEM_SIZE, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fluid_vertices), fluid_vertices);

	GLuint Fluid_EBO;
	glGenBuffers(1, &Fluid_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Fluid_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fluid_elements), fluid_elements, GL_STATIC_DRAW);

	{
		GLint PosAttrib = 0;
		glVertexAttribPointer(PosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), NULL);
		glEnableVertexAttribArray(PosAttrib);

		GLint OffsetAttrib = 1;
		glVertexAttribPointer(OffsetAttrib, 3, GL_FLOAT, GL_FALSE, NULL, (void*)(sizeof(fluid_vertices)));
		glVertexAttribDivisor(OffsetAttrib, 1);
		glEnableVertexAttribArray(OffsetAttrib);
	}

	GLuint FluidVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(FluidVertShader, 1, &WaterVertSource, NULL);
	glCompileShader(FluidVertShader);

	GLuint FluidFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FluidFragShader, 1, &WaterFragSource, NULL);
	glCompileShader(FluidFragShader);

	GLuint FluidShaderProgram = glCreateProgram();
	glAttachShader(FluidShaderProgram, FluidVertShader);
	glAttachShader(FluidShaderProgram, FluidFragShader);
	glBindFragDataLocation(FluidShaderProgram, 0, "FragColor");
	glLinkProgram(FluidShaderProgram);
	glUseProgram(FluidShaderProgram);

	GLint FluidProjMatrix = glGetUniformLocation(FluidShaderProgram, "Proj");
	GLint FluidViewMatrix = glGetUniformLocation(FluidShaderProgram, "View");
	GLint FluidTimer = glGetUniformLocation(FluidShaderProgram, "Timer");

	glUniformMatrix4fv(FluidProjMatrix, 1, GL_FALSE, glm::value_ptr(Proj));
	glUniformMatrix4fv(FluidViewMatrix, 1, GL_FALSE, glm::value_ptr(View));
	/*-------------- Bullet Setup ----------*/

	float bullet_vertices[]{
		// X     Y      Z  
		0.0f,  1.0f,  1.0f, // Left  - Top - Near - 0
		0.0f,  1.0f,  0.0f, // Left  - Top - Far  - 1
		1.0f,  1.0f,  0.0f, // Right - Top - Far  - 2
		1.0f,  1.0f,  1.0f, // Right - Top - Near - 3

		0.0f,  0.0f,  1.0f, // Left  - Bottom - Near - 4
		0.0f,  0.0f,  0.0f, // Left  - Bottom - Far  - 5
		1.0f,  0.0f,  0.0f, // Right - Bottom - Far  - 6
		1.0f,  0.0f,  1.0f, // Right - Bottom - Near - 7
	};

	//Element Buffer (Counter-Clockwise)
	GLuint bullet_elements[]{
		4, 7, 3, //Near 1
		3, 0, 4, //Near 2

		5, 6, 2, //Far 1
		2, 1, 5, //Far 2

		0, 3, 2, //Top 1
		2, 1, 0, //Top 2

		4, 7, 6, //Bottom 1
		6, 5, 4, //Bottom 2

		4, 0, 1, //Left 1
		1, 5, 4, //Left 2

		7, 6, 2, //Right 1
		2, 3, 7  //Right 2
	};

	GLuint BULLET_VAO;
	glGenVertexArrays(1, &BULLET_VAO);
	glBindVertexArray(BULLET_VAO);

#define BULLET_RENDER_MEM_SIZE sizeof(bullet_vertices) + ( sizeof(Bullet_Renderable) * NUM_BULLETS )
	GLuint Bullet_VBO;
	glGenBuffers(1, &Bullet_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Bullet_VBO);
	glBufferData(GL_ARRAY_BUFFER, BULLET_RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bullet_vertices), bullet_vertices);

	GLuint Bullet_EBO;
	glGenBuffers(1, &Bullet_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Bullet_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bullet_elements), bullet_elements, GL_STATIC_DRAW);

	{
		GLint VertAttrib = 0;
		glVertexAttribPointer(VertAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(VertAttrib);

		GLint PosAttrib = 1;
		glVertexAttribPointer(PosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Bullet_Renderable), (void*)(sizeof(bullet_vertices)));
		glVertexAttribDivisor(PosAttrib, 1);
		glEnableVertexAttribArray(PosAttrib);

		GLint ColorAttrib = 2;
		glVertexAttribPointer(ColorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Bullet_Renderable), (void*)(sizeof(bullet_vertices) + sizeof(vec3)));
		glVertexAttribDivisor(ColorAttrib, 1);
		glEnableVertexAttribArray(ColorAttrib);
	}

	GLuint BulletVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(BulletVertShader, 1, &BulletVertSource, NULL);
	glCompileShader(BulletVertShader);

	GLuint BulletFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(BulletFragShader, 1, &BulletFragSource, NULL);
	glCompileShader(BulletFragShader);

	GLuint BulletShaderProgram = glCreateProgram();
	glAttachShader(BulletShaderProgram, BulletVertShader);
	glAttachShader(BulletShaderProgram, BulletFragShader);
	glBindFragDataLocation(BulletShaderProgram, 0, "BulletColor");
	glLinkProgram(BulletShaderProgram);
	glUseProgram(BulletShaderProgram);

	GLint BulletProjMatrix = glGetUniformLocation(BulletShaderProgram, "Proj");
	GLint BulletViewMatrix = glGetUniformLocation(BulletShaderProgram, "View");

	glUniformMatrix4fv(BulletProjMatrix, 1, GL_FALSE, glm::value_ptr(Proj));
	glUniformMatrix4fv(BulletViewMatrix, 1, GL_FALSE, glm::value_ptr(View));
	/*-------------- Turret Setup ----------*/

	float turret_vertices[]{
		// X     Y      Z  
		0.0f,  1.0f,  1.0f, // Left  - Top - Near - 0
		0.0f,  1.0f,  0.0f, // Left  - Top - Far  - 1
		1.0f,  1.0f,  0.0f, // Right - Top - Far  - 2
		1.0f,  1.0f,  1.0f, // Right - Top - Near - 3

		0.0f,  0.0f,  1.0f, // Left  - Bottom - Near - 4
		0.0f,  0.0f,  0.0f, // Left  - Bottom - Far  - 5
		1.0f,  0.0f,  0.0f, // Right - Bottom - Far  - 6
		1.0f,  0.0f,  1.0f, // Right - Bottom - Near - 7
	};

	//Element Buffer (Counter-Clockwise)
	GLuint turret_elements[]{
		4, 7, 3, //Near 1
		3, 0, 4, //Near 2

		5, 6, 2, //Far 1
		2, 1, 5, //Far 2

		0, 3, 2, //Top 1
		2, 1, 0, //Top 2

		4, 7, 6, //Bottom 1
		6, 5, 4, //Bottom 2

		4, 0, 1, //Left 1
		1, 5, 4, //Left 2

		7, 6, 2, //Right 1
		2, 3, 7  //Right 2
	};

	GLuint TURRET_VAO;
	glGenVertexArrays(1, &TURRET_VAO);
	glBindVertexArray(TURRET_VAO);

#define TURRET_RENDER_MEM_SIZE sizeof(turret_vertices) + ( sizeof(Turret_Renderable) * NUM_TURRETS )
	GLuint Turret_VBO;
	glGenBuffers(1, &Turret_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Turret_VBO);
	glBufferData(GL_ARRAY_BUFFER, TURRET_RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(turret_vertices), turret_vertices);

	GLuint Turret_EBO;
	glGenBuffers(1, &Turret_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Turret_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(turret_elements), turret_elements, GL_STATIC_DRAW);

	{
		GLint VertAttrib = 0;
		glVertexAttribPointer(VertAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(VertAttrib);

		GLint PosAttrib = 1;
		glVertexAttribPointer(PosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Turret_Renderable), (void*)(sizeof(turret_vertices)));
		glVertexAttribDivisor(PosAttrib, 1);
		glEnableVertexAttribArray(PosAttrib);
	}

	GLuint TurretVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(TurretVertShader, 1, &TurretVertSource, NULL);
	glCompileShader(TurretVertShader);

	GLuint TurretFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(TurretFragShader, 1, &TurretFragSource, NULL);
	glCompileShader(TurretFragShader);

	GLuint TurretShaderProgram = glCreateProgram();
	glAttachShader(TurretShaderProgram, TurretVertShader);
	glAttachShader(TurretShaderProgram, TurretFragShader);
	glBindFragDataLocation(TurretShaderProgram, 0, "TurretColor");
	glLinkProgram(TurretShaderProgram);
	glUseProgram(TurretShaderProgram);

	GLint TurretProjMatrix = glGetUniformLocation(TurretShaderProgram, "Proj");
	GLint TurretViewMatrix = glGetUniformLocation(TurretShaderProgram, "View");

	glUniformMatrix4fv(TurretProjMatrix, 1, GL_FALSE, glm::value_ptr(Proj));
	glUniformMatrix4fv(TurretViewMatrix, 1, GL_FALSE, glm::value_ptr(View));
	/*-------------- Enemy Setup -----------*/

	float enemy_vertices[]{
		// X     Y      Z  
		-.5f,  1.0f,  0.5f, // Left  - Top - Near - 0
		-.5f,  1.0f,  -.5f, // Left  - Top - Far  - 1
		0.5f,  1.0f,  -.5f, // Right - Top - Far  - 2
		0.5f,  1.0f,  0.5f, // Right - Top - Near - 3

		-.5f,  0.0f,  0.5f, // Left  - Bottom - Near - 4
		-.5f,  0.0f,  -.5f, // Left  - Bottom - Far  - 5
		0.5f,  0.0f,  -.5f, // Right - Bottom - Far  - 6
		0.5f,  0.0f,  0.5f, // Right - Bottom - Near - 7
	};

	//Element Buffer (Counter-Clockwise)
	GLuint enemy_elements[]{
		4, 7, 3, //Near 1
		3, 0, 4, //Near 2

		5, 6, 2, //Far 1
		2, 1, 5, //Far 2

		0, 3, 2, //Top 1
		2, 1, 0, //Top 2

		4, 7, 6, //Bottom 1
		6, 5, 4, //Bottom 2

		4, 0, 1, //Left 1
		1, 5, 4, //Left 2

		7, 6, 2, //Right 1
		2, 3, 7  //Right 2
	};

	GLuint ENEMY_VAO;
	glGenVertexArrays(1, &ENEMY_VAO);
	glBindVertexArray(ENEMY_VAO);

#define ENEMY_RENDER_MEM_SIZE sizeof(enemy_vertices) + ( sizeof(Enemy_Renderable) * NUM_ENEMIES )
	GLuint Enemy_VBO;
	glGenBuffers(1, &Enemy_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Enemy_VBO);
	glBufferData(GL_ARRAY_BUFFER, ENEMY_RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(enemy_vertices), enemy_vertices);

	GLuint Enemy_EBO;
	glGenBuffers(1, &Enemy_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Enemy_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(enemy_elements), enemy_elements, GL_STATIC_DRAW);

	{
		GLint VertAttrib = 0;
		glVertexAttribPointer(VertAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(VertAttrib);

		GLint PosAttrib = 1;
		glVertexAttribPointer(PosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Enemy_Renderable), (void*)(sizeof(enemy_vertices)));
		glVertexAttribDivisor(PosAttrib, 1);
		glEnableVertexAttribArray(PosAttrib);

		GLint ScaleAttrib = 2;
		glVertexAttribPointer(ScaleAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Enemy_Renderable), (void*)(sizeof(enemy_vertices) + offsetof(Enemy_Renderable, Scale)));
		glVertexAttribDivisor(ScaleAttrib, 1);
		glEnableVertexAttribArray(ScaleAttrib);

		GLint ColorAttrib = 3;
		glVertexAttribPointer(ColorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Enemy_Renderable), (void*)(sizeof(enemy_vertices) + offsetof(Enemy_Renderable, Color)));
		glVertexAttribDivisor(ColorAttrib, 1);
		glEnableVertexAttribArray(ColorAttrib);
	}

	GLuint EnemyVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(EnemyVertShader, 1, &EnemyVertSource, NULL);
	glCompileShader(EnemyVertShader);

	GLuint EnemyFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(EnemyFragShader, 1, &EnemyFragSource, NULL);
	glCompileShader(EnemyFragShader);

	GLuint EnemyShaderProgram = glCreateProgram();
	glAttachShader(EnemyShaderProgram, EnemyVertShader);
	glAttachShader(EnemyShaderProgram, EnemyFragShader);
	glBindFragDataLocation(EnemyShaderProgram, 0, "EnemyColor");
	glLinkProgram(EnemyShaderProgram);
	glUseProgram(EnemyShaderProgram);

	GLint EnemyProjMatrix = glGetUniformLocation(EnemyShaderProgram, "Proj");
	GLint EnemyViewMatrix = glGetUniformLocation(EnemyShaderProgram, "View");

	glUniformMatrix4fv(EnemyProjMatrix, 1, GL_FALSE, glm::value_ptr(Proj));
	glUniformMatrix4fv(EnemyViewMatrix, 1, GL_FALSE, glm::value_ptr(View));
	/*-------------- Cursor Setup ----------*/

	float cursor_vertices[]{
		// X     Y      Z  
		0.0f,  1.0f,  1.0f, // Left  - Top - Near - 0
		0.0f,  1.0f,  0.0f, // Left  - Top - Far  - 1
		1.0f,  1.0f,  0.0f, // Right - Top - Far  - 2
		1.0f,  1.0f,  1.0f, // Right - Top - Near - 3

		0.0f,  0.0f,  1.0f, // Left  - Bottom - Near - 4
		0.0f,  0.0f,  0.0f, // Left  - Bottom - Far  - 5
		1.0f,  0.0f,  0.0f, // Right - Bottom - Far  - 6
		1.0f,  0.0f,  1.0f, // Right - Bottom - Near - 7
	};

	//Element Buffer (Counter-Clockwise)
	GLuint cursor_elements[]{
		4, 7, 3, //Near 1
		3, 0, 4, //Near 2

		5, 6, 2, //Far 1
		2, 1, 5, //Far 2

		0, 3, 2, //Top 1
		2, 1, 0, //Top 2

		4, 7, 6, //Bottom 1
		6, 5, 4, //Bottom 2

		4, 0, 1, //Left 1
		1, 5, 4, //Left 2

		7, 6, 2, //Right 1
		2, 3, 7  //Right 2
	};

	GLuint CURSOR_VAO;
	glGenVertexArrays(1, &CURSOR_VAO);
	glBindVertexArray(CURSOR_VAO);

#define CURSOR_RENDER_MEM_SIZE sizeof(cursor_vertices) + sizeof(Cursor_Renderable)
	GLuint Cursor_VBO;
	glGenBuffers(1, &Cursor_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Cursor_VBO);
	glBufferData(GL_ARRAY_BUFFER, CURSOR_RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cursor_vertices), cursor_vertices);

	GLuint Cursor_EBO;
	glGenBuffers(1, &Cursor_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Cursor_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cursor_elements), cursor_elements, GL_STATIC_DRAW);

	{
		GLint VertAttrib = 0;
		glVertexAttribPointer(VertAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(VertAttrib);

		GLint PosAttrib = 1;
		glVertexAttribPointer(PosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Cursor_Renderable), (void*)(sizeof(cursor_vertices)));
		glVertexAttribDivisor(PosAttrib, 1);
		glEnableVertexAttribArray(PosAttrib);

		GLint ColorAttrib = 2;
		glVertexAttribPointer(ColorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Cursor_Renderable), (void*)(sizeof(cursor_vertices) + sizeof(vec3)));
		glVertexAttribDivisor(ColorAttrib, 1);
		glEnableVertexAttribArray(ColorAttrib);
	}

	GLuint CursorVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(CursorVertShader, 1, &CursorVertSource, NULL);
	glCompileShader(CursorVertShader);

	GLuint CursorFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(CursorFragShader, 1, &CursorFragSource, NULL);
	glCompileShader(CursorFragShader);

	GLuint CursorShaderProgram = glCreateProgram();
	glAttachShader(CursorShaderProgram, CursorVertShader);
	glAttachShader(CursorShaderProgram, CursorFragShader);
	glBindFragDataLocation(CursorShaderProgram, 0, "CursorColor");
	glLinkProgram(CursorShaderProgram);
	glUseProgram(CursorShaderProgram);

	GLint CursorProjMatrix = glGetUniformLocation(CursorShaderProgram, "Proj");
	GLint CursorViewMatrix = glGetUniformLocation(CursorShaderProgram, "View");

	glUniformMatrix4fv(CursorProjMatrix, 1, GL_FALSE, glm::value_ptr(Proj));
	glUniformMatrix4fv(CursorViewMatrix, 1, GL_FALSE, glm::value_ptr(View));
	/*-------------- Quad Setup -----------*/

	float quad_vertices[]{
		// X     Y
		0.000f, 0.00f, // B L - 0  1-------3
		0.000f, 0.10f, // T L - 1  |	   |
		0.056f, 0.00f, // B R - 2  |	   |
		0.056f, 0.10f  // T R - 3  0-------2
	};

	GLuint quad_elements[]{
		0,1,3,
		3,2,0
	};

	GLuint QUAD_VAO;
	glGenVertexArrays(1, &QUAD_VAO);
	glBindVertexArray(QUAD_VAO);

	GLuint QUAD_EBO;
	glGenBuffers(1, &QUAD_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QUAD_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_elements), quad_elements, GL_STATIC_DRAW);

#define QUAD_RENDER_MEM_SIZE sizeof(quad_vertices) + (sizeof(Quad_Renderable) * NUM_QUADS)
	GLuint HUD_VBO;
	glGenBuffers(1, &HUD_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, HUD_VBO);
	glBufferData(GL_ARRAY_BUFFER, QUAD_RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad_vertices), quad_vertices);

	{
		GLint VertAttrib = 0;
		glVertexAttribPointer(VertAttrib, 2, GL_FLOAT, GL_FALSE, NULL, (void*)0);
		glEnableVertexAttribArray(VertAttrib);

		GLint PosAttrib = 1;
		glVertexAttribPointer(PosAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Quad_Renderable), (void*)(sizeof(quad_vertices)));
		glVertexAttribDivisor(PosAttrib, 1);
		glEnableVertexAttribArray(PosAttrib);

		GLint ScaleAttrib = 2;
		glVertexAttribPointer(ScaleAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Quad_Renderable), (void*)(sizeof(quad_vertices) + sizeof(vec2)));
		glVertexAttribDivisor(ScaleAttrib, 1);
		glEnableVertexAttribArray(ScaleAttrib);

		GLint ColorAttrib = 3;
		glVertexAttribPointer(ColorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Quad_Renderable), (void*)(sizeof(quad_vertices) + sizeof(vec4)));
		glVertexAttribDivisor(ColorAttrib, 1);
		glEnableVertexAttribArray(ColorAttrib);
	}

	GLuint QuadVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(QuadVertShader, 1, &QuadVertSource, NULL);
	glCompileShader(QuadVertShader);

	GLuint QuadFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(QuadFragShader, 1, &QuadFragSource, NULL);
	glCompileShader(QuadFragShader);

	GLuint HUDShaderProgram = glCreateProgram();
	glAttachShader(HUDShaderProgram, QuadVertShader);
	glAttachShader(HUDShaderProgram, QuadFragShader);
	glBindFragDataLocation(HUDShaderProgram, 0, "QuadColor");
	glLinkProgram(HUDShaderProgram);
	glUseProgram(HUDShaderProgram);
	/*-------------- Textures --------------*/
	GLuint TileTextureAtlas;
	{
		int Width, Height, NumChannels;
		uint8* Image;
		stbi_set_flip_vertically_on_load(true);

		//Image = stbi_load("textures/Font.bmp", &Width, &Height, &NumChannels, 0);
		//
		//glGenTextures(1, &FontBitmap);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, FontBitmap);
		//glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Image);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		Image = stbi_load("Tile_Textures.bmp", &Width, &Height, &NumChannels, 0);

		glGenTextures(1, &TileTextureAtlas);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TileTextureAtlas);
		glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(Image);
	}
	/*-------------- Game Level ------------*/

	Level_Data LevelData = {};
	LoadLevelData(&LevelData);

	Game_Level Level;
	GameLevelInit(&Level, &LevelData);

	/*-------------- World --------------*/

	Game_World* World = &Level.World;

	World_Renderer WorldRenderer = {};

	/*-------------- Enemies ------------*/

	Enemy* Enemies = Level.Enemies;

	Enemy_Renderer EnemyRenderer = {};
	EnemyRenderer.EnemyMem = (Enemy_Renderable*)MemoryManager.EnemyPointers[0];

	/*-------------- Bullets ------------*/

	Bullet* Bullets = Level.Bullets;

	Bullet_Renderer BulletRenderer = {};
	BulletRenderer.BulletMem = (Bullet_Renderable*)MemoryManager.BulletPointers[0];

	/*-------------- Towers -------------*/

	Turret* Turrets = Level.Turrets;

	Turret_Renderer TurretRenderer = {};
	TurretRenderer.TurretMem = (Turret_Renderable*)MemoryManager.TowerPointers[0];

	/*-------------- Cursor -------------*/

	Cursor_3D* Cursor = &Level.Cursor;

	Cursor_Renderer CursorRenderer = {};

	/*---------------- HUD --------------*/

	HUD_Renderer HUDRenderer = {};
	HUDRenderer.QuadMem = new Quad_Renderable[NUM_QUADS];

	/*-------------- Input --------------*/

	Game_Camera Camera = {};
	CameraInit(&Camera);

	GameMouse Mouse = {};
	GameMouse PrevMouse = {};
	glfwGetCursorPos(WindowInstance, &PrevMouse.RawX, &PrevMouse.RawY);

	/*-------------- Timing Logic --------------*/

	//Get CPU clock frequency for Timing
	int64 PerformanceFrequency;
	{
		LARGE_INTEGER win32_PerformanceFrequency;
		QueryPerformanceFrequency(&win32_PerformanceFrequency);
		PerformanceFrequency = (int64)win32_PerformanceFrequency.QuadPart;
	}

#define TARGET_FRAMERATE ((float)120);
	float TargetFrameTime = 1 / TARGET_FRAMERATE;
	float DeltaTime = TargetFrameTime;

	LARGE_INTEGER StartCounter;
	LARGE_INTEGER EndCounter;

#define DESIRED_SCHEDULER_GRANULARITY 1 //millisecond
	HRESULT SchedulerResult = timeBeginPeriod(DESIRED_SCHEDULER_GRANULARITY);

	QueryPerformanceCounter(&StartCounter);

	/*-------------------------------------------------------------- Main Game Loop --------------------------------------------------------------*/
	while (!glfwWindowShouldClose(WindowInstance))
	{
		/*------------------------------- Input & Movement -------------------------------*/
		{
			glfwGetCursorPos(WindowInstance, &Mouse.RawX, &Mouse.RawY);

			double XOffset = Mouse.RawX - PrevMouse.RawX;
			double YOffset = PrevMouse.RawY - Mouse.RawY;

			PrevMouse.RawX = Mouse.RawX;
			PrevMouse.RawY = Mouse.RawY;

			//Camera.Position = vec3(2.620, 7.420, 13.28);
			//Camera.Front = vec3(0.130, -0.57, -0.79);
			//Camera.Pitch = -0.63;
			//Camera.Yaw = -1.40;
			CameraProcessMouseMovement(&Camera, XOffset, YOffset); //Update Camera

			Mouse.NormX = ((uint)Mouse.RawX % SCREEN_WIDTH)  / (double)SCREEN_WIDTH;
			Mouse.NormY = ((uint)Mouse.RawY % SCREEN_HEIGHT) / (double)SCREEN_HEIGHT;

			Mouse.NormY = 1 - Mouse.NormY;
			Mouse.NormX *= 2; Mouse.NormX += -1;
			Mouse.NormY *= 2; Mouse.NormY += -1;
		}

		/* Leap-Motion Input */
		Cursor->Connected = LeapController.isConnected();
		if (Cursor->Connected)
		{
			// palm is better for height, fingers are better for x,z
			LeapFrame = LeapController.frame();
			vec3 LeapPos(LeapFrame.pointables().frontmost().tipPosition().x, 
						 LeapFrame.pointables().frontmost().tipPosition().y, 
						 LeapFrame.pointables().frontmost().tipPosition().z);
			LeapPos.x /= 120;
			LeapPos.y /= 200;
			LeapPos.z /= 120;
			Level.Cursor.Position = LeapPos + vec3(3, 2, 7);
		}

		if (glfwGetKey(WindowInstance, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(WindowInstance, GL_TRUE);

		if (glfwGetKey(WindowInstance, GLFW_KEY_W) == GLFW_PRESS) CameraProcessKeyboard(&Camera, CAM_FORWARD , 10.f, DeltaTime);
		if (glfwGetKey(WindowInstance, GLFW_KEY_S) == GLFW_PRESS) CameraProcessKeyboard(&Camera, CAM_BACKWARD, 10.f, DeltaTime);
		if (glfwGetKey(WindowInstance, GLFW_KEY_A) == GLFW_PRESS) CameraProcessKeyboard(&Camera, CAM_LEFT    , 10.f, DeltaTime);
		if (glfwGetKey(WindowInstance, GLFW_KEY_D) == GLFW_PRESS) CameraProcessKeyboard(&Camera, CAM_RIGHT   , 10.f, DeltaTime);

		static bool FirstPress = true;
		if (glfwGetKey(WindowInstance, GLFW_KEY_F) == GLFW_PRESS)
		{
			if (FirstPress)
			{
				SpawnTurret(Turrets, (float)Cursor->Position.x, (float)Cursor->Position.z, SMALL_TURRET);
				Level.Stats.TurretsPlaced += 1;
				FirstPress = false;

			} else {}
		} else FirstPress = true;

		/*------------------------------- Physics ------------------------------*/
		
		static bool test = true;
		if (test)
		{
			for (uint i = 0; i < NUM_ENEMIES; i++) SpawnEnemy(Enemies, GRUNT_ENEMY, vec3(.5, 1, i + 6.5), Level.Path.Nodes);
			test = false;
		}

		SimulateCollisions(Bullets, Enemies);

		/*------------------------------- Enemies ------------------------------*/
		UpdateEnemies(Enemies, DeltaTime);

		EnemiesToRenderable(Enemies, &EnemyRenderer);
		/*------------------------------- Turrets ------------------------------*/
		UpdateTurrets(Turrets, Bullets, Enemies, DeltaTime);

		TurretsToRenderable(Turrets, &TurretRenderer);
		/*------------------------------- Bullets ------------------------------*/
		UpdateBullets(Bullets, DeltaTime);

		BulletsToRenderable(Bullets, &BulletRenderer);
		/*------------------------------- Game World ---------------------------*/

		GameWorldToRenderable(World, &WorldRenderer);
		/*--------------------------------- Level ------------------------------*/

		UpdateLevel(&Level, DeltaTime);
		/*------------------------------- 3D Cursor ----------------------------*/
		Cursor->Position = CursorGetSelectedTile(Cursor);

		CursorToRenderable(Cursor, &CursorRenderer);
		/*---------------------------------- HUD -------------------------------*/
		Cursor->Position = CursorGetSelectedTile(Cursor);

		HUDToRenderable(&Level, &HUDRenderer);
		/*------------------------------- Rendering ----------------------------*/
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		View = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		/* ----- Draw the HUD ----- */
		glBindVertexArray(QUAD_VAO);

		glUseProgram(HUDShaderProgram);

		glBindBuffer(GL_ARRAY_BUFFER, HUD_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad_vertices), HUDRenderer.NumQuads * sizeof(Quad_Renderable), HUDRenderer.QuadMem);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, HUDRenderer.NumQuads);
		/* ----- Draw Cursor ----- */
		glBindVertexArray(CURSOR_VAO);

		glUseProgram(CursorShaderProgram);
		glUniformMatrix4fv(CursorViewMatrix, 1, GL_FALSE, glm::value_ptr(View));

		glBindBuffer(GL_ARRAY_BUFFER, Cursor_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cursor_vertices), sizeof(Cursor_Renderable), &CursorRenderer.Renderable);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		/* ----- Draw Enemies ----- */
		glBindVertexArray(ENEMY_VAO);

		glUseProgram(EnemyShaderProgram);
		glUniformMatrix4fv(EnemyViewMatrix, 1, GL_FALSE, glm::value_ptr(View));

		glBindBuffer(GL_ARRAY_BUFFER, Enemy_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(enemy_vertices), sizeof(Enemy_Renderable) * EnemyRenderer.NumEnemies, EnemyRenderer.EnemyMem);

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, EnemyRenderer.NumEnemies);
		/* ----- Draw Bullets ----- */
		glBindVertexArray(BULLET_VAO);

		glUseProgram(BulletShaderProgram);
		glUniformMatrix4fv(BulletViewMatrix, 1, GL_FALSE, glm::value_ptr(View));

		glBindBuffer(GL_ARRAY_BUFFER, Bullet_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(bullet_vertices), sizeof(Bullet_Renderable) * BulletRenderer.NumBullets, BulletRenderer.BulletMem);

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, BulletRenderer.NumBullets);
		/* ----- Draw Turrets ----- */
		glBindVertexArray(TURRET_VAO);

		glUseProgram(TurretShaderProgram);
		glUniformMatrix4fv(TurretViewMatrix, 1, GL_FALSE, glm::value_ptr(View));

		glBindBuffer(GL_ARRAY_BUFFER, Turret_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(turret_vertices), sizeof(Turret_Renderable) * TurretRenderer.NumTurrets, TurretRenderer.TurretMem);

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, TurretRenderer.NumTurrets);
		/* ----- Draw Solid Blocks ----- */
		glBindVertexArray(SOLID_VAO);

		glUseProgram(SolidShaderProgram);
		glUniformMatrix4fv(SolidViewMatrix, 1, GL_FALSE, glm::value_ptr(View));

		glBindBuffer(GL_ARRAY_BUFFER, Solid_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(solid_vertices), sizeof(Solid_Renderable) * WorldRenderer.NumSolidBlocks, WorldRenderer.SolidMem);

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, WorldRenderer.NumSolidBlocks);
		/* ----- Draw Fluid Blocks ----- */
		glBindVertexArray(FLUID_VAO);
		
		glUseProgram(FluidShaderProgram);
		glUniformMatrix4fv(FluidViewMatrix, 1, GL_FALSE, glm::value_ptr(View));

		static float timer = 0; timer += DeltaTime; if (timer > 4) timer = 0;
		glUniform1f(FluidTimer, timer);
		
		glBindBuffer(GL_ARRAY_BUFFER, Fluid_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(fluid_vertices), sizeof(Fluid_Renderable) * WorldRenderer.NumFluidBlocks, WorldRenderer.FluidMem);
		
		glDrawElementsInstanced(GL_TRIANGLES, 150, GL_UNSIGNED_INT, 0, WorldRenderer.NumFluidBlocks);
		/*------------------------------- Finish Frame -------------------------------*/
		
		// Update the window
		glfwSwapBuffers(WindowInstance);
		glfwPollEvents();

		//Frame Time
		QueryPerformanceCounter(&EndCounter);
		int64 CyclesElapsed = (int64)(EndCounter.QuadPart - StartCounter.QuadPart);
		int64 MillisecondsElapsed = (CyclesElapsed * 1000) / PerformanceFrequency;

		if ((uint)(1000 * TargetFrameTime) > (uint)MillisecondsElapsed)
		{
			uint SleepMS = (uint)(TargetFrameTime * 1000) - (uint)MillisecondsElapsed;
			Sleep(SleepMS);

			QueryPerformanceCounter(&EndCounter);
			CyclesElapsed = (int64)(EndCounter.QuadPart - StartCounter.QuadPart);
			MillisecondsElapsed = (CyclesElapsed * 1000) / PerformanceFrequency;
			StartCounter.QuadPart = EndCounter.QuadPart;
		}
	}

	glfwTerminate(); SaveToWebpage(&Level.Stats);
	VirtualFree(GameMemory, 0, MEM_RELEASE); //Give back the memory

	return 0;
}