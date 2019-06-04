/*
  author: Miloslav Ciz
  license: CC0
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//#define S3L_PRESET_HIGHEST_QUALITY

#define S3L_FLAT 0
#define S3L_STRICT_NEAR_CULLING 0
#define S3L_PERSPECTIVE_CORRECTION11
#define S3L_SORT 0
#define S3L_Z_BUFFER 1

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 640
#define S3L_RESOLUTION_Y 480

#include "small3dlib.h"

#include "houseTexture.h"
#include "house.h"

int32_t offScreenPixels = 0;
const S3L_Unit ver[] = { S3L_CUBE_VERTICES(S3L_FRACTIONS_PER_UNIT) };
const S3L_Index tri[] = { S3L_CUBE_TRIANGLES };
const S3L_Unit tex_coords[] = { S3L_CUBE_TEXCOORDS(16) };

S3L_Model3D models[2];
S3L_Scene scene;

uint8_t houseVertexLighting[127];

int8_t keys[256];

const uint8_t testTexture[] =
{
  2,2,2,0,0,0,2,2,2,2,0,0,0,2,2,2,
  2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
  2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
  0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
  0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
  2,0,0,0,0,1,1,1,1,1,0,0,0,0,0,2,
  2,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
  2,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
  2,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
  0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
  0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
  0,0,2,2,0,0,0,0,0,0,0,0,1,1,0,0,
  2,0,2,2,0,0,0,0,0,0,0,0,1,1,0,2,
  2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
  2,2,2,0,0,0,2,2,2,2,0,0,0,2,2,2
};
/*
{
  0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
  1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,
  2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
  0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
  1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,
  2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
  0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
  1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,
  2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
  0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
  1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,
  2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
  0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
  1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,
  2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2
};
*/

uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];

uint32_t frame = 0;

void clearScreen()
{
  memset(pixels,0,S3L_RESOLUTION_X * S3L_RESOLUTION_Y * sizeof(uint32_t));
}

static inline void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
  if (x < 0 || x >= S3L_RESOLUTION_X || y < 0 || y >= S3L_RESOLUTION_Y)
    return;

  uint32_t r = red & 0x000000FF;
  r = r << 24;

  uint32_t g = green & 0x000000FF;
  g = g << 16;

  uint32_t b = blue & 0x000000FF;
  b = b << 8;

  pixels[y * S3L_RESOLUTION_X + x] = r | g | b;
}

uint8_t texturePixel(int32_t u, int32_t v)
{
  u %= 16;
  v %= 16;

  return testTexture[v * 16 + u];
}

void houseTex(int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  if (u < 0)
    u = 0;

  if (v < 0)
    v = 0;

  u %= HOUSE_TEXTURE_WIDTH;
  v %= HOUSE_TEXTURE_HEIGHT;

  int index = (v * HOUSE_TEXTURE_WIDTH + u) * 3;

  *r = houseTexture[index];
  *g = houseTexture[index + 1];
  *b = houseTexture[index + 2];
}

int l0, l1, l2;
int previousTriangle = 255;

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleIndex != previousTriangle)
  {
    l0 = houseVertexLighting[houseTriangleIndices[p->triangleIndex * 3]];
    l1 = houseVertexLighting[houseTriangleIndices[p->triangleIndex * 3 + 1]];
    l2 = houseVertexLighting[houseTriangleIndices[p->triangleIndex * 3 + 2]];
    previousTriangle = p->triangleIndex;
  }

  if (p->x < 0 || p ->x >= S3L_RESOLUTION_X || p->y < 0 || p->y >= S3L_RESOLUTION_Y)
  {
    offScreenPixels++;
    return;
  }

  S3L_Unit u, v;
  const S3L_Unit *coords;

  coords = tex_coords + p->triangleIndex * 6;

if (p->modelIndex != 0)
{
  int tmp = p->triangleIndex * 3;

  int i0 = houseUVIndices[tmp];
  int i1 = houseUVIndices[tmp + 1];
  int i2 = houseUVIndices[tmp + 2];

  tmp = i0 * 2;

  int uv0[2];

  uv0[0] = houseUVs[tmp];
  uv0[1] = houseUVs[tmp + 1];

  tmp = i1 * 2;

  int uv1[2];

  uv1[0] = houseUVs[tmp];
  uv1[1] = houseUVs[tmp + 1];

  tmp = i2 * 2;

  int uv2[2];

  uv2[0] = houseUVs[tmp];
  uv2[1] = houseUVs[tmp + 1];
  
  u = S3L_interpolateBarycentric(
    uv0[0],
    uv1[0],
    uv2[0],
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  v = S3L_interpolateBarycentric(
    uv0[1],
    uv1[1],
    uv2[1],
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  uint8_t r,g,b;
  houseTex(
    (u / ((float) S3L_FRACTIONS_PER_UNIT)) * HOUSE_TEXTURE_WIDTH,
    (v / ((float) S3L_FRACTIONS_PER_UNIT)) * HOUSE_TEXTURE_HEIGHT,
    &r,&g,&b);

  uint8_t l = S3L_interpolateBarycentric(l0,l1,l2,
                p->barycentric[0],
                p->barycentric[1],
                p->barycentric[2]);

  l = 255 - l;

  l /= 2;

  int16_t clampTmp = r - l;
  r = clampTmp >= 0 ? clampTmp : 0;

  clampTmp = g - l;
  g = clampTmp >= 0 ? clampTmp : 0;

  clampTmp = b - l;
  b = clampTmp >= 0 ? clampTmp : 0;

  setPixel(p->x,p->y,r,g,b); 
}
else
{
  u = S3L_interpolateBarycentric(
    coords[0],
    coords[2],
    coords[4],
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  v = S3L_interpolateBarycentric(
    coords[1],
    coords[3],
    coords[5],
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  uint8_t col = texturePixel(u,v);
  setPixel(p->x,p->y,col,col * 30,(2 - col) * 120);
}

//  uint8_t dep = (p->depth / 5000.0) * 255;

// setPixel(p->x,p->y,  (p->triangleID * 60) % 255, (p->triangleID * 170) % 255, (p->triangleID * 10) % 255 );

//setPixel(p->x,p->y,sss, (p->triangleID * 37) % 255 ,128);

//setPixel(p->x,p->y,p->modelID * 64,p->modelID * 128,255);

//  setPixel(p->x,p->y,p->barycentric0 / ((float) S3L_FRACTIONS_PER_UNIT) * 255,p->barycentric1 / ((float) S3L_FRACTIONS_PER_UNIT) * 255,p->barycentric2 / ((float) S3L_FRACTIONS_PER_UNIT) * 255);
}

S3L_Transform3D modelTransform;
S3L_DrawConfig conf;

clock_t nextT;

int fps = 0;

void recomputeLight()
{ 
  S3L_Mat4 m;

  S3L_makeWorldMatrix(scene.models[1].transform,&m);

  int radius = S3L_FRACTIONS_PER_UNIT * 12;
 
  for (int i = 0; i < 127; ++i)
  {
    houseVertexLighting[i] = 255;
  }
}

void draw()
{
  S3L_newFrame();

  offScreenPixels = 0;

  clearScreen();

  uint32_t f = frame;

  if (f % 16 == 0)
    recomputeLight();

  scene.models[0].transform.translation.x = sin(f / 16.0) * S3L_FRACTIONS_PER_UNIT;

  S3L_drawScene(scene);

  if (offScreenPixels > 0)
    printf("offscreen pixels: %d\n",offScreenPixels);

  clock_t nowT = clock();

  double timeDiff = ((double) (nowT - nextT)) / CLOCKS_PER_SEC;

  fps++;

  if (timeDiff >= 1.0)
  {
    nextT = nowT;
    printf("FPS: %d\n",fps);

    printf("camera: ");
    S3L_logTransform3D(scene.camera.transform);
    fps = 0;
  }
}

int main()
{
  SDL_Window *window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, S3L_RESOLUTION_X, S3L_RESOLUTION_Y, SDL_WINDOW_SHOWN); 
  SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
  SDL_Texture *texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC, S3L_RESOLUTION_X, S3L_RESOLUTION_Y);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_Event event;

  nextT = clock();

  S3L_initCamera(&scene.camera);

//  scene.camera.transform.translation.z = -S3L_FRACTIONS_PER_UNIT * 2;

scene.camera.transform.translation.x = 105;
scene.camera.transform.translation.y = 469;
scene.camera.transform.translation.z = 9;

scene.camera.transform.rotation.x = -35;
scene.camera.transform.rotation.y = 128;
scene.camera.transform.rotation.z = 0;

S3L_setTransform3D(3196,1814,5958,-18,300,0,512,512,512,&(scene.camera.transform));

  scene.modelCount = 2;
  scene.models = models;

  scene.models[0].vertices = ver;
  scene.models[0].vertexCount = S3L_CUBE_VERTEX_COUNT; 
  scene.models[0].triangles = tri;
  scene.models[0].triangleCount = S3L_CUBE_TRIANGLE_COUNT;
  S3L_initTransoform3D(&(scene.models[0].transform));
  S3L_initDrawConfig(&(scene.models[0].config));
  scene.models[0].transform.translation.x = S3L_FRACTIONS_PER_UNIT;

//  scene.models[1] = scene.models[0];
//  scene.models[1].transform.translation.x = 0.5 * S3L_FRACTIONS_PER_UNIT;

  scene.models[1] = houseModel;
  S3L_initTransoform3D(&(scene.models[1].transform));
  S3L_initDrawConfig(&(scene.models[1].config));
  scene.models[1].transform.translation.y = -1 * S3L_FRACTIONS_PER_UNIT;
  scene.models[1].transform.translation.z = 4 * S3L_FRACTIONS_PER_UNIT;

  recomputeLight();

//  scene.camera.transform.translation.x = S3L_FRACTIONS_PER_UNIT;
//  scene.camera.transform.translation.y = S3L_FRACTIONS_PER_UNIT;

  S3L_initTransoform3D(&modelTransform);
  S3L_initDrawConfig(&conf);

  int running = 1;

  for (int i = 0; i < 256; ++i)
    keys[i] = 0;

  while (running)
  {
    draw();
    SDL_UpdateTexture(texture,NULL,pixels,S3L_RESOLUTION_X * sizeof(uint32_t));

    int code = 0;

    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          running = 0;
          break;

        case SDL_KEYDOWN:
          code = 'a' + event.key.keysym.scancode - SDL_SCANCODE_A;

          if (code >= 0 && code <= 255)
            keys[code] = 1;

          break;

        case SDL_KEYUP:
          code = 'a' + event.key.keysym.scancode - SDL_SCANCODE_A;

          if (code >= 0 && code <= 255)
            keys[code] = 0;

          break;

        default:
          break;
      }
    }

    S3L_Vec4 camF, camR, camU;
    int step = 50;
 
    S3L_rotationToDirections(
      scene.camera.transform.rotation,
      step,
      &camF,
      &camR,
      &camU);

    if (keys['w'])
      S3L_vec3Add(&scene.camera.transform.translation,camF);

    if (keys['s'])
      S3L_vec3Sub(&scene.camera.transform.translation,camF);

    if (keys['d'])
      S3L_vec3Add(&scene.camera.transform.translation,camR);

    if (keys['a'])
      S3L_vec3Sub(&scene.camera.transform.translation,camR);

    if (keys['c'])
      S3L_vec3Add(&scene.camera.transform.translation,camU);

    if (keys['x'])
      S3L_vec3Sub(&scene.camera.transform.translation,camU);

    if (keys['q'])
      scene.camera.transform.rotation.y -= 1;

    if (keys['e'])
      scene.camera.transform.rotation.y += 1;

    if (keys['r'])
      scene.camera.transform.rotation.x -= 1;

    if (keys['t'])
      scene.camera.transform.rotation.x += 1;

    if (keys['f'])
      scene.camera.transform.rotation.z -= 1;

    if (keys['g'])
      scene.camera.transform.rotation.z += 1;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);

    frame++;
  }

  return 0;
}
