/*
  Example program for small3dlib -- a GTA-like game demo.

  author: Miloslav Ciz
  license: CC0 1.0
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>

#define S3L_FLAT 0
#define S3L_NEAR_CROSS_STRATEGY 3
#define S3L_PERSPECTIVE_CORRECTION 2
#define S3L_SORT 0
#define S3L_STENCIL_BUFFER 0
#define S3L_Z_BUFFER 2

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 640
#define S3L_RESOLUTION_Y 480

#include "../small3dlib.h"

#include "cityModel.h"
#include "cityTexture.h"
#include "carModel.h"

#define TEXTURE_W 256
#define TEXTURE_H 256

#define MAX_VELOCITY 1000
#define ACCELERATION 700
#define TURN_SPEED 300
#define FRICTION 600

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
  uint8_t *p = ((uint8_t *) pixels) + (y * S3L_RESOLUTION_X + x) * 4 + 1;

  *p = blue;
  ++p;
  *p = green;
  ++p;
  *p = red;
}

void sampleTexture(int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  u = S3L_clamp(u,0,CITY_TEXTURE_WIDTH - 1);
  v = S3L_clamp(v,0,CITY_TEXTURE_HEIGHT - 1);

  const uint8_t *t = cityTexture + (v * CITY_TEXTURE_WIDTH + u) * 3;

  *r = *t;
  t++;
  *g = *t;
  t++;
  *b = *t;
}

uint32_t previousTriangle = -1;
S3L_Vec4 uv0, uv1, uv2;

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleID != previousTriangle)
  {
    const S3L_Index *uvIndices;
    const S3L_Unit *uvs;

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

    S3L_getIndexedTriangleValues(p->triangleIndex,uvIndices,uvs,2,&uv0,&uv1,&uv2);

    previousTriangle = p->triangleID;
  }

  uint8_t r, g, b;

  S3L_Unit uv[2];

  uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric);
  uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric);

  sampleTexture(uv[0] >> 1,uv[1] >> 1,&r,&g,&b);
  
  setPixel(p->x,p->y,r,g,b); 
}

void draw()
{
  S3L_newFrame();
  clearScreen();
  S3L_drawScene(scene);
}

static inline uint8_t collision(S3L_Vec4 worldPosition)
{
  worldPosition.x /= S3L_FRACTIONS_PER_UNIT;
  worldPosition.z /= -S3L_FRACTIONS_PER_UNIT;    

  uint16_t index = worldPosition.z * 8 + worldPosition.x;

  return collisionMap[index];
}

static inline void handleCollision(S3L_Vec4 *pos, S3L_Vec4 previousPos)
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
  SDL_Window *window = SDL_CreateWindow("city demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, S3L_RESOLUTION_X, S3L_RESOLUTION_Y, SDL_WINDOW_SHOWN); 
  SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
  SDL_Texture *textureSDL = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC, S3L_RESOLUTION_X, S3L_RESOLUTION_Y);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_Event event;

  cityModelInit();
  carModelInit();

  models[0] = cityModel;
  models[1] = carModel;

  S3L_sceneInit(models,2,&scene);

  S3L_transform3DSet(1909,16,-3317,0,-510,0,512,512,512,&(models[1].transform));

  int running = 1;

  clock_t nextPrintT;

  nextPrintT = clock();

  S3L_Vec4 carDirection;

  S3L_vec4Init(&carDirection);
  
  scene.camera.transform.translation.y = S3L_FRACTIONS_PER_UNIT / 2;
  scene.camera.transform.rotation.x = -S3L_FRACTIONS_PER_UNIT / 16;

  int16_t velocity = 0;

  while (running) // main loop
  {
    clock_t frameStartT = clock();

    models[1].transform.rotation.y += models[1].transform.rotation.z; // overturn the car for the rendering

    draw();

    models[1].transform.rotation.y -= models[1].transform.rotation.z; // turn the car back for the physics

    fps++;

    SDL_UpdateTexture(textureSDL,NULL,pixels,S3L_RESOLUTION_X * sizeof(uint32_t));

    clock_t nowT = clock();

    double timeDiff = ((double) (nowT - nextPrintT)) / CLOCKS_PER_SEC;
    double frameDiff = ((double) (nowT - frameStartT)) / CLOCKS_PER_SEC;
    int16_t frameDiffMs = frameDiff * 1000;

    if (timeDiff >= 1.0)
    {
      nextPrintT = nowT;
      printf("FPS: %d\n",fps);
      fps = 0;
    }

    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT)
        running = 0;

    const uint8_t *state = SDL_GetKeyboardState(NULL);

    int16_t step = (velocity * frameDiffMs) / 1000;
    int16_t stepFriction = (FRICTION * frameDiffMs) / 1000;
    int16_t stepRotation = TURN_SPEED * frameDiffMs * S3L_max(0,velocity - 200) / (MAX_VELOCITY * 1000);

    int16_t stepVelocity = S3L_nonZero((ACCELERATION * frameDiffMs) / 1000);

    if (stepRotation == 0 && S3L_abs(velocity) >= 200)
      stepRotation = 1;

    if (velocity < 0)
      stepRotation *= -1;

    if (state[SDL_SCANCODE_LEFT])
    {
      models[1].transform.rotation.y += stepRotation;
      models[1].transform.rotation.z =
        S3L_min(S3L_abs(velocity) / 64, models[1].transform.rotation.z + 1);
    }
    else if (state[SDL_SCANCODE_RIGHT])
    {
      models[1].transform.rotation.y -= stepRotation;
      models[1].transform.rotation.z =
        S3L_max(-S3L_abs(velocity) / 64, models[1].transform.rotation.z - 1);
    }
    else
      models[1].transform.rotation.z = (models[1].transform.rotation.z * 3) / 4;

    S3L_rotationToDirections(models[1].transform.rotation,S3L_FRACTIONS_PER_UNIT,&carDirection,0,0);

    S3L_Vec4 previousCarPos = models[1].transform.translation;

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
        friction = 2;
      }
      else if (coll == 2)
      {
        // teleport the car
        models[1].transform.translation.x += 5 * S3L_FRACTIONS_PER_UNIT;
        models[1].transform.translation.z += 2 * S3L_FRACTIONS_PER_UNIT;
      }
      else
      {
        // teleport the car
        models[1].transform.translation.x -= 5 * S3L_FRACTIONS_PER_UNIT;
        models[1].transform.translation.z -= 2 * S3L_FRACTIONS_PER_UNIT;
      }
    }

    if (velocity > 0)
      velocity = S3L_max(0,velocity - stepFriction * friction);
    else
      velocity = S3L_min(0,velocity + stepFriction * friction);

    S3L_Unit cameraDistance =
      S3L_interpolate(S3L_FRACTIONS_PER_UNIT / 2,(3 * S3L_FRACTIONS_PER_UNIT) / 4,S3L_abs(velocity),MAX_VELOCITY);

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
