# CPU RayTracer

## Features
- Whitted style raytracer
  - Anti aliasing
- PathTracer
- BVH Acceleration structure
  - Binning / SAH
  - Top Level BVH 
  - Instancing
  - Rigid Animations
  - Heatmap renderer displaying the cost of each ray
- Multithreading
- Post-processing effects. (Vignette, Gamma correction, Chromatic aberration)
- Materials (albedo, specular, dieletrics, textures)
- Camera controlls

## Future work
- Skybox
- Top level BVH construction using: Fast Agglomerative Clustering for Rendering, Walter et al., 2008
- GPGPU implementation
- PathTracing Importance Sampling
- PathTracing various filtering techniques
- Improved Camera controlls/implementation
- Graphical User Interface

## Credits

### Contributions 
- Arjen Simons (https://github.com/ArjenSimons): Whitted style raytracer, BVH Acceleration structure, multithreading, pp effects, materials
- Robert Oost (https://github.com/robertoost): PathTracer, Scene Management, pp effects, BVH heatmap renderer
- Jan Willem de Ruig (https://github.com/De-Signated): Camera controlls, torus intersection, directional/spot lights

### Used libraries
advgrtmpl8 https://github.com/jbikker/advgrtmpl8<br/>
tinyobj https://github.com/tinyobjloader/tinyobjloader<br/>
stb_image https://github.com/nothings/stb<br/>
ThreadPool https://github.com/progschj/ThreadPool<br/>
