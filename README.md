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
- Multithreading
- Post-processing effects. (Vignette, Gamma correction, Chromatic aberration)
- Materials (albedo, specular, dieletrics, textures)
- Camera controlls

## Credits

### Contributions 
- Arjen Simons (https://github.com/ArjenSimons): Whitted style raytracer, BVH Acceleration structure, multithreading, pp effects, materials
- Robert Oost (https://github.com/robertoost): PathTracer, Scene Management, pp effects
- Jan Willem de Ruig (https://github.com/De-Signated): Camera controlls, torus intersection, directional/spot lights

### Used libraries
advgrtmpl8 https://github.com/jbikker/advgrtmpl8<br/>
tinyobj https://github.com/tinyobjloader/tinyobjloader<br/>
stb_image https://github.com/nothings/stb<br/>
ThreadPool https://github.com/progschj/ThreadPool<br/>
