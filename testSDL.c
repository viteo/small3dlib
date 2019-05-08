/*
  author: Miloslav Ciz
  license: CC0
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 640
#define S3L_RESOLUTION_Y 480

#include "s3l.h"

const int16_t test_coords[] =
  {
    100,100, 99,101,    101,101,  // 0, small triangle
    190,50,  200,10,    400,80,   // 1, arbitrary
    40,80,   60,50,     100,30,   // 2, arbitrary
    350,270, 440,200,   490,220,  // 3, arbitrary
    150,300, 290,400,   450,400,  // 4, regular
    105,200, 120,200,   201,200,  // 5, horizontal line
    300,200, 300,250,   300,220,  // 6, vertical line
    496,15,  613,131,   552,203
  };

const S3L_Unit ver[] = { S3L_CUBE_VERTICES };
const S3L_Index tri[] = { S3L_CUBE_TRIANGLES };
const S3L_Unit tex_coords[] = { S3L_CUBE_TEXCOORDS };

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

void drawPixel(S3L_PixelInfo *p)
{
  S3L_Unit u, v, *coords;

  coords = tex_coords + p->triangleID * 6;

  S3L_interpolateTexCoords(
    coords[0] * 16,coords[1] * 16,
    coords[2] * 16,coords[3] * 16,
    coords[4] * 16,coords[5] * 16,
    p,
    &u,&v);

  uint8_t col = texturePixel(u,v);

  setPixel(p->x,p->y,col * 120,20,(2 - col) * 120);

//  setPixel(p->x,p->y,p->barycentric0 / ((float) S3L_FRACTIONS_PER_UNIT) * 255,p->barycentric1 / ((float) S3L_FRACTIONS_PER_UNIT) * 255,p->barycentric2 / ((float) S3L_FRACTIONS_PER_UNIT) * 255);
}

S3L_Camera camera;
S3L_Transform3D modelTransform;
S3L_DrawConfig conf;

void draw()
{
  clearScreen();

  modelTransform.rotation.z = frame * 0.1;
  modelTransform.rotation.x = frame * 0.3;

  S3L_drawModelIndexed(ver,tri,12,modelTransform,camera,conf);

/*
conf.backfaceCulling = S3L_BACKFACE_CULLING_NONE;
conf.mode = S3L_MODE_TRIANGLES;

  for (int c = 0; c < 8; ++c)
  {
    int
      x0 = test_coords[6 * c],
      y0 = test_coords[6 * c + 1],
      x1 = test_coords[6 * c + 2],
      y1 = test_coords[6 * c + 3],
      x2 = test_coords[6 * c + 4],
      y2 = test_coords[6 * c + 5];

int cent = (x0 + x1 + x2) / 3.0;
x2 = cent + (x2 - cent) * sin(frame * 0.001) * 2;
//x2 = cent + (x2 - cent) * sin(600 * 0.001) * 2;

    S3L_drawTriangle(x0,y0,x1,y1,x2,y2,conf,0);

    setPixel(x0,y0,255,0,0);
    setPixel(x1,y1,255,0,0);
    setPixel(x2,y2,255,0,0);

  }

float rotRate = 0.002;

int16_t rotX0 = 200 + sin(frame * rotRate) * 100;
int16_t rotY0 = 200 + cos(frame * rotRate) * 100;

int16_t rotX1 = 200 + sin((frame + 1500) * rotRate) * 100;
int16_t rotY1 = 200 + cos((frame + 1500) * rotRate) * 100;

int16_t rotX2 = 200 + sin((frame + 500) * rotRate) * 100;
int16_t rotY2 = 200 + cos((frame + 500) * rotRate) * 100;

S3L_drawTriangle(rotX0,rotY0,rotX1,rotY1,rotX2,rotY2,conf,0);
  */
}

int main()
{
  SDL_Window *window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, S3L_RESOLUTION_X, S3L_RESOLUTION_Y, SDL_WINDOW_SHOWN); 
  SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
  SDL_Texture *texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC, S3L_RESOLUTION_X, S3L_RESOLUTION_Y);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_Event event;

  S3L_initCamera(&camera);
  camera.transform.translation.z = -S3L_FRACTIONS_PER_UNIT * 2;
//  camera.transform.translation.x = S3L_FRACTIONS_PER_UNIT;
//  camera.transform.translation.y = S3L_FRACTIONS_PER_UNIT;

  S3L_initTransoform3D(&modelTransform);
  S3L_initDrawConfig(&conf);

  int running = 1;

  while (running)
  {
    draw();
    SDL_UpdateTexture(texture,NULL,pixels,S3L_RESOLUTION_X * sizeof(uint32_t));

    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          running = 0;
          break;

        default:
          break;
      }
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);

    frame++;
  }

  return 0;
}
