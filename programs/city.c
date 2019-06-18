/*
  author: Miloslav Ciz
  license: CC0
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define S3L_FLAT 0
#define S3L_STRICT_NEAR_CULLING 0
#define S3L_PERSPECTIVE_CORRECTION 2
#define S3L_SORT 0
#define S3L_STENCIL_BUFFER 0
#define S3L_Z_BUFFER 1

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 800
#define S3L_RESOLUTION_Y 600

#include "../small3dlib.h"

#include "cityModel.h"
#include "cityTexture.h"
#include "carModel.h"

#define TEXTURE_W 256
#define TEXTURE_H 256

S3L_Model3D models[2];

S3L_Scene scene;

uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];

uint32_t frame = 0;

void clearScreen()
{
  memset(pixels,255,S3L_RESOLUTION_X * S3L_RESOLUTION_Y * sizeof(uint32_t));
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

void sampleTexture(uint8_t *texture, int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  u = S3L_clamp(u,0,TEXTURE_W - 1);
  v = S3L_clamp(v,0,TEXTURE_H - 1);

  int32_t index = (v * TEXTURE_W + u) * 3;

  *r = texture[index];
  index++;
  *g = texture[index];
  index++;
  *b = texture[index];
}

uint32_t previousTriangle = -1;
S3L_Unit uv0[2], uv1[2], uv2[2];

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleID != previousTriangle)
  {
    S3L_Index *uvIndices;
    S3L_Unit *uvs;

    if (p->modelIndex == 0)
    {
      uvIndices = cityUVIndices;
      uvs = cityUVs;
    }
    else
    {
      uvIndices = carUVIndices;
      uvs = carUVs;
    }

    int16_t index;

    index = p->triangleIndex * 3;

    int16_t i0 = uvIndices[index];
    int16_t i1 = uvIndices[index + 1];
    int16_t i2 = uvIndices[index + 2];

    index = i0 * 2;

    uv0[0] = uvs[index];
    uv0[1] = uvs[index + 1];

    index = i1 * 2;

    uv1[0] = uvs[index];
    uv1[1] = uvs[index + 1];

    index = i2 * 2;

    uv2[0] = uvs[index];
    uv2[1] = uvs[index + 1];

    previousTriangle = p->triangleID;
  }

  uint8_t r,g,b;

  S3L_Unit uv[2];

  uv[0] = S3L_interpolateBarycentric(uv0[0],uv1[0],uv2[0],p->barycentric);
  uv[1] = S3L_interpolateBarycentric(uv0[1],uv1[1],uv2[1],p->barycentric);

  sampleTexture(cityTexture,uv[0] / 2,uv[1] / 2,&r,&g,&b);
  
  setPixel(p->x,p->y,r,g,b); 
}

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

  cityModelInit();
  carModelInit();

  models[0] = cityModel;
  models[1] = carModel;

  S3L_initScene(models,2,&scene);

  scene.camera.transform.translation.z = -S3L_FRACTIONS_PER_UNIT * 8;

  int running = 1;

  clock_t nextPrintT;

  nextPrintT = clock();

  S3L_Vec4 carDirection;

  S3L_initVec4(&carDirection);

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

    int16_t step = S3L_max(1,3000 * frameDiff);
    int16_t stepRotation = S3L_max(1,300 * frameDiff);

    if (state[SDL_SCANCODE_LEFT])
    {
      models[1].transform.rotation.y += stepRotation;
      S3L_rotationToDirections(models[1].transform.rotation,S3L_FRACTIONS_PER_UNIT,&carDirection,0,0);
    }
    else if (state[SDL_SCANCODE_RIGHT])
    {
      models[1].transform.rotation.y -= stepRotation;
      S3L_rotationToDirections(models[1].transform.rotation,S3L_FRACTIONS_PER_UNIT,&carDirection,0,0);
    }

    if (state[SDL_SCANCODE_UP])
    {
      models[1].transform.translation.x += (carDirection.x * step) / S3L_FRACTIONS_PER_UNIT;
      models[1].transform.translation.z += (carDirection.z * step) / S3L_FRACTIONS_PER_UNIT;
    }
    else if (state[SDL_SCANCODE_DOWN])
    {
      models[1].transform.translation.x -= (carDirection.x * step) / S3L_FRACTIONS_PER_UNIT;
      models[1].transform.translation.z -= (carDirection.z * step) / S3L_FRACTIONS_PER_UNIT;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,textureSDL,NULL,NULL);
    SDL_RenderPresent(renderer);

    frame++;
  }

  return 0;
}