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
#define S3L_PERSPECTIVE_CORRECTION 2
#define S3L_SORT 0
#define S3L_Z_BUFFER 1

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 640
#define S3L_RESOLUTION_Y 480

#include "../small3dlib.h"

#include "levelModel.h"
#include "levelTextures.h"

S3L_Model3D models[3];
S3L_Scene scene;

S3L_Vec4 teleportPoint;

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

uint8_t *texture = 0;
S3L_Unit *uvs = 0;
S3L_Index *uvIndices = 0;

uint32_t previousTriangle = 1000;

S3L_Vec4 uv0, uv1, uv2;

void sampleTxture(S3L_Unit u, S3L_Unit v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  u = (u * LEVEL_TEXTURE_WIDTH) / S3L_FRACTIONS_PER_UNIT;
  v = (v * LEVEL_TEXTURE_HEIGHT) / S3L_FRACTIONS_PER_UNIT;

  u = S3L_wrap(u,LEVEL_TEXTURE_WIDTH);
  v = S3L_wrap(v,LEVEL_TEXTURE_HEIGHT);

  uint32_t index = (v * LEVEL_TEXTURE_WIDTH + u) * 3;

  *r = texture[index];
  index++;
  *g = texture[index];
  index++;
  *b = texture[index];
}

void drawTeleport(int16_t x, int16_t y, S3L_ScreenCoord size)
{
  int16_t halfSize = size / 2;

  S3L_ScreenCoord x0 = S3L_max(0,x - halfSize);
  S3L_ScreenCoord x1 = S3L_min(S3L_RESOLUTION_X,x + halfSize);
  S3L_ScreenCoord y0 = S3L_max(0,y - halfSize);
  S3L_ScreenCoord y1 = S3L_min(S3L_RESOLUTION_Y,y + halfSize);

  for (S3L_ScreenCoord j = y0; j < y1; ++j)
    for (S3L_ScreenCoord i = x0; i < x1; ++i)
      setPixel(i,j,255,0,0);
}

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleID != previousTriangle)
  {
    if (p->modelIndex == 0)
    {
      uvs = levelWallsUVs;
      uvIndices = levelWallsUVIndices;
      texture = level1Texture;
    }
    else if (p->modelIndex == 1)
    {
      uvs = levelFloorUVs;
      uvIndices = levelFloorUVIndices;
      texture = level2Texture;
    }
    else
    {
      uvs = levelCeilingUVs;
      uvIndices = levelCeilingUVIndices;
      texture = level3Texture;
    }

    S3L_getIndexedTriangleValues(p->triangleIndex,uvIndices,uvs,2,&uv0,&uv1,&uv2);
    previousTriangle = p->triangleID;
  }

  S3L_Unit uv[2];

  uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric);
  uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric);

  uint8_t r, g, b;

  sampleTxture(uv[0],uv[1],&r,&g,&b);

  S3L_Unit fog = (p->depth * 8) / S3L_FRACTIONS_PER_UNIT;

  r = S3L_clamp(((S3L_Unit) r) - fog,0,255);
  g = S3L_clamp(((S3L_Unit) g) - fog,0,255);
  b = S3L_clamp(((S3L_Unit) b) - fog,0,255);

  setPixel(p->x,p->y,r,g,b); 
}

S3L_Transform3D modelTransform;
S3L_DrawConfig conf;

clock_t nextT;

int fps = 0;

void draw()
{
  S3L_newFrame();

  clearScreen();

  uint32_t f = frame;

  S3L_drawScene(scene);

  S3L_Vec4 screenPoint;

  project3DPointToScreen(teleportPoint,scene.camera,&screenPoint);

  if (screenPoint.w > 0 && 
      screenPoint.x >= 0 && screenPoint.x <= S3L_RESOLUTION_X &&
      screenPoint.y >= 0 && screenPoint.y <= S3L_RESOLUTION_Y &&
      screenPoint.z < S3L_zBufferRead(screenPoint.x,screenPoint.y)) 
    drawTeleport(screenPoint.x,screenPoint.y,screenPoint.w);

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

  teleportPoint.x = 6 * S3L_FRACTIONS_PER_UNIT;
  teleportPoint.y = -3 * S3L_FRACTIONS_PER_UNIT;
  teleportPoint.z = 2 * S3L_FRACTIONS_PER_UNIT;
  teleportPoint.w = S3L_FRACTIONS_PER_UNIT;

  nextT = clock();

  S3L_initCamera(&scene.camera);

  levelWallsModelInit();
  levelFloorModelInit();
  levelCeilingModelInit();

  scene.modelCount = 3;
  scene.models = models;

  scene.models[0] = levelWallsModel;
  scene.models[1] = levelFloorModel;
  scene.models[2] = levelCeilingModel;

  S3L_Unit scale = S3L_FRACTIONS_PER_UNIT / 3;

  S3L_Vec4 s;

  s.x = scale;
  s.y = scale;
  s.z = scale;

  scene.models[0].transform.scale = s;
  scene.models[1].transform.scale = s;
  scene.models[2].transform.scale = s;

  S3L_initTransoform3D(&modelTransform);
  S3L_initDrawConfig(&conf);

  int running = 1;

  while (running)
  {
    draw();
    SDL_UpdateTexture(texture,NULL,pixels,S3L_RESOLUTION_X * sizeof(uint32_t));

    int code = 0;

    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT)
        running = 0;

    S3L_Vec4 camF, camR;
    int step = 50;
 
    S3L_rotationToDirections(
      scene.camera.transform.rotation,
      step,
      &camF,
      &camR,
      0);

    uint8_t *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_A])
      scene.camera.transform.rotation.y += 1;
    else if (state[SDL_SCANCODE_D])
      scene.camera.transform.rotation.y -= 1;
    else if (state[SDL_SCANCODE_W])
      scene.camera.transform.rotation.x += 1;
    else if (state[SDL_SCANCODE_S])
      scene.camera.transform.rotation.x -= 1;

    if (state[SDL_SCANCODE_UP])
      S3L_vec3Add(&scene.camera.transform.translation,camF);
    else if (state[SDL_SCANCODE_DOWN])
      S3L_vec3Sub(&scene.camera.transform.translation,camF);
    else if (state[SDL_SCANCODE_LEFT])
      S3L_vec3Sub(&scene.camera.transform.translation,camR);
    else if (state[SDL_SCANCODE_RIGHT])
      S3L_vec3Add(&scene.camera.transform.translation,camR);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);

    frame++;
  }

  return 0;
}
