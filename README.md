# PCSS

by Jiaxi Ye

CS291A Final Project


# Program Introduction

This program implements Percentage Closer Soft Shadow.

In general, this program rasterizes the scene, generates shadowmap, computes filtersize from blocker's depth, applies the filter, and finally applies the shadow factor.


## Shading Process

1. **Geometry Shader**

    Rasterize mesh.

    `Geometry -> Position(cam), Normal(cam), Color(cam)`


2. **Depth Shader**

    Compute depth (and depth^2) from the direction of light source.

    `Geometry -> Depth(light)`


3. **Depth Background Shader** (deferred)

    A patch-up for the Depth shader. Set the depth of background (non-object) from 0 to a high value, as this is difficult to implement in DepthShader.

    `Depth(light) -> Depth(light)`


4. **ShadowShader** (deferred)

    Generate shadow.

    Compute the size of blocker search region based on receiver's depth.  Search for blockers to decide filter size. Finally apply the filter.

    `Depth(light), Position(cam) -> BlockerSearchRegion. ->  AvgBlockerDepth. -> FilterSize. -> Shadow(cam)`


5. **Blend** (deferred)

    Perform Blinn-Phong shading and apply the shadow.

    `Position, Normal, Color, Shadow -> Yeah!`


I tried to implement Variance Soft Shadow, but it yields strange noise in the shadow. I should look more into it.


## Learnings

- I learned of the idea of utilizing hardward optimized operation to accelerate computation, which in this project is, using MipMap to compute the average value of an arbitrarily large area, in order to get the percentage of blockers.

- From the engineering perspective, I learned how to store, pass, and fetch data of various framebuffers between shaders efficiently and safely.


## References

PCSS:  
Fernando, Randima.
*Percentage-Closer Soft Shadows*.

PCF:  
Reeves, William T, et al. 
*Rendering Antialiased Shadow with Depth Maps*



# Usage

## Compilation and Launch

In the project root directory,

`$ mkdir build`

`$ cd build`

`$ cmake ..`

`$ make main`

`$ ./main`



## Controls

- Rotate the scene with arrow keys.

- This program supports three light sources. Each light can move horizontally and change color.

- Select a light in the bottom box of the panel, and move that light with WASD keys
