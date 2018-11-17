#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define S3L_PIXEL_FUNCTION drawPixel

#include "s3l.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

uint32_t frame = 0;

void clearScreen()
{
  memset(pixels,0,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
}

static inline void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
  uint32_t r = red & 0x000000FF;
  r = r << 16;

  uint32_t g = green & 0x000000FF;
  g = g << 8;

  uint32_t b = blue & 0x000000FF;

  pixels[y * SCREEN_WIDTH + x] = r | g | b;
}

void drawPixel(S3L_PixelInfo *p)
{
  setPixel(p->x,p->y,255,255,0);
}

void draw()
{
  clearScreen();

  setPixel(90,50,255,0,0);
  setPixel(100,10,255,0,0);
  setPixel(300,80,255,0,0);

  S3L_drawTriangle(90,50,100,10,300,80);
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
