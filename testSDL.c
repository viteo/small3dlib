/*
  author: Miloslav Ciz
  license: CC0
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define S3L_PIXEL_FUNCTION drawPixel

#include "s3l.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

uint32_t frame = 0;

void clearScreen()
{
  memset(pixels,0,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
}

static inline void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
  uint32_t r = red & 0x000000FF;
  r = r << 24;

  uint32_t g = green & 0x000000FF;
  g = g << 16;

  uint32_t b = blue & 0x000000FF;
  b = b << 8;

  pixels[y * SCREEN_WIDTH + x] = r | g | b;
}

uint8_t texturePixel(int32_t u, int32_t v)
{
  u %= 16;
  v %= 16;

  return testTexture[v * 16 + u];
}

void drawPixel(S3L_PixelInfo *p)
{
  float b0 = p->barycentric0 / ((float) S3L_FRACTIONS_PER_UNIT);
  float b1 = p->barycentric1 / ((float) S3L_FRACTIONS_PER_UNIT);
  float b2 = p->barycentric2 / ((float) S3L_FRACTIONS_PER_UNIT);

  int32_t u = b0 * 0 + b1 * 0 + b2 * 16;
  int32_t v = b0 * 0 + b1 * 16 + b2 * 16;

  uint8_t col = texturePixel(u,v);

//  setPixel(p->x,p->y,col * 120,20,(2 - col) * 120);

  setPixel(p->x,p->y,
    p->barycentric0 / ((float) S3L_FRACTIONS_PER_UNIT) * 255,
    p->barycentric1 / ((float) S3L_FRACTIONS_PER_UNIT) * 255,
    p->barycentric2 / ((float) S3L_FRACTIONS_PER_UNIT) * 255);
}

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

void draw()
{
  clearScreen();

S3L_DrawConfig conf;

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

}

int main()
{
  SDL_Window *window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); 
  SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
  SDL_Texture *texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_Event event;

  int running = 1;

  while (running)
  {
    draw();
    SDL_UpdateTexture(texture,NULL,pixels,SCREEN_WIDTH * sizeof(uint32_t));

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
