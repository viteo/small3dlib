/*
  Example program of small3dlib for Pokitto -- Quake-like level.

  author: Miloslav Ciz
  license: CC0 1.0
*/

#include "Pokitto.h"

#define SUBSAMPLE 3

#if 1   // This can switch between a textured and flat mode.
  #define S3L_Z_BUFFER 2
  #define S3L_SORT 0
  #define S3L_STENCIL_BUFFER 0
  #define S3L_FLAT 0
  #define S3L_PERSPECTIVE_CORRECTION 2
#else
  #define S3L_Z_BUFFER 2
  #define S3L_SORT 0
  #define S3L_STENCIL_BUFFER 0
  #define S3L_FLAT 1
  #define S3L_MAX_TRIANGES_DRAWN 200
#endif

#define S3L_PIXEL_FUNCTION pixelFunc

// Because we'll be writing pixels as 2x2, define the resolution one smaller.
#define BASE_W 109
#define BASE_H 87

#define S3L_RESOLUTION_X (BASE_W - BASE_W / SUBSAMPLE)
#define S3L_RESOLUTION_Y (BASE_H - BASE_H / SUBSAMPLE)

#define S3L_STRICT_NEAR_CULLING 0

#define S3L_COMPUTE_DEPTH 1 // for fog

#define S3L_REDUCED_Z_BUFFER_GRANULARITY 6

#include "small3dlib.h"

#include "levelTexture1Pal.h"
#include "levelModel.h"

Pokitto::Core pokitto;

#if S3L_FLAT
uint8_t triangleColors[LEVEL_TRIANGLE_COUNT];
#endif

static inline uint8_t texture(int32_t u, int32_t v)
{
  u = S3L_wrap(u,LEVEL1_TEXTURE_WIDTH);
  v = S3L_wrap(v,LEVEL1_TEXTURE_HEIGHT);

  uint32_t index = v * LEVEL1_TEXTURE_WIDTH + u;

  return level1Texture[index];
}

S3L_ScreenCoord subsampleMap[BASE_W + SUBSAMPLE];

uint32_t previousTriangle = 100;

static inline uint8_t addIntensity(uint8_t color, int16_t intensity)
{
  int16_t newValue = (color & 0b00001111) + intensity; // value as in HSV
  // TODO: ^ this could be uint8? Would be faster! Also in the below function.

  if (newValue >= 16)
    newValue = 15;

  return (color & 0b11110000) | newValue;
}

static inline uint8_t substractIntensity(uint8_t color, int16_t intensity)
{
  int16_t newValue = (color & 0b00001111) - intensity; // value as in HSV

  if (newValue <= 0)
    return 0;

  return (color & 0b11110000) | newValue;
}

uint8_t c = 0;

S3L_Vec4 uv0, uv1, uv2;

S3L_Index material = 0;

void pixelFunc(S3L_PixelInfo *p)
{
  uint8_t val;
  uint8_t *buf = pokitto.display.screenbuffer;

#if S3L_FLAT
  val = triangleColors[p->triangleIndex];
#else
  if (p->triangleIndex != previousTriangle)
  {
    material = levelMaterials[p->triangleIndex];

    if (material == 1)
      c = 135;
    else if (material == 2)
      c = 213;
    else
      S3L_getIndexedTriangleValues(p->triangleIndex,levelUVIndices,levelUVs,2,&uv0,&uv1,&uv2);

    previousTriangle = p->triangleID;
  }

  S3L_Unit fog = p->depth >> 9;
  
  if (material == 0)
  {
    S3L_Unit uv[2];
    uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric);
    uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric);

    c = texture(uv[0] / 32,uv[1] / 32);
  }

  val = substractIntensity(c,fog);
#endif

  buf += subsampleMap[p->y] * 110;
  buf += subsampleMap[p->x];
  *buf = val;

  buf++;
  *buf = val;
  buf += 109;
  *buf = val;
  buf++;
  *buf = val;
}

S3L_Scene scene;

void draw()
{
  S3L_newFrame();
  S3L_drawScene(scene);
}

unsigned short palette[256];

int main()
{
  for (uint16_t i = 0; i < BASE_W + SUBSAMPLE; ++i)
    subsampleMap[i] = i + i / SUBSAMPLE;

#if S3L_FLAT
  S3L_Vec4 toLight;
  S3L_setVec4(&toLight,10,5,7,0);
  S3L_normalizeVec3(&toLight);

  for (uint16_t i = 0; i < LEVEL_TRIANGLE_COUNT; ++i)
  {
    uint8_t c;

    S3L_Vec4 v0, v1, v2;

    S3L_getIndexedTriangleValues(
      i,
      levelTriangleIndices,
      levelVertices,3,&v0,&v1,&v2);

    material = levelMaterials[i];

    if (material == 1)
      c = 38;
    else if (material == 2)
      c = 53;
    else
      c = 24;

    S3L_Vec4 normal;

    S3L_triangleNormal(v0,v1,v2,&normal);

    triangleColors[i] = addIntensity(c,    
      S3L_max(0,(S3L_dotProductVec3(normal,toLight) + S3L_FRACTIONS_PER_UNIT) / 64));
  }

#endif

  pokitto.begin();

  pokitto.setFrameRate(60);

  pokitto.display.load565Palette(level1Palette);

  S3L_initCamera(&scene.camera);

  levelModelInit();

  S3L_initScene(&levelModel,1,&scene);

  while (pokitto.isRunning())
  {
    if (pokitto.update())
    {
      S3L_Vec4 camF, camR, camU;
      int step = 300;
      int step2 = 8;
 
      S3L_rotationToDirections(
        scene.camera.transform.rotation,
        step,
        &camF,
        &camR,
        &camU);

      if (pokitto.aBtn())
      {
        if (pokitto.upBtn())
          scene.camera.transform.rotation.x += 8;
        else if (pokitto.downBtn())
          scene.camera.transform.rotation.x -= 8;
        else if (pokitto.rightBtn())
          scene.camera.transform.rotation.y += 8;
        else if (pokitto.leftBtn())
          scene.camera.transform.rotation.y -= 8;
      }
      else
      {
        if (pokitto.upBtn())
          S3L_vec3Add(&(scene.camera.transform.translation),camF);
        else if (pokitto.downBtn())
          S3L_vec3Sub(&scene.camera.transform.translation,camF);
        else if (pokitto.rightBtn())
          S3L_vec3Add(&scene.camera.transform.translation,camR);
        else if (pokitto.leftBtn())
          S3L_vec3Sub(&scene.camera.transform.translation,camR);
      }

      draw();
    }
  }
}
