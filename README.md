


# Shader function

1. **GeomShader**

    Rasterize mesh.

    Geometry -> Position(cam), Normal(cam), Color(cam)


2. **DepthShader**

    Compute depth and depth^2 in light source's space

    Geometry -> Depth&2(light)


3. **DepthBackgroundShader** (deferred)

    Set the depth of background (non-object) from 0 to a high value, as this is difficult to implement in DepthShader

    Depth&2(light) -> Depth&2(light)


4. **ShadowShader** (deferred)

    Generate shadow
    Search for blockers to decide filter size and then apply filter

    Depth&2(light), Position(cam) -> BlockerSearchRegion. ->  AvgBlockerDepth. -> FilterSize. -> Shadow(cam)


5. Blend

    Perform Blinn-Phong shading and apply shadow

    Shadow, Normal -> Yeah!
