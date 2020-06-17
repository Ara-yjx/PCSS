


# Shader function

1. GeomShader

    Rasterize mesh.

    Geometry -> Position(cam), Normal(cam), Color(cam)


2. ShadowmapShader

    Generate shadowmap (depth)

    Geometry -> Shadowmap(light)

3. ShadowShader

    Generate shadow/blockers (cam)

    Shadowmap[tex], Position(cam) -> Shadow(cam)

4. FilterShader

    Search for blockers to decide filter size and then apply filter

    Position(cam), Shadow(cam), Shadowmap[tex] -> BlockerSearchRegion. -> AvgBlockerDepth. -> FilterSize. -> FilteredShadow(cam)

5. Blend

    Do Blinn-Phong shading and apply shadow

    FilteredShadow, Normal, Color -> Yeah!
