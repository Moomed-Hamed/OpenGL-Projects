/*
------ The Game ------

  Gameplay Loop:
  - Kill enemies to aquire money to buy turrets to kill enemies
	to stop them from destroying the great mcguffin

 ------ Program Structure ------
  Main()
  {
	- Acquire  game memory
	- Initialize glew & glfw

	- Setup OpenGL buffers & VAOs
	- Load Textures

	- Setup & Load Game World
	- Setup Physics Objects
	- Setup Turrets
	- Setup Enemies

	- Initialize Timers

	- Main Game Loop
		- Get & Handle Input

		- Process AI
		- Process Physics
		- Process Animation

		- Prepare for rendering
		- Render the scene

		- Update timers and finish frame
  }

 ------ Memory ------

  Memory Pages:
  - Even though VirtuAlalloc is used to allocate a "contiguous" block of
    *virtual* memory, game memory is allocated in 4096 byte pages, because
	they are the only blocks of memory guaranteed to be physically contiguous

  - An appropriate number of pages is assigned to each of the game's
    systems as needed. Pages are given out as raw pointers

  Initialization:
  - This is a realtime application that is running at a fixed framarate
    and should therefore have no trouble running at maximum theoretical
	load all the time.
	
  - To make sure this is true, memory is allocated and initialized before
	the main game loop begins. This saves me the time of making things more
	dynamic, and ensures the game can run at maximum load with no trouble

 ------ Rendering ------

  Renderables:
  - Objects have a 'renderable' data structure & 'to-renderable' function.
	Giving all objects unique renderable states allows for more fine-grain
	control of the rendering of each type of object.

  Abstractions:
  - When i was a little boy i was chased by an abstraction, and have developed
    a fear of them ever since. As a result, i only use abstractions in my code
	when i absulotely have to.
	 
  - So when i built the renderer, all i did was group the rendering of all objects
    of a single type(turrets, enemies, tiles, etc..) and give the job to a "xxx_renderer"
	object that is responsible for keeping track of all rendering data for those objects
	
  - This makes my life easier without turning my program into an incomprehensible
	object-oriented mess(see LeapMotion.h for reference on what i'm trying to avoid)

 ------ Game Level ------
  Level_Renderer:
   - ...

 ------ Turrets ------
  Turret_Renderer:
   - ...

 ------ Enemies ------
  Enemy_Renderer:
   - ...

 ------ Physics & Timing ------

  Frame Rate:
   - The game enforces a fixed time per frame, this makes the physics simulation
	 Significantly simpler and more accurate. The current goal is 120 frames per
	 second (on my computer)
*/

