#include <SDL2/SDL.h>

uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];
  
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *textureSDL;
SDL_Surface *screenSurface;
SDL_Event event;

void sdlInit(void)
{
  window = SDL_CreateWindow("model viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, S3L_RESOLUTION_X, S3L_RESOLUTION_Y, SDL_WINDOW_SHOWN); 
  renderer = SDL_CreateRenderer(window,-1,0);
  textureSDL = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC, S3L_RESOLUTION_X, S3L_RESOLUTION_Y);
  screenSurface = SDL_GetWindowSurface(window);
}

void sdlEnd(void)
{
  SDL_DestroyTexture(textureSDL);
  SDL_DestroyRenderer(renderer); 
  SDL_DestroyWindow(window);
}

void sdlUpdate(void)
{
  SDL_UpdateTexture(textureSDL,NULL,pixels,S3L_RESOLUTION_X * sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer,textureSDL,NULL,NULL);
  SDL_RenderPresent(renderer);
}

void clearScreen(void)
{
  memset(pixels,200,S3L_RESOLUTION_X * S3L_RESOLUTION_Y * sizeof(uint32_t));
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

void sampleTexture(const uint8_t *tex, int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  u = S3L_wrap(u,TEXTURE_W);
  v = S3L_wrap(v,TEXTURE_H);

  const uint8_t *t = tex + (v * TEXTURE_W + u) * 4;

  *r = *t;
  t++;
  *g = *t;
  t++;
  *b = *t;
}

