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
- Triangles provide barycentric coordinates, thanks to which practically anything that can be achieved with OpenGL can be achieved (texturing, shading, normal-mapping, texture fitering, transparency, PBR, shadow mapping, MIP mapping, ...).
- Tested on multiple platforms (TODO).
- Many compile-time options to tune the performance vs quality.
- Similar to OpenGL in principle, but simpler, easier to use, with higher-level features.
- Tools (Python scripts) for converting 3D models and textures to C array format used by the library.
- Well commented and formatted code.
- Completely free of legal restrictions, do literally anything you want.

**NOTE**: Backwards compatibility isn't a goal of this libraray. It is meant to
be an as-is set of tools that the users is welcome to adjust for their
specific project. So new features will be preferred to keeping the same
interface.

## limitations

- Some values, like screen resolution, are a compile-time option due to performance and simplicity, and can't change during runtime.
- No scenegraph (object parenting), just a scene list. Parenting can still be achieved by using cutom transform matrices.
- Though performance is high, due to multiplatformness it can't match platform-specific rasterizers written in assembly.
- There is no far plane.
- There is a near plane, but a proper culling by it (subdividing triangles) is missing. You can either cull whole triangles completely or "push" them by the near plane. These options are okay when drawing a models not very close to the camera, but e.g. 3D environments may suffer from artifacts.
- Due to the limitations of 32bit integer arithmetics, some types of movement (particularly camera) may look jerky, and artifact may appear in specific situations.

## how to use

TODO

## tips/troubleshooting

- Seeing buggy triangles flashing in front of the camera? With the limited 32bit arithmetic far-away things may be overflowing. Try to scale down the scene. If you also don't mind it, set `S3L_STRICT_NEAR_CULLING` to `1` -- this should probably solve it.
- Seeing triangles disappear randomly in sorted modes? This is because the size of the memory for triangle sorting is limited by default -- increase `S3L_MAX_TRIANGLES_DRAWN`.
- Sorted mode sorts triangles before drawing, but sometimes you need to control the drawing order more precisely. This can be done by reordering the objects in the scene list or rendering the scene multiple times without clearing the screen.

## license

Everything is CC0 1.0 + a waiver of all other IP rights (including patents). The art used in demos is either my own released nder CC0 or someone else's released under CC0.

Please support free software and free culture by using free licenses and/or waivers.

If you'd like to support me or just read something about me and my projects, visit my site: [www.tastyfish.cz](http://www.tastyfish.cz/).