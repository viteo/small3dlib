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
#define S3L_Z_BUFFER 2

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 800
#define S3L_RESOLUTION_Y 600

#include "../small3dlib.h"

#include "cityModel.h"
#include "cityTexture.h"
#include "carModel.h"

#define TEXTURE_W 256
#define TEXTURE_H 256

#define MAX_VELOCITY 800

S3L_Model3D models[2];

const uint8_t collisionMap[8 * 10] =
{
  1,1,1,1,1,1,1,1,
  1,1,1,1,0,0,0,1,
  1,1,1,1,0,1,0,1,
  2,2,1,0,0,0,0,3,
  1,2,1,0,1,1,3,1,
  2,0,0,0,1,1,3,3,
  1,0,1,0,0,1,1,1,
  1,0,0,0,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1
};

S3L_Scene scene;

uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];

uint32_t frame = 0;

void clearScreen()
{
  uint32_t index = 0;

  for (uint16_t y = 0; y < S3L_RESOLUTION_Y; ++y)
  {
    S3L_Unit t = S3L_min(S3L_FRACTIONS_PER_UNIT,((y * S3L_FRACTIONS_PER_UNIT) / S3L_RESOLUTION_Y) * 4);

    uint32_t r = S3L_interpolateByUnit(200,242,t);
    uint32_t g = S3L_interpolateByUnit(102,255,t);
    uint32_t b = S3L_interpolateByUnit(255,230,t);

    uint32_t color = (r << 24) | (g << 16 ) | (b << 8);

    for (uint16_t x = 0; x < S3L_RESOLUTION_X; ++x)
    {
      pixels[index] = color;
      index++;
    }
  }
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

static inline uint32_t collision(S3L_Vec4 worldPosition)
{
  worldPosition.x /= S3L_FRACTIONS_PER_UNIT;
  worldPosition.z /= -S3L_FRACTIONS_PER_UNIT;    

  uint16_t index = worldPosition.z * 8 + worldPosition.x;

  return collisionMap[index];
}

void handleCollision(S3L_Vec4 *pos, S3L_Vec4 previousPos)
{
        S3L_Vec4 newPos = *pos;
        newPos.x = previousPos.x;
          
        if (collision(newPos))
        {
          newPos = *pos;
          newPos.z = previousPos.z;

          if (collision(newPos))
            newPos = previousPos;
        }

        *pos = newPos;

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

  carModel.transform.translation.x = 7 * (S3L_FRACTIONS_PER_UNIT / 2);
  carModel.transform.translation.z = -7 * (S3L_FRACTIONS_PER_UNIT / 2);
  carModel.transform.translation.y = (S3L_FRACTIONS_PER_UNIT / 32);

  models[0] = cityModel;
  models[1] = carModel;

  S3L_initScene(models,2,&scene);

  scene.camera.transform.translation.z = -S3L_FRACTIONS_PER_UNIT * 8;

  int running = 1;

  clock_t nextPrintT;

  nextPrintT = clock();

  S3L_Vec4 carDirection;

  S3L_initVec4(&carDirection);
  
  scene.camera.transform.translation.y = (3 * S3L_FRACTIONS_PER_UNIT) / 5;
  scene.camera.transform.rotation.x = -S3L_FRACTIONS_PER_UNIT / 16;

  int16_t velocity = 0;

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

    int16_t step = velocity * frameDiff;
    int16_t stepFriction = 300 * frameDiff;
    int16_t stepRotation = 200 * frameDiff * S3L_max(0,velocity - 400) / ((float) MAX_VELOCITY);
    int16_t stepVelocity = S3L_nonZero(1000 * frameDiff);

    if (stepRotation == 0 && S3L_abs(velocity) >= 200)
      stepRotation = 1;

    if (velocity < 0)
      stepRotation *= -1;

    if (state[SDL_SCANCODE_LEFT])
    {
      models[1].transform.rotation.y += stepRotation;
      models[1].transform.rotation.z = S3L_min(velocity / 64, models[1].transform.rotation.z + 1);
    }
    else if (state[SDL_SCANCODE_RIGHT])
    {
      models[1].transform.rotation.y -= stepRotation;
      models[1].transform.rotation.z = S3L_max(-velocity / 64, models[1].transform.rotation.z - 1);
    }
    else
      models[1].transform.rotation.z = (models[1].transform.rotation.z * 3) / 4;

    S3L_rotationToDirections(models[1].transform.rotation,S3L_FRACTIONS_PER_UNIT,&carDirection,0,0);

    S3L_Vec4 previousCarPos = models[1].transform.translation;
    S3L_Vec4 previousCamPos = scene.camera.transform.translation;

    int16_t friction = 0;

    if (state[SDL_SCANCODE_UP])
      velocity = S3L_min(MAX_VELOCITY,velocity + (velocity < 0 ? (2 * stepVelocity) : stepVelocity));
    else if (state[SDL_SCANCODE_DOWN])
      velocity = S3L_max(-MAX_VELOCITY,velocity - (velocity > 0 ? (2 * stepVelocity) : stepVelocity));
    else
      friction = 1;

    models[1].transform.translation.x += (carDirection.x * step) / S3L_FRACTIONS_PER_UNIT;
    models[1].transform.translation.z += (carDirection.z * step) / S3L_FRACTIONS_PER_UNIT;

    uint8_t coll = collision(models[1].transform.translation);

    if (coll != 0)
    {
      if (coll == 1)
      {
        handleCollision(&(models[1].transform.translation),previousCarPos);
        friction = 8;
      }
      else if (coll == 2)
      {
        models[1].transform.translation.x += 5 * S3L_FRACTIONS_PER_UNIT;
        models[1].transform.translation.z += 2 * S3L_FRACTIONS_PER_UNIT;
      }
      else
      {
        models[1].transform.translation.x -= 5 * S3L_FRACTIONS_PER_UNIT;
        models[1].transform.translation.z -= 2 * S3L_FRACTIONS_PER_UNIT;
      }
    }

    if (velocity > 0)
      velocity = S3L_max(0,velocity - stepFriction * friction);
    else
      velocity = S3L_min(0,velocity + stepFriction * friction);

    S3L_Unit cameraDistance =
      S3L_FRACTIONS_PER_UNIT / 2 + (S3L_abs(velocity) * (S3L_FRACTIONS_PER_UNIT / 2) / MAX_VELOCITY);

    scene.camera.transform.translation.x =
      scene.models[1].transform.translation.x - (carDirection.x * cameraDistance) / S3L_FRACTIONS_PER_UNIT;

    scene.camera.transform.translation.z =
      scene.models[1].transform.translation.z - (carDirection.z * cameraDistance) / S3L_FRACTIONS_PER_UNIT;

    scene.camera.transform.rotation.y = models[1].transform.rotation.y;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,textureSDL,NULL,NULL);
    SDL_RenderPresent(renderer);

    frame++;
  }

  return 0;
}
