# Raycaster Pro
This is a hybrid raycaster system.
It uses traditional raycsting for visibility but uses real OpenGL for rendering.
This allows for constant time rendering of maps regardless of size, rendering time is only affected by what is in the view.

## Method of Operation
The system works by doing a binary search pattern on the rays, from outside in, always checking pairs of rays.
If the two rays hit different cells then the rays are subdivided and checked again (using flags to not recast the same ray multiple times).
When the two rays hit the same cell then no subdivision is needed. This means we can cast only as many rays as are needed for the vissible cells, not each pixel.

Hit cells are tracked and then drawn with OpenGL. Empty cells that were traversed are tracked as well so they can be drawn as floors and ceilings.

