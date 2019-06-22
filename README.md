# WIP, NOT WORKABLE YET!

# small3dlib

3D software rasterizer for (not only) resource-limited computers.

If you like this, you may also like my similar project: [raycastlib](https://gitlab.com/drummyfish/raycastlib).

## eye-candy previews

TODO

## features

- Very fast, small and efficient.
- Uses only integer math (32bit).
- No dependencies (uses only stdint standard library), extremely portable.
- Single header, KISS.
- Pure C99, tested to run as C++ as well.
- Still flexible -- pixels are left for you to draw in any way you want with a custom fragment-shader like function.
- Perspective correction, 3 modes: none (linear only), full (per-pixel), approximation (per-N-pixels). 
- Different drawing strategies to choose from: none, z-buffer (none, full, reduced), triangle sorting (back-to-front, fron-to-back with stencil buffer).
- Triangles provide barycentric coordinates, thanks to which practically anything that can be achieved with OpenGL can be achieved (texturing, shading, normal-mapping, transparency, PBR, shadow mapping, ...).
- Tested on multiple platforms (PC, Arduboy, Pokitto, Gamebuino META).
- Many compile-time options to tune the performance vs quality.
- Well commented and formatted code.
- Completely free of legal restrictions, do literally everything you want.

## limitations

- Some values, like screen resolution, are a compile-time option due to performance and simplicity, and can't change during runtime.
- Though performance is high, due to multiplatformness it can't match platform-specific rasterizers written in assembly.
- Proper near-plane culling (subdividing triangles) is missing. You can either cull whole triangles completely or "push" them by the near plane. These options are okay when drawing a models not very close to the camera, but e.g. 3D environments may suffer from artifacts.
- Due to the limitations of 32bit integer arithmetics, some types of movement may be jerky.

TODO

## how to use

TODO

## tips/troubleshooting

- Seeing buggy triangles flashing in front of the camera? With the limited 32bit arithmetic far-away things may be overflowing. Try to scale down the scene. If you also don't mind it, set `S3L_STRICT_NEAR_CULLING` to `1` -- this should probably solve it.