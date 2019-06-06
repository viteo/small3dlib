/*
  Example for small3dlib: model viewer.

  author: Miloslav Ciz
  license: CC0
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define S3L_FLAT 0
#define S3L_STRICT_NEAR_CULLING 1
#define S3L_PERSPECTIVE_CORRECTION 0
#define S3L_SORT 0
#define S3L_STENCIL_BUFFER 0
#define S3L_Z_BUFFER 1

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 800
#define S3L_RESOLUTION_Y 600

#include "../small3dlib.h"

#include "houseTexture.h"
#include "houseModel.h"

#define TEXTURE_W 128
#define TEXTURE_H 128

S3L_Unit houseNormals[HOUSE_VERTEX_COUNT * 3];

S3L_Model3D model;
S3L_Scene scene;

uint8_t *texture;

uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];

uint32_t frame = 0;

void clearScreen()
{
  memset(pixels,200,S3L_RESOLUTION_X * S3L_RESOLUTION_Y * sizeof(uint32_t));
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

void sampleTexture(int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  int32_t index = (v * TEXTURE_W + u) * 3;

  index = S3L_clamp(index,0,TEXTURE_W * TEXTURE_H * 3);

  *r = texture[index];
  index++;
  *g = texture[index];
  index++;
  *b = texture[index];
}

int16_t previousTriangle = -1;
S3L_Unit uv0[2], uv1[2], uv2[2];
uint16_t l0, l1, l2;
S3L_Vec4 toLight;

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleIndex != previousTriangle)
  {
    int16_t index = p->triangleIndex * 3;

    int16_t i0 = houseUVIndices[index];
    int16_t i1 = houseUVIndices[index + 1];
    int16_t i2 = houseUVIndices[index + 2];

    index = i0 * 2;

    uv0[0] = houseUVs[index];
    uv0[1] = houseUVs[index + 1];

    index = i1 * 2;

    uv1[0] = houseUVs[index];
    uv1[1] = houseUVs[index + 1];

    index = i2 * 2;

    uv2[0] = houseUVs[index];
    uv2[1] = houseUVs[index + 1];    

    S3L_Vec4 n0, n1, n2;

    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3] * 3;

    n0.x = houseNormals[index];
    index++;
    n0.y = houseNormals[index];
    index++;
    n0.z = houseNormals[index];

    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 1] * 3;

    n1.x = houseNormals[index];
    index++;
    n1.y = houseNormals[index];
    index++;
    n1.z = houseNormals[index];
 
    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 2] * 3;

    n2.x = houseNormals[index];
    index++;
    n2.y = houseNormals[index];
    index++;
    n2.z = houseNormals[index];

    l0 = 256 + S3L_clamp(S3L_dotProductVec3(n0,toLight),-511,511) / 2;
    l1 = 256 + S3L_clamp(S3L_dotProductVec3(n1,toLight),-511,511) / 2;
    l2 = 256 + S3L_clamp(S3L_dotProductVec3(n2,toLight),-511,511) / 2;

    previousTriangle = p->triangleIndex;
  }

  S3L_Unit uv[2];

  uv[0] = S3L_interpolateBarycentric(
    uv0[0],
    uv1[0],
    uv2[0],
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  uv[1] = S3L_interpolateBarycentric(
    uv0[1],
    uv1[1],
    uv2[1],
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  int16_t l = S3L_interpolateBarycentric(
    l0,
    l1,
    l2,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  uint8_t r,g,b;

  sampleTexture(uv[0] / 4,uv[1] / 4,&r,&g,&b);

  r = S3L_clamp((((int16_t) r) * l) / S3L_FRACTIONS_PER_UNIT,0,255);
  g = S3L_clamp((((int16_t) g) * l) / S3L_FRACTIONS_PER_UNIT,0,255);
  b = S3L_clamp((((int16_t) b) * l) / S3L_FRACTIONS_PER_UNIT,0,255);

  setPixel(p->x,p->y,r,g,b); 
}

S3L_Transform3D modelTransform;
S3L_DrawConfig conf;

void draw()
{
  S3L_newFrame();

  clearScreen();

  S3L_drawScene(scene);
}

int16_t fps = 0;

int main()
{
  SDL_Window *window = SDL_CreateWindow("model viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, S3L_RESOLUTION_X, S3L_RESOLUTION_Y, SDL_WINDOW_SHOWN); 
  SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
  SDL_Texture *textureSDL = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC, S3L_RESOLUTION_X, S3L_RESOLUTION_Y);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_Event event;

  toLight.x = 10;
  toLight.y = 10;
  toLight.z = 10;

  S3L_normalizeVec3(&toLight);

  S3L_initCamera(&scene.camera);

  scene.camera.transform.translation.z = -S3L_FRACTIONS_PER_UNIT * 8;

  texture = houseTexture;

  scene.modelCount = 1;
  scene.models = &model;

  scene.models[0] = houseModel;
  S3L_initTransoform3D(&(scene.models[0].transform));
  S3L_initDrawConfig(&(scene.models[0].config));

  S3L_initTransoform3D(&modelTransform);
  S3L_initDrawConfig(&conf);

  int running = 1;
   
  S3L_computeModelNormals(scene.models[0],houseNormals,1);

  clock_t nextPrintT;

  nextPrintT = clock();

  while (running)
  {
    clock_t frameStartT = clock();

    draw();

    fps++;

    SDL_UpdateTexture(textureSDL,NULL,pixels,S3L_RESOLUTION_X * sizeof(uint32_t));

    clock_t nowT = clock();

    double timeDiff = ((double) (nowT - nextPrintT)) / CLOCKS_PER_SEC;
    double frameDiff = ((double) (nowT - frameStartT)) / CLOCKS_PER_SEC;

    if (timeDiff >= 1.0)
    {
      nextPrintT = nowT;
      printf("FPS: %d\n",fps);
      fps = 0;
    }

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        running = 0;
    }

    uint8_t *state = SDL_GetKeyboardState(NULL);

    int16_t rotationStep = S3L_max(1,300 * frameDiff);
    int16_t zoomStep = S3L_max(1,3000 * frameDiff);
    int16_t fovStep = S3L_max(1,1000 * frameDiff);

    if (!state[SDL_SCANCODE_LCTRL])
    {
      if (state[SDL_SCANCODE_LEFT])
        model.transform.rotation.y += rotationStep;
      else if (state[SDL_SCANCODE_RIGHT])
        model.transform.rotation.y -= rotationStep;
      
      if (state[SDL_SCANCODE_DOWN])
        model.transform.rotation.x += rotationStep;
      else if (state[SDL_SCANCODE_UP])
        model.transform.rotation.x -= rotationStep;
    }
    else
    {
      if (state[SDL_SCANCODE_LEFT])
        scene.camera.focalLength =
          S3L_min(S3L_FRACTIONS_PER_UNIT * 5,scene.camera.focalLength + fovStep);
      else if (state[SDL_SCANCODE_RIGHT])
        scene.camera.focalLength =
          S3L_max(S3L_FRACTIONS_PER_UNIT / 2,scene.camera.focalLength - fovStep);

      if (state[SDL_SCANCODE_UP])
        scene.camera.transform.translation.z =
          S3L_min(S3L_FRACTIONS_PER_UNIT, scene.camera.transform.translation.z + zoomStep);
      else if (state[SDL_SCANCODE_DOWN])
        scene.camera.transform.translation.z =
          S3L_max(-S3L_FRACTIONS_PER_UNIT * 16, scene.camera.transform.translation.z - zoomStep);
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,textureSDL,NULL,NULL);
    SDL_RenderPresent(renderer);

    frame++;
  }

  return 0;
}
