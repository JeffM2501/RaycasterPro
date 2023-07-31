# Raycaster Pro
This is a hybrid raycaster system.
It uses traditional raycsting for visibility but uses real OpenGL for rendering.
This allows for constant time rendering of maps regardless of size, rendering time is only affected by what is in the view.

## Method of Operation
The system works by doing a binary search pattern on the rays, from outside in, always checking pairs of rays.
If the two rays hit different cells then the rays are subdivided and checked again (using flags to not recast the same ray multiple times).
When the two rays hit the same cell then no subdivision is needed. This means we can cast only as many rays as are needed for the vissible cells, not each pixel.

Hit cells are tracked and then drawn with OpenGL. Empty cells that were traversed are tracked as well so they can be drawn as floors and ceilings.

This allows each wall and floor to be drawn as a full 3d quad, allowing the GPU to apply effects such as shaders and anisotropic filtering for better visuals. Real shader lighting is also possible using this method, but not included in this demo.

# Building
This project uses game-premake. Run the batch files for windows or the correct premake command for your platform. See https://github.com/raylib-extras/game-premake for more info.

# Running
In release mode the game is fullscreen and works like a FPS. Mouse to rotate WADS to move.

In debug mode it is not fullscreen, and you must hold the right mouse button down to rotate with the mouse, all other controls are the same.

# TODO
* Doors
* Objects
	* Billboards
	* Meshes
* Per face textures
* Skybox
* Atlas generation
* Map resource loading
* Cell Attributes
* AI raycasting (line of sight)
* Editor and asset format?